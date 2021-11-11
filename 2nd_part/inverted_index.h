#pragma once

#include <istream>
#include <ostream>
#include <unordered_map>
#include <string>
#include <deque>
#include <string_view>
#include <vector>

using namespace std;

void RemovePrefixSpace(string_view& s);
string_view ReturnWord(string_view& s);
vector<string_view> SplitIntoWords(const string& s);

class InvertedIndex {
public:
    InvertedIndex() = default;
    
    explicit InvertedIndex(istream& input);
    
    struct Rating
    {
        int64_t num_docs = 0;
        int64_t rating = 0;
    };
    
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
