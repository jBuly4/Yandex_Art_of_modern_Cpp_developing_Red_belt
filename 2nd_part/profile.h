//
//  profile.h
//  assignment1_student_rating
//
//  Created by research on 26.05.2021.
//

#pragma once

//#include <chrono>
//#include <iostream>
//#include <string>
//
//using namespace std;
//using namespace std::chrono;
//
//class LogDuration {
//public:
//  explicit LogDuration(const string& msg = "")
//    : message(msg + ": ")
//    , start(steady_clock::now())
//  {
//  }
//
//  ~LogDuration() {
//    auto finish = steady_clock::now();
//    auto dur = finish - start;
////    cerr << message
////       << duration_cast<microseconds>(dur).count()
////       << " mcs" << endl;
//    cerr << message
//        << duration_cast<milliseconds>(dur).count()
//        << " ms" << endl;
//  }
//private:
//  string message;
//  steady_clock::time_point start;
//};
//
//#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
//#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)
//
//#define LOG_DURATION(message) \
//  LogDuration UNIQ_ID(__LINE__){message};

#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

struct TotalDuration {
        string message;
        steady_clock::duration value;

        explicit TotalDuration(const string& msg);
        ~TotalDuration();
};
istream& ReadLine(istream& input, string& s, TotalDuration& dest);
class AddDuration {
    public:
        explicit AddDuration(steady_clock::duration& dest);
        explicit AddDuration(TotalDuration& dest);

        ~AddDuration();

    private:
        steady_clock::duration& add_to;
        steady_clock::time_point start;
};

#define MY_UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define MY_UNIQ_ID(lineno) MY_UNIQ_ID_IMPL(lineno)

#define ADD_DURATION(value) \
    AddDuration MY_UNIQ_ID(__LINE__){value};


