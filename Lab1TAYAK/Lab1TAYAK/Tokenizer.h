#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

enum Lex {
    BEGIN,
    NUM,
    OP,
    OPEN_BRAKET,
    CLOSE_BRAKET,
    FUNC
};

class Tokenizer {
    string raw_string;
    uint32_t index;

    Lex prev_lex;
public:
    vector<string> tokens;

    Tokenizer() : index(0), raw_string(""), prev_lex(BEGIN) {}

    Tokenizer(string raw_str) : index(0), raw_string(raw_str), prev_lex(BEGIN) {}

    bool get_source_string();
    void add_num();
    void add_operator();
    void add_bracket();
    bool is_number(char ch);
    bool is_char(char ch);
    bool is_open_bracket(char ch);
    bool is_close_bracket(char ch);
    bool is_operator(char ch);
    int check_args(size_t comma_pos, size_t open_bracket_pos, size_t close_bracket_pos);
    void add_func(int end_func);
    int is_func();
    bool is_space(char ch);
    bool split();

};