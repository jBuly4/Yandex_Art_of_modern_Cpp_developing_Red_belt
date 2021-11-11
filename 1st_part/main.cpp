//
//  main.cpp
//  final_1st
//
//  Created by JB on 20.09.2021.
//

#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>

using namespace std;

void TestFunctionality(
  const vector<string>& docs,
  const vector<string>& queries,
  const vector<string>& expected
)
{
    istringstream docs_input(Join('\n', docs));
    istringstream queries_input(Join('\n', queries));

    SearchServer srv;
    
//        LOG_DURATION("UpdateDocumentBase1");
        srv.UpdateDocumentBase(docs_input);
    
    ostringstream queries_output;
    
//        LOG_DURATION("AddQueriesStream1");
        srv.AddQueriesStream(queries_input, queries_output);
    
    

    const string result = queries_output.str();
    const auto lines = SplitBy(Strip(result), '\n');
    ASSERT_EQUAL(lines.size(), expected.size());
    for (size_t i = 0; i < lines.size(); ++i)
    {
        ASSERT_EQUAL(lines[i], expected[i]);
    }
}

void TestFunctionality2(
  const vector<string>& docs,
  const vector<string>& queries,
  const vector<string>& expected,
  const vector<string>& docs2,
  const vector<string>& queries2,
  const vector<string>& expected2
)
{
    istringstream docs_input(Join('\n', docs));
    istringstream queries_input(Join('\n', queries));
    istringstream docs_input2(Join('\n', docs2));
    istringstream queries_input2(Join('\n', queries2));

    SearchServer srv;
    {
//        LOG_DURATION("UpdateDocumentBase1");
        srv.UpdateDocumentBase(docs_input);
    }
    ostringstream queries_output;
    {
//        LOG_DURATION("AddQueriesStream1");
        srv.AddQueriesStream(queries_input, queries_output);
    }
    

    const string result = queries_output.str();
    const auto lines = SplitBy(Strip(result), '\n');
    ASSERT_EQUAL(lines.size(), expected.size());
    for (size_t i = 0; i < lines.size(); ++i)
    {
        ASSERT_EQUAL(lines[i], expected[i]);
    }
    
    srv.UpdateDocumentBase(docs_input2);
    ostringstream queries_output2;
    srv.AddQueriesStream(queries_input2, queries_output2);
    const string result2 = queries_output2.str();
    const auto lines2 = SplitBy(Strip(result2), '\n');
    ASSERT_EQUAL(lines2.size(), expected2.size());
    for (size_t i = 0; i < lines2.size(); ++i)
    {
        ASSERT_EQUAL(lines2[i], expected2[i]);
    }
    
}

void TestSerpFormat()
{
    const vector<string> docs = {
        "london is the capital of great britain",
        "i am travelling down the river"
    };
    const vector<string> queries = {"london", "the"};
    const vector<string> expected = {
        "london: {docid: 0, hitcount: 1}",
        Join(' ', vector{ "the:",
            "{docid: 0, hitcount: 1}",
            "{docid: 1, hitcount: 1}" })
    };

    TestFunctionality(docs, queries, expected);
}

void TestTop5()
{
    const vector<string> docs = {
        "milk a",
        "milk b",
        "milk c",
        "milk d",
        "milk e",
        "milk f",
        "milk g",
        "water a",
        "water b",
        "fire and earth"
    };

    const vector<string> queries = {"milk", "water", "rock"};
    const vector<string> expected = {
        Join(' ', vector{
            "milk:",
            "{docid: 0, hitcount: 1}",
            "{docid: 1, hitcount: 1}",
            "{docid: 2, hitcount: 1}",
            "{docid: 3, hitcount: 1}",
            "{docid: 4, hitcount: 1}"
        }),
        Join(' ', vector{
            "water:",
            "{docid: 7, hitcount: 1}",
            "{docid: 8, hitcount: 1}",
        }),
        "rock:",
    };
    TestFunctionality(docs, queries, expected);
}

void TestHitcount()
{
    const vector<string> docs = {
        "the river goes through the entire city there is a house near it",
        "the wall",
        "walle",
        "is is is is",
    };
    const vector<string> queries = {"the", "wall", "all", "is", "the is"};
    const vector<string> expected = {
        Join(' ', vector{
            "the:",
            "{docid: 0, hitcount: 2}",
            "{docid: 1, hitcount: 1}",
        }),
        "wall: {docid: 1, hitcount: 1}",
        "all:",
        Join(' ', vector{
            "is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 1}",
        }),
        Join(' ', vector{
            "the is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 3}",
            "{docid: 1, hitcount: 1}",
        }),
    };
    TestFunctionality(docs, queries, expected);
}

