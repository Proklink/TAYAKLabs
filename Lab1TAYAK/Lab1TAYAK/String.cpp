#include "String.h"
#include <iostream>

//using namespace std;

double String::key_func(double a, double b) {
    return pow(a, b);
}

    String::String() {
        raw_string = "";
        index = 0;
    }

    String::String(string raw_string) : index(0) {
        this->raw_string = raw_string;
    }

    string String::get_num() {
        string output = "";

        while (true) {
            output += raw_string[index];
            index++;

            if (raw_string[index] > 47 && raw_string[index] < 58)
                continue;
            else
                break;
        }

        return output;
    }

    string String::get_func() {
        bool ret = check_func();

        if (ret == false) {
            return "";
        }
        else {
            size_t comma = raw_string.find(',', index);
            String first_arg(raw_string.substr(index + func.length(), comma - 1));
            size_t pos = raw_string.find(')', index);
            String second_arg(raw_string.substr(comma + 1, pos - 1));

            double first = first_arg.calculate();
            double second = second_arg.calculate();

            index = pos + 1;

            return to_string(key_func(first, second));
        }
    }

    string String::get_operator() {
            string op(1, raw_string[index]);
            map<string, uint8_t>::iterator it = priorities.find(op);

            if (it != priorities.end() || (op == " ")) {
                index++;
                return op;
            }
            else {
                return "";
            }
                    
                
    }
        

    string String::get_next() {

        if (raw_string[index] > 47 && raw_string[index] < 58)
            return get_num();
        else if (raw_string[index] > 65 && raw_string[index] < 123)
            return get_func();
        else if ((raw_string[index] == 40) || (raw_string[index] == 41))
            return to_string(raw_string[index]);
        else
            return get_operator();
    }

    void String::get_source_string() {
        raw_string = "";
        string source = "";
        ifstream file("source.txt");

        if (file.is_open())
            while (getline(file, source))
                raw_string += source;

        file.close();
    }

    bool String::check_func() {
        size_t pos = raw_string.find(func, index - 1);

        if (pos == index - 1)
            return true;

        return false;
    }

    bool String::evaluate() {
        double first_num, second_num;
        string oper;

        try {
            first_num = operands.top();
            operands.pop();
            second_num = operands.top();
            operands.pop();
            oper = operators.top();
            operators.pop();

            if (oper == string("+")) {
                operands.push(first_num + second_num);
            }
            else if (oper == string("-")) {
                operands.push(first_num - second_num);
            }
            else if (oper == string("*")) {
                operands.push(first_num * second_num);
            }
            else if (oper == string("/")) {
                operands.push(first_num / second_num);
            }
        }
        catch (...) {
            cout << "The string does not match an arithmetic expression\n";
            return false;
        }

        return true;
    }

    double String::calculate() {
        while (index < raw_string.size()) {
            double number;
            string current = get_next();

            if (current == "") {
                throw string("Invalid source expression\n");
            }
            try {
                number = stoi(current);
                operands.push(number);
            }
            catch (invalid_argument inv_arg) {
                if (current == " ")
                    continue;
                if (current == "(" || operators.empty()) {
                    operators.push(current);
                }
                else if (operators.top() == ")") {
                    while (operators.top() != "(")
                        if (!evaluate())
                            throw string("Invalid source expression\n");
                    operators.pop();
                }
                else if (priorities[operators.top()] >= priorities[current]) {
                    while (!operators.empty() && (priorities[operators.top()] >= priorities[current]))
                        if (!evaluate())
                            throw string("Invalid source expression\n");
                    operators.push(current);
                }
                else if (priorities[operators.top()] < priorities[current]) {
                    operators.push(current);
                }

            }

        }
        if (!operators.empty()) {
            while (!operators.empty())
                if (!evaluate())
                    throw string("Invalid source expression\n");
            if (operands.size() > 1)
                throw string("Invalid source expression\n");
        }
        return operands.top();
    }

