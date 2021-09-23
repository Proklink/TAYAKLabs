#pragma once
#include <stack>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <map>
#include "Func.h"
using namespace std;

extern map<std::string, uint8_t> priorities;

class Calc {
    stack<string> operators;
    stack<double> operands;

    bool evaluate();
public:
    vector<string> tokens;

    Calc(vector<string> *tokens);

    bool get_number(string& token);
    bool get_func(string& token);
    double calculate();
};

