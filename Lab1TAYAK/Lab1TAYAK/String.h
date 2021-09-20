#pragma once
#include <stack>
#include <string>
#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <map>

using namespace std;

class String {
    stack<string> operators;
    stack<double> operands;
    static string func;
    static map<string, uint8_t> priorities;

    double key_func(double a, double b);
    string get_num();
    string get_func();
    string get_operator();
    string get_next();
    bool check_func();
    bool evaluate();
public:
    string raw_string;
    size_t index;

    String();
    String(string raw_string);

    void get_source_string();
    double calculate();
};

