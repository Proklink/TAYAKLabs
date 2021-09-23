#include "Calc.h"
#include "Func.h"
#include <iostream>

using namespace std;


Calc::Calc(vector<string> *tokens) : tokens(*tokens) {}

bool Calc::evaluate() {
        double first_num, second_num;
        string oper;

        try {
            second_num = operands.top();
            operands.pop();
            first_num = operands.top();
            operands.pop();
            oper = operators.top();
            operators.pop();

            if (oper == "+") {
                operands.push(first_num + second_num);
            }
            else if (oper == "-") {
                operands.push(first_num - second_num);
            }
            else if (oper == "*") {
                operands.push(first_num * second_num);
            }
            else if (oper == "/") {
                operands.push(first_num / second_num);
            }
        }
        catch (...) {
            return false;
        }

        return true;
}

bool Calc::get_number(string& token) {
    double number;
    try {
        number = stod(token);
    }
    catch (...) {
        return false;
    }

    operands.push(number);
    return true;
}

bool Calc::get_func(string& token) {
    int ret = token.find(func);
    if (ret < 0)
        return false;

    int open_br = token.find('(');
    int comma = token.find(',');
    int close_br = token.find(')');

    if (open_br < 0 || comma < 0 || close_br < 0)
        return false;

    string sarg1 = token.substr(open_br + 1, comma - open_br - 1);
    string sarg2 = token.substr(comma + 1, close_br - comma - 1);
    double arg1, arg2;
    
    try {
        arg1 = stod(sarg1);
        arg2 = stod(sarg2);
    }
    catch (...) {
        return false;
    }

    operands.push(key_func(arg1, arg2));
    return true;
}

double Calc::calculate() {
    for (int i = 0; i < tokens.size(); i++) {
        string token = tokens[i];

        if (get_number(token)) {
            continue;
        } 
        else if (get_func(token)) {
            continue;
        }
        else if (token.size() == 1) {
            if (operators.empty() || operators.top() == "(") {
                operators.push(token);
                continue;
            }
            if (token == "(") {
                operators.push(token);
            }
            else if (token == ")") {
                while (operators.top() != "(") {
                    if (!evaluate())
                        throw "Error";
                }
                operators.pop();
            }
            else if (priorities[operators.top()] >= priorities[token]) {
                while (!operators.empty() && priorities[operators.top()] >= priorities[token])
                    if (!evaluate())
                        throw "Error";
                operators.push(token);
            }
            else {
                operators.push(token);
            }
        }

    }
    while (!operators.empty()) {
        if (!evaluate())
            throw "Error";
    }

    if (operands.size() > 1)
        throw "Error";

    return operands.top();
}

