#include "inverted_index.h"


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

InvertedIndex::InvertedIndex(istream& stream)
{
    for (string document; getline(stream, document); )
    {
        docs.push_back(move(document));
        int64_t docid = docs.size() - 1;
        for (auto word : SplitIntoWords(docs.back()))
        {
            if (!index[word].empty() && index[word].back().num_docs == docid)
            {
                ++index[word].back().rating;
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