void TestRanking()
{
    const vector<string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockgolm is the capital of sweden",
        "riga is the capital of latvia",
        "tallin is the capital of estonia",
        "warsaw is the capital of poland",
    };

    const vector<string> queries = {"moscow is the capital of russia"};
    const vector<string> expected = {
        Join(' ', vector{
            "moscow is the capital of russia:",
            "{docid: 7, hitcount: 6}",
            "{docid: 14, hitcount: 6}",
            "{docid: 0, hitcount: 4}",
            "{docid: 1, hitcount: 4}",
            "{docid: 2, hitcount: 4}",
        })
    };
    TestFunctionality(docs, queries, expected);
}

void TestBasicSearch()
{
    const vector<string> docs = {
        "we are ready to go",
        "come on everybody shake you hands",
        "i love this game",
        "just like exception safety is not about writing try catch everywhere in your code move semantics are not about typing double ampersand everywhere in your code",
        "daddy daddy daddy dad dad dad",
        "tell me the meaning of being lonely",
        "just keep track of it",
        "how hard could it be",
        "it is going to be legen wait for it dary legendary",
        "we dont need no education"
    };

    const vector<string> queries = {
        "we need some help",
        "it",
        "i love this game",
        "tell me why",
        "dislike",
        "about"
    };

    const vector<string> expected = {
        Join(' ', vector{
            "we need some help:",
            "{docid: 9, hitcount: 2}",
            "{docid: 0, hitcount: 1}"
        }),
        Join(' ', vector{
            "it:",
            "{docid: 8, hitcount: 2}",
            "{docid: 6, hitcount: 1}",
            "{docid: 7, hitcount: 1}",
        }),
        "i love this game: {docid: 2, hitcount: 4}",
        "tell me why: {docid: 5, hitcount: 2}",
        "dislike:",
        "about: {docid: 3, hitcount: 2}",
    };
    TestFunctionality(docs, queries, expected);
}

void TestCase3()
{
    const vector<string> docs = {
        "a   b c   d",
        "  a b c d",
        "   c  d e f",
        "d  e f    g"
    };

    const vector<string> queries = {
        "a",
        "b",
        "c",
        "d",
        "e",
        "f",
        "g",
        "b  c   d  f",
        "cde g",
        " c d",
        "  a b  ",
        "g    "
    };

    const vector<string> expected = {
        Join(' ', vector{
            "a:",
            "{docid: 0, hitcount: 1}", "{docid: 1, hitcount: 1}"
        }),
        Join(' ', vector{
            "b:",
            "{docid: 0, hitcount: 1}", "{docid: 1, hitcount: 1}"
        }),
        Join(' ', vector{
            "c:",
            "{docid: 0, hitcount: 1}", "{docid: 1, hitcount: 1}",
            "{docid: 2, hitcount: 1}"
        }),
        Join(' ', vector{
            "d:",
            "{docid: 0, hitcount: 1}", "{docid: 1, hitcount: 1}",
            "{docid: 2, hitcount: 1}", "{docid: 3, hitcount: 1}"
        }),
        Join(' ', vector{
            "e:",
            "{docid: 2, hitcount: 1}",
            "{docid: 3, hitcount: 1}",
        }),
        Join(' ', vector{
            "f:",
            "{docid: 2, hitcount: 1}",
            "{docid: 3, hitcount: 1}",
        }),
        "g: {docid: 3, hitcount: 1}",
        Join(' ', vector{
            "b  c   d  f:",
            "{docid: 0, hitcount: 3}",
            "{docid: 1, hitcount: 3}",
            "{docid: 2, hitcount: 3}",
            "{docid: 3, hitcount: 2}"
        }),
        "cde g: {docid: 3, hitcount: 1}",
        Join(' ', vector{
            " c d:",
            "{docid: 0, hitcount: 2}",
            "{docid: 1, hitcount: 2}",
            "{docid: 2, hitcount: 2}",
            "{docid: 3, hitcount: 1}"
        }),
        Join(' ', vector{
            "  a b  :",
            "{docid: 0, hitcount: 2}",
            "{docid: 1, hitcount: 2}",
        }),
        "g    : {docid: 3, hitcount: 1}",
    };
    TestFunctionality(docs, queries, expected);
}

