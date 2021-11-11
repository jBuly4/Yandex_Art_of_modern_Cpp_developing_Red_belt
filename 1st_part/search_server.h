#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <deque>
#include <string_view>

using namespace std;

class InvertedIndex {
public:
    InvertedIndex() = default;
    explicit InvertedIndex(istream& input);
    struct Rating
    {
        size_t num_docs = 0;
        size_t rating = 0;
    };
    
//    void Add(string document);
    const vector<Rating>& Lookup(string_view word) const;

    const string& GetDocument(size_t id) const
    {
        return docs[id];
    }
    
    size_t GetSize() const
    {
        return docs.size();
    }
    
    inline static vector<Rating> empty_v = {};
private:
    
    unordered_map<string_view, vector<Rating>> index;
    deque<string> docs;
    
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
    InvertedIndex index;
};
