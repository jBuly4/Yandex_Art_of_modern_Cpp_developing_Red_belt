#include "search_server.h"
#include "iterator_range.h"
//#include "profile.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <future>


void UpdateSingleThread(istream& document_input, Synchronized<InvertedIndex>& old_index)
{
    InvertedIndex new_index(document_input);
    swap(old_index.GetAccess().ref_to_value, new_index);
}

void SearchServer::UpdateDocumentBase(istream& document_input)
{
    futures.push_back(
                      async(
                            UpdateSingleThread, ref(document_input), ref(index)
                            )
                      );
//    UpdateSingleThread(document_input, index);
}

void AddSingleThread(istream &query_input, ostream &search_results_output, Synchronized<InvertedIndex>& index)
{
    vector<InvertedIndex::Rating> docid_count;
    
    for (string current_query; getline(query_input, current_query); )
    {
        const vector<string_view> words = SplitIntoWords(current_query);
        {
            const auto& access = index.GetAccess();
            docid_count.assign(access.ref_to_value.GetSize(), {0, 0});
//            vector<InvertedIndex::Rating> docid_count(access.ref_to_value.GetSize(), {0, 0});
            
            auto& current_index = access.ref_to_value;
            for (const auto& word : words)
            {
                for (const auto& [docid, rating] : current_index.Lookup(word))
                {
                    docid_count[docid].num_docs = docid;
                    docid_count[docid].rating += rating;
                }
            }
        }

        partial_sort(docid_count.begin(), Head(docid_count, 5).end() /*docid_count.begin() + ((5 < docid_count.size()) ? 5 : docid_count.size())*/, docid_count.end(),
                             [&docid_count](InvertedIndex::Rating lhs, InvertedIndex::Rating rhs)
                             {
                               return pair(lhs.rating, -(lhs.num_docs)) > pair(rhs.rating, -(rhs.num_docs));
                             });
        search_results_output << current_query << ":";
        for (auto& [docid, hitcount] : Head(docid_count, 5))
        {
            if (hitcount != 0)
            {
                search_results_output << " {" << "docid: " << docid << ", " << "hitcount: " << hitcount << "}";
            }
        }
        search_results_output << "\n";
    }
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output)
{
    futures.push_back(
                      async(
                            AddSingleThread, ref(query_input), ref(search_results_output), ref(index)
                            )
                      );
//    AddSingleThread(query_input, search_results_output, index);
}

/*
 request #975: `sk udjfheys udjfheys eicu sk m seozerk sk:
 {docid: 107, hitcount: 33} {docid: 351, hitcount: 32} {docid: 422, hitcount: 29} {docid: 206, hitcount: 28} {docid: 322, hitcount: 28}` expected but got `sk udjfheys udjfheys eicu sk m seozerk sk:
 {docid: 107, hitcount: 32} {docid: 351, hitcount: 31} {docid: 322, hitcount: 27} {docid: 422, hitcount: 27} {docid: 206, hitcount: 26}
 */

/*
 request #963: `vtcodwmml n loqbm mlxmziuwtovnhi vtcodwmml udjfheys:
 {docid: 389, hitcount: 24} {docid: 48, hitcount: 23} {docid: 268, hitcount: 22} {docid: 300, hitcount: 22} {docid: 363, hitcount: 22}` expected but got `vtcodwmml n loqbm mlxmziuwtovnhi vtcodwmml udjfheys:
 {docid: 324, hitcount: 13} {docid: 333, hitcount: 13} {docid: 392, hitcount: 13} {docid: 286, hitcount: 12} {docid: 455, hitcount: 12}
 */

/*
 1000 lines expected in the output but 249 lines found (Time used: 0.42/200.00, preprocess time used: 0/None, memory used: 12414976/805306368.)
 */

/*
 request #2069: `n ndkdvkbpigkiq eicu:
 {docid: 19, hitcount: 13} {docid: 218, hitcount: 13} {docid: 305, hitcount: 12} {docid: 355, hitcount: 12} {docid: 203, hitcount: 11}` expected but got `n ndkdvkbpigkiq eicu:
 {docid: 268, hitcount: 10} {docid: 329, hitcount: 10} {docid: 355, hitcount: 10} {docid: 19, hitcount: 9} {docid: 203, hitcount: 9}`
 */