void TestCase2()
{
    const vector<string> docs = {
        "the river goes through the entire city there is a house near it",
        "the wall",
        "walle",
        "is is is is",
    };
    const vector<string> queries = {"the", "wall", "all", "is", "the is"};
    const vector<string> expected = {
        Join(' ', vector{
            "the:",
            "{docid: 0, hitcount: 2}",
            "{docid: 1, hitcount: 1}",
        }),
        "wall: {docid: 1, hitcount: 1}",
        "all:",
        Join(' ', vector{
            "is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 1}",
        }),
        Join(' ', vector{
            "the is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 3}",
            "{docid: 1, hitcount: 1}",
        }),
    };
    
    const vector<string> docs2 = {
        "milk a",
        "milk b",
        "milk c",
        "milk d",
        "milk e",
        "milk f",
        "milk g",
        "water a",
        "water b",
        "fire and earth"
    };

    const vector<string> queries2 = {
        "milk",
        "water",
        "rock"
    };

    const vector<string> expected2 = {
        Join(' ', vector{
            "milk:",
            "{docid: 0, hitcount: 1}",
            "{docid: 1, hitcount: 1}",
            "{docid: 2, hitcount: 1}",
            "{docid: 3, hitcount: 1}",
            "{docid: 4, hitcount: 1}"
        }),
        Join(' ', vector{
            "water:",
            "{docid: 7, hitcount: 1}",
            "{docid: 8, hitcount: 1}",
        }),
        "rock:"
    };
    TestFunctionality2(docs, queries, expected, docs2, queries2, expected2);
}

void TestLoad() {

    default_random_engine rd(34);

    vector<string> key_words;
    ifstream word_input("k_words.txt"); // list of words from github
    for (string word; getline(word_input, word);) {
//        cout << word << "\n";
        key_words.push_back(word);
    }
//    cout << key_words.size() << endl;
    uniform_int_distribution<size_t> line_len_gen(1000, 1000); //max 1000 words in 1 doc

    ofstream text_out("docs_input.txt");
    for (int line = 0; line < 800; ++line) { // 50000 docs max
        ostringstream line_out;
        auto line_len = line_len_gen(rd);
        for (size_t i = 0; i < line_len; ++i) {
            uniform_int_distribution<size_t> word_index(0, key_words.size() - 1);
            line_out << key_words[word_index(rd)];
            line_out << ' ';
        }
        text_out << line_out.str() << '\n';
    }

    uniform_int_distribution<size_t> q_line_len_gen(10, 10); // [1; 10] words in query
    ofstream query_out("queries_input.txt");
    for (int line = 0; line < 3000; ++line) { //500000 queries max
        ostringstream line_out;
        auto line_len = q_line_len_gen(rd);
        for (size_t i = 0; i < line_len; ++i) {
            uniform_int_distribution<size_t> word_index(0, key_words.size() - 1);
            line_out << key_words[word_index(rd)];
            line_out << ' ';
        }
        query_out << line_out.str() << '\n';
    }

    ifstream docs_input("docs_input.txt");
    ifstream queries_input("queries_input.txt");

    SearchServer srv;
    {
//        LOG_DURATION("UpdateDocumentBase");
        srv.UpdateDocumentBase(docs_input);
    }
    ofstream queries_output("queries_output.txt");
    {
//        LOG_DURATION("AddQueriesStream");
        srv.AddQueriesStream(queries_input, queries_output);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSerpFormat);
    RUN_TEST(tr, TestTop5);
    RUN_TEST(tr, TestHitcount);
    RUN_TEST(tr, TestRanking);
    RUN_TEST(tr, TestBasicSearch);
    RUN_TEST(tr, TestCase2);
    RUN_TEST(tr, TestCase3);
    RUN_TEST(tr, TestLoad);
}

// UpdateDocumentBase - 2402 ms
// AddQueriesStream - 157939 ms
// AddQueriesStream - vec instead of list - 77276 ms
// AddQueriesStream 3 times less - 51744 ms
// AddQueriesStream 3 times less - vec instead of list - 25507 ms

// unordered_map:
// UpdateDocumentBase - 976 ms
// AddQueriesStream - 26251 ms

// vec for docid_count
// UpdateDocumentBase: 893 ms
// AddQueriesStream: 2143 ms

// full load
// UpdateDocumentBase: 910 ms
// AddQueriesStream: 6456 ms

// full load
//UpdateDocumentBase: 877936 mcs
//AddQueriesStream: 5554664 mcs
//UpdateDocumentBase: 903 ms
//AddQueriesStream: 5716 ms

// partial_sort
//UpdateDocumentBase: 897897 mcs
//AddQueriesStream: 5915630 mcs

//UpdateDocumentBase: 892837 mcs
//AddQueriesStream: 5434087 mcs

// LookUp - const vec&
//UpdateDocumentBase: 888 ms
//AddQueriesStream: 3086 ms
