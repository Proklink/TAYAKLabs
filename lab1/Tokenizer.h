#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

//структура для предыдущих лексем
enum Lex {
    BEGIN,
    NUM,
    OP,
    OPEN_BRAKET,
    CLOSE_BRAKET,
    FUNC
};

class Tokenizer {
    string raw_string; //исходная строка
    uint32_t index; //текущий индекс
    bool minus; //флаг была ли предыдущая лексема унарным минусом

    Lex prev_lex; //предыдущая лексема
public:
    vector<string> tokens;

    Tokenizer() : index(0), raw_string(""), prev_lex(BEGIN), minus(false) {}

    Tokenizer(string raw_str) : index(0), raw_string(raw_str), prev_lex(BEGIN), minus(false) {}

    void reset();

    bool get_source_string();

    bool is_number(char ch);
    bool is_char(char ch);
    bool is_open_bracket(char ch);
    bool is_close_bracket(char ch);
    bool is_operator(char ch);
    bool is_space(char ch);
    bool is_separator(char ch);
    int is_func();
    bool is_minus(char ch);

    void add_func(int end_func);
    void add_num();
    void add_operator();
    void add_bracket();
    
    int check_args(size_t comma_pos, size_t open_bracket_pos, size_t close_bracket_pos);

    bool split();

};