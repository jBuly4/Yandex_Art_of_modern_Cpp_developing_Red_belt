#pragma once

#include "inverted_index.h"
#include "synchronized.h"

#include <istream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <deque>
#include <string_view>
#include <future>

using namespace std;


class SearchServer {
public:
    SearchServer() = default;
//    ~SearchServer();
    explicit SearchServer(istream& document_input) : index(InvertedIndex(document_input)) {};
//    void UpdateSingleThread(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
//    void AddSingleThread(istream& query_input, ostream& search_results_output);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
    Synchronized<InvertedIndex> index;
    vector<future<void>> futures;
};
