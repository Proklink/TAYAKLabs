#include "Tokenizer.h"
#include "Func.h"
#include "Calc.h"
#include <iostream>

using namespace std;

map<std::string, uint8_t> priorities = { {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}};

bool Tokenizer::get_source_string() {
    raw_string = "";
    string source = "";
    ifstream file("source.txt");

    if (file.is_open())
        while (getline(file, source))
            raw_string += source;

    file.close();
    
    if (raw_string == "")
        return false;

    return true;
}

void Tokenizer::add_num() {
    string number = "";

    while (index < raw_string.size() && is_number(raw_string[index])) {
        number += raw_string[index];
        index++;
    }

    tokens.push_back(number);
}

void Tokenizer::add_operator() {
    string op(1, raw_string[index]);
    index++;
    tokens.push_back(op);
}

bool Tokenizer::is_number(char ch) {
    return (ch > 47) && (ch < 58);
}

bool Tokenizer::is_char(char ch) {
    return (ch > 65) && (ch < 123);
}

bool Tokenizer::is_open_bracket(char ch) {
    return (ch == 40);
}

bool Tokenizer::is_close_bracket(char ch) {
    return (ch == 41);
}

bool Tokenizer::is_operator(char ch) {
    if ((ch == 42) || (ch == 43) || (ch == 45) || (ch == 47))
        return true;
    else
        return false;
}

void Tokenizer::add_func(int end_func) {
    string sfunc = raw_string.substr(index, end_func - index + 1);

    tokens.push_back(sfunc);
    index = end_func + 1;
}

int Tokenizer::check_args(size_t comma_pos, size_t open_bracket_pos, size_t close_bracket_pos) {
    int first, second;
    size_t begin_first_arg = open_bracket_pos + 1;
    size_t end_first_arg = comma_pos - 1;
    size_t begin_second_arg = comma_pos + 1;
    size_t end_second_arg = close_bracket_pos - 1;

    //подстрока первого аргумента        начало первого арг    конец второго аргумента
    string first_arg = raw_string.substr(begin_first_arg, end_first_arg - begin_first_arg + 1);
    //подстрока второго аргумента        
    string second_arg = raw_string.substr(begin_second_arg, end_second_arg - begin_second_arg + 1);

    //вычисляем первый аргумент
    Tokenizer tokenizer(first_arg);

    if (!tokenizer.split())
        return -1;
    Calc calc(&tokenizer.tokens);
    try {
        first = calc.calculate();
    }
    catch (...) {
        return -1;
    }

    //вычисляем второй аргумент
    tokenizer = Tokenizer(second_arg);
    if (!tokenizer.split())
        return -1;
    calc = Calc(&tokenizer.tokens);
    try {
        second = calc.calculate();
    }
    catch (...) {
        return -1;
    }

    string first_arg_str = to_string(first);
    string second_arg_str = to_string(second);
    //замещаем выражения для аргументов строчным представлением вычисленных аргументов
    raw_string.replace(begin_first_arg, end_first_arg - begin_first_arg + 1, first_arg_str);
    begin_second_arg = raw_string.find(',', begin_first_arg) + 1;
    end_second_arg = raw_string.find(')', begin_second_arg) - 1;
    raw_string.replace(begin_second_arg, end_second_arg - begin_second_arg + 1, second_arg_str);

    //возвращаем новую позицию закрывающей скобки
    return open_bracket_pos + first_arg_str.size() + 1 + second_arg_str.size() + 1;
}

int Tokenizer::is_func() {
    int pos = raw_string.find(func, index);

    if ((pos == index) && (raw_string[pos + 3] == '(')) {
        //int comma_count = 0;//, open_br_count = 1, close_br_count = 0;
        size_t i;
        int comma_pos = -1, close_bracket_pos = 0, level = 1;

        for (i = pos + 4; i < raw_string.size(); i++) {
            if (raw_string[i] == ',') {
                //comma_count++;
                if (level == 1)
                    comma_pos = i;
            }
            if (raw_string[i] == '(') {
                //open_br_count++;
                level++;
            }

            if (raw_string[i] == ')') {
                //close_br_count++;
                level--;
                close_bracket_pos = i;
            }
            if (level == 0)
                break;
        }

        if (level != 0 || comma_pos == -1)
            return -1;

        //проверка на корректность аргументов 

        close_bracket_pos = check_args(comma_pos, pos + 3, close_bracket_pos);
        if (close_bracket_pos == -1)
            return -1;

        return close_bracket_pos;
    }

    return -1;
}

bool Tokenizer::is_space(char ch) {
    return ch == 32;
}

void Tokenizer::add_bracket() {
    string br(1, raw_string[index]);
    index++;
    tokens.push_back(br);
}

bool Tokenizer::split() {
    index = 0;
    size_t open_bracket = 0;
    size_t close_bracket = 0;

    while (true) {
        if (index >= raw_string.size())
            break;
        if ((index == 0) && (is_operator(raw_string[index]) || is_close_bracket(raw_string[index])))
            return false;

        if (is_space(raw_string[index])) {
            index++;
            continue;
        }
        else if (is_number(raw_string[index])) {
            if (prev_lex != OP && prev_lex != BEGIN && prev_lex != OPEN_BRAKET)
                return false;
            add_num();
            prev_lex = NUM;
        }
        else if (is_char(raw_string[index])) {
            int ret = is_func();

            if (ret == -1 && prev_lex != OP)
                return false;

            add_func(ret);
            prev_lex = FUNC;
        }
        else if (is_operator(raw_string[index])) {
            if (prev_lex != FUNC && prev_lex != NUM && prev_lex != BEGIN)
                return false;
            prev_lex = OP;
            add_operator();
        } 
        else if (is_open_bracket(raw_string[index])) {
            open_bracket++;
            prev_lex = OPEN_BRAKET;
            add_bracket();
        }
        else if (is_close_bracket(raw_string[index])) {
            close_bracket++;
            prev_lex = CLOSE_BRAKET;
            add_bracket();
        }
    }

    if (open_bracket != close_bracket)
        return false;
    
    if (tokens.size() == 1) {
        try {
            double digit = stod(tokens[0]);
        }
        catch (...) {
            int pos = tokens[0].find(func);

            if (pos >= 0)
                return true;
            
            return false;
        }
        return true;
    }
    if (tokens.size() < 3)
        return false;

    for (string token : tokens)
        cout << token << " ";

    return true;
}