#include "search_server.h"
#include "iterator_range.h"
//#include "profile.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

void RemovePrefixSpace(string_view& s)
{
    while (!s.empty() && isspace(s[0]))
    {
        s.remove_prefix(1);
    }
}

string_view ReturnWord(string_view& s)
{
    RemovePrefixSpace(s);
    size_t pos = s.find(' ');
    string_view resul_w = s.substr(0, pos);
    if (pos != s.npos)
    {
        s.remove_prefix(pos);
    } else {
        s.remove_prefix(s.size());
    }
    
    return resul_w;
}

vector<string_view> SplitIntoWords(const string& s)
{
    string_view str = s;
    vector<string_view> result;
    
    for (string_view word = ReturnWord(str); !word.empty(); word = ReturnWord(str))
    {
        result.push_back(word);
    }
    return result;
}

SearchServer::SearchServer(istream& document_input)
{
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input)
{
//    TotalDuration build_index("  Build_index");
//    ADD_DURATION(build_index);
    index = InvertedIndex(document_input);
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output)
{
    vector<InvertedIndex::Rating> docid_count(index.GetSize(), {0, 0});
//    TotalDuration words_split("  Words split");
//    TotalDuration lookup("  Lookup");
//    TotalDuration sorting("  Sort");
//    TotalDuration build_results("  Build results");
//    TotalDuration total_iteration("  Total loop iteration");
    
    for (string current_query; getline(query_input, current_query); ) // 500 000
    {
//        ADD_DURATION(total_iteration);
        vector<string_view> words;
        {
//            ADD_DURATION(words_split);
            words = SplitIntoWords(current_query);
        }
        {
//            ADD_DURATION(lookup);
            for (const auto& word : words) // 10
            {
                for (const auto& [docid, rating] : index.Lookup(word)) // 50 000
                {
                    docid_count[docid].num_docs = docid;
                    docid_count[docid].rating += rating;
                }
            }
        }
        
        {
//            ADD_DURATION(sorting);
            partial_sort(docid_count.begin(), docid_count.begin() + ((5 < docid_count.size()) ? 5 : docid_count.size()), docid_count.end(),
                         [](InvertedIndex::Rating lhs, InvertedIndex::Rating rhs)
                         {
                           return make_pair(lhs.rating, -static_cast<int64_t>(lhs.num_docs)) > make_pair(rhs.rating, -static_cast<int64_t>(rhs.num_docs));
                         });
        }
//        ADD_DURATION(build_results);
        search_results_output << current_query << ":";
        for (auto& [docid, hitcount] : Head(docid_count, 5))
        {
            if (hitcount != 0)
            {
                search_results_output << " {" << "docid: " << docid << ", " << "hitcount: " << hitcount << "}";
            }
        }
        search_results_output << "\n";
        docid_count.assign(index.GetSize(), {0,0});
    }
}

InvertedIndex::InvertedIndex(istream& stream)
{
    
    for (string document; getline(stream, document); )
    {
        docs.push_back(move(document));
        size_t docid = docs.size() - 1;
        for (auto word : SplitIntoWords(docs.back()))
        {
            if (!index[word].empty() && index[word].back().num_docs == docid)
            {
                index[word].back().rating++;
            } else {
                index[word].push_back({docid, 1});
            }
        }
    }
}

const vector<InvertedIndex::Rating>& InvertedIndex::Lookup(string_view word) const
{
    if (auto it = index.find(word); it != index.end())
    {
        return index.at(word);
    } else {
        return empty_v;
    }
}

