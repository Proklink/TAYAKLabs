#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

#define END_ROW "\'$\'"

struct data_token {
    int num_row;
    string token;
};

enum err_type {
    undef,
    term_mismatch,
    void_cell,
    unknown_term
};

class Analyzier {
public:
    string start_lex = ""; 
    map<string, vector<string>> grammars;
    set<string> terms;
    set<string> nonTerms;
    set<string> grammar_lexems;
    map<string, set<string>> follow;
    map<string, set<string>> first;
    map<string, map<string, pair<string, string>>> table;
    int err_count;
    string o_table;
    
    bool is_term(string lex) {
        int pos = lex.find('\'');

        if (pos > -1) 
            return true;
        else
            return false;
    }

    string get_lex_next(string &instruction, int *_pos = NULL) {
        int close_sym, pos;

        if (_pos) {
            if (*_pos < 0)
                pos = 0;
            else 
                pos = *_pos;
        } else 
            pos = 0;

        for (int i = pos; i < instruction.size(); i++) {
            if (instruction[i] == '\'') {
                close_sym = instruction.find('\'', i + 1);
                *_pos = close_sym;
                return instruction.substr(i, close_sym - i + 1);
            }
            if (instruction[i] == '<') {
                close_sym = instruction.find('>', i + 1);
                *_pos = close_sym;
                return instruction.substr(i, close_sym - i + 1);
            }
        }
        return "";
    }

    string get_lex_before(string &instruction, int *_pos = NULL) {
        int open_sym;
        int pos;

        if (_pos) {
            if (*_pos > (int)instruction.size())
                pos = instruction.size() - 1;
            else
                pos = *_pos;
        }
        else {
            pos = instruction.size() - 1;
        }

        for (int i = pos; i >= 0; i--) {
            if (instruction[i] == '\'') {
                open_sym = instruction.find_last_of('\'', i - 1);
                *_pos = open_sym;
                return instruction.substr(open_sym, i - open_sym + 1);
            }
            if (instruction[i] == '>') {
                open_sym = instruction.find_last_of('<', i - 1);
                *_pos = open_sym;
                return instruction.substr(open_sym, i - open_sym + 1);
            }
        }
        return "";
    }

#define foreach_lexems(pos, lexems, lex) for (lex = get_lex_next(lexems, pos); *pos < lexems.size(); \
                                              *pos += 1, lex = get_lex_next(lexems, pos))
#define foreach_lexems_revert(pos, lexems, lex) for (lex = get_lex_before(lexems, pos); *pos >= 0; \
                                               *pos -= 1, lex = get_lex_before(lexems, pos))

    
    void print_grammars() {
        for (auto it : grammars) {
            cout << it.first << " : ";
            for (string str : it.second)
                cout << str << "   ";
            cout << endl;
        }
    }

    void print_map_of_set(map<string, set<string>> &m) {
        for (auto it : m) {
            cout << it.first << " : ";
            for (string str : it.second)
                cout << str << "   ";
            cout << endl;
        }
    }

    void print_follows() {
        cout << "follow:\n";
        print_map_of_set(follow);
    }

    void print_firsts() {
        cout << "first:\n";
        print_map_of_set(first);
    }

    void print_table() {
        cout << endl;
        for (auto it : table) {
            cout << it.first << ":";
            for (auto _it : table[it.first]) {
                cout << " " << _it.first << ": " << _it.second.first << "->" << _it.second.second << endl;
            }
            cout << endl;
        }
        cout << endl;
        
    }

    void parsing_rule(string &rule) {
        // int close_sym;

        // for (int i = 0; i < rule.size(); i++) {
        //     if (rule[i] == '\'') {
        //         close_sym = rule.find('\'', i + 1);
        //         terms.insert(rule.substr(i, close_sym - i + 1));
        //         i = close_sym;
        //         continue;
        //     }
        //     if (rule[i] == '<') {
        //         close_sym = rule.find('>', i + 1);
        //         terms.insert(rule.substr(i, close_sym - i + 1));
        //         i = close_sym;
        //         continue;
        //     }
        // }
        int pos = 0;
        string lexem;

        foreach_lexems(&pos, rule, lexem) {
            if (is_term(lexem))
                terms.insert(lexem);
            else
                nonTerms.insert(lexem);
        }
    }

    bool get_grammars(string &filename) {
        string line;
        ifstream file(filename);

        if (!file.is_open())
            return false;

        while (getline(file, line))
        {
            vector<string> rules;
            int delimiter = line.find(':');
            int stick = line.find('|');
            int prev_stick = delimiter;
            string nonTerm = line.substr(0, delimiter);

            if (start_lex == "")
                start_lex = nonTerm;

            while (stick > 0) {
                string rule = line.substr(prev_stick + 1, stick - prev_stick - 1);

                parsing_rule(rule);
                rules.push_back(rule);
                prev_stick = stick;
                stick = line.find('|', stick + 1);
            }

            string ended_rule = line.substr(prev_stick + 1, line.size() - delimiter - 1);

            if (ended_rule != "") {
                parsing_rule(ended_rule);
                rules.push_back(ended_rule);
            }

            nonTerms.insert(nonTerm);
            grammars[nonTerm] = rules;
        }
        file.close();
        
        grammar_lexems.insert(nonTerms.begin(), nonTerms.end());
        grammar_lexems.insert(terms.begin(), terms.end());
        FIRST();
        FOLLOW();
        buildTable();
        return true;
    }

    set<string> first_for(string lexems) {
        int pos = 0;
        set<string> first_set;
        string lex;

        // while (pos < lexems.size()) {
        //     string lex = get_lex_next(lexems, pos);
        //     //auto it = first.find(lex);
        //     //if (it != first.end()) {
        //         first_set = first[lex];
        //         if (first_set.size() == 1 && *first_set.begin() == "e")
        //             pos += lex.size();
        //         else
        //             break;
        //     //}
        // }
        foreach_lexems(&pos, lexems, lex) {
            if (is_term(lex)) {
                if (lex == END_ROW) 
                    first_set.insert("e");
                else
                    first_set.insert(lex);
            return first_set;
            } else {
                auto it = first.find(lex);

                if (it != first.end()) {
                    first_set = first[lex];
                    if (first_set.size() == 1 && *first_set.begin() == "e")
                        continue;
                    else
                        break;
                }
            }
        }
        return first_set;
    }

    set<string> FIRST(string lex) {
        set<string> first_set;

        if (is_term(lex)) {
            if (lex == END_ROW) 
                first_set.insert("e");
            else
                first_set.insert(lex);
            return first_set;
        } else {
            for (string prod : grammars[lex]) {
                int pos = 0;
                set<string> temp_first_set;
                string lex;

                foreach_lexems(&pos, prod, lex) {
                    temp_first_set = FIRST(lex);
                    if (temp_first_set.size() == 1 && *temp_first_set.begin() == "e") {
                        continue;
                    } else
                        break;
                }
                first_set.insert(temp_first_set.begin(), temp_first_set.end());
            }
            first[lex].insert(first_set.begin(), first_set.end());
            return first_set;
        }
        
    }

    void FIRST() {
        bool _continue;
        do { 
            _continue = false;
            //Для каждого нетерминала X
            for (string nonTerm : nonTerms) {
	            //Для каждого правила X -> Y1Y2...Yk 
                auto it = first.find(nonTerm);
                int old_size = 0;

                if (it != first.end())
                    old_size = first[nonTerm].size();
                
                FIRST(nonTerm);
                if (old_size != first[nonTerm].size())
                    _continue = true;
            }
        } while (_continue);
    }

    void FOLLOW() {
        follow[start_lex].insert(END_ROW);
        for (string gr_lex : nonTerms) {
            for (auto entry : grammars) {
                for (string prod : entry.second) {
                    int pos = prod.find(gr_lex, 0);
                    if (pos < 0)
                        continue;

                    string lexems_after = prod.substr(pos + gr_lex.size(), prod.size() - (pos + gr_lex.size()));
                    if (lexems_after == "") {
                        follow[gr_lex].insert(END_ROW);
                        continue;
                    }

                    set<string> first_for_lexems_after = first_for(lexems_after);
                    for (string l : first_for_lexems_after) 
                        if (l == "e")
                            follow[gr_lex].insert(END_ROW);
                        else 
                            follow[gr_lex].insert(l);
                }
            }
        }
        bool _continue;
        do { 
            _continue = false;
            for (auto entry : grammars) {
                for (string prod : entry.second) {
                    int pos = prod.size() - 1;
                    string last_lex = get_lex_before(prod, &pos);

                    if (is_term(last_lex))
                        continue;
                    if (last_lex != entry.first) {
                        int old_size = follow[last_lex].size();

                        follow[last_lex].insert(follow[entry.first].begin(), follow[entry.first].end());
                        
                        if (old_size != follow[last_lex].size())
                            _continue = true;
                    }

                    //если во множестве first у последней лексемы(last_lex) в продукции(prod) содержится е, 
                    //то есть last_lex может вырождаться в пустой символ (е), то в таком случае
                    //предпоследняя(lex_before_last_lex) лексема продукции(prod) тоже должна содержать
                    //в своём множестве follow элементы множества follow лексемы entry.first из текущего 
                    //правила грамматики:
                    // (entry.first -> <some lexems>...<lex_before_last_lex><last_lex>)
                    if (first[last_lex].find("e") != first[last_lex].end()) {
                        //int ppos = pos - last_lex.size(); 
                        string lex_before_last_lex = get_lex_before(prod, &pos);

                        //проверим, что last_lex - не единственная лексема в продукции(prod) и,
                        //если она не единственная, что - нетерминальная
                        if (lex_before_last_lex != "" && !is_term(lex_before_last_lex)) {
                            int old_size = follow[lex_before_last_lex].size();
                            
                            follow[lex_before_last_lex].insert(follow[entry.first].begin(), follow[entry.first].end());
                            if (old_size != follow[lex_before_last_lex].size())
                                _continue = true;
                        }
                    }
                }
            }
        } while (_continue);
    }
    
    void buildTable() {
        for (string n_term : nonTerms)
            for (string _term : terms)
                table[n_term][_term] = pair<string, string>();
        for (auto entry : grammars) {
            for (string prod : entry.second) {
                int pos = 0;
                string lexems;
                set<string> first_for_prod = first_for(prod);
                bool has_e = false;
                
                for (string term : first_for_prod) {
                    if (term == "e") {
                        has_e = true;
                        continue;
                    } else
                        table[entry.first][term] = make_pair(entry.first, prod);
                }
                if (has_e) {
                    bool has_end_row = false;
                    set<string> follow_entry_first = follow[entry.first];

                    for (string _term : follow_entry_first) {
                        if (_term == END_ROW) {
                            has_end_row = true;
                            continue;
                        }
                        table[entry.first][_term] = make_pair(entry.first, "e");
                    }
                    if (has_e && has_end_row)
                        table[entry.first][END_ROW] = make_pair(entry.first, "e");
                }
            }
        }
    }

    void error(err_type type, int num_row, string &expected, string &got) {
        err_count++;
        cout << "Error in row " << num_row << ": ";
        if (type == term_mismatch)
            cout << "Term mismatch. ";
        else 
            cout << "Empty cell in table. ";
        cout << "Expected " << expected << ", but got " << got << endl;
    }

    string get_term_from_str(string &str, int pos) {
        for (string term : terms) {
            int _pos = str.find(term, pos);
            if (_pos == 0)
                return str.substr(_pos, term.size());
            else 
                return "";
        }
    }

    bool tokenize(vector<data_token> &data, string filename) {
        ifstream file(filename);
        string line;
        int row_count = 0;

        if (!file.is_open())
            return false;

        while (getline(file, line)) {
            row_count++;
            if (line == "")
                continue;
            stringstream input(line);
            
            while (!input.eof()) {
                string base;
                data_token token;

                input >> token.token;

                token.token = "\'" + token.token + "\'";
                token.num_row = row_count;
                data.push_back(token);
            }
        }
        file.close();
        return true;
    }

    void log(vector<data_token> &data, int i, err_type err, stack<string> pushdown) {
        string output_table_row = "";
        string token_colomn;
        string pushdown_colomn;
        int next_data_row = data[i].num_row + 1;

        while (pushdown.size() > 0) {
            pushdown_colomn.insert(0, pushdown.top());
            pushdown.pop();
        }

        while (i < data.size() && data[i].num_row < next_data_row) {
            token_colomn += data[i].token;
            i++;
        }
        
        output_table_row += pushdown_colomn + " | " + token_colomn;

        if (err == term_mismatch)
            output_table_row += "   | Error. Term mismatch\n";
        else if (err == void_cell)
            output_table_row += "   | Error. Empty cell in table\n";
        else if (err == unknown_term)
            output_table_row += "   | Error. Unknown term\n";
        else 
            output_table_row += "\n";
        o_table += output_table_row;
    }

    void analyze(vector<data_token> &data) {
        stack<string> pushdown;
        int i = 0;

        err_count = 0;
        pushdown.push("\'$\'");
        pushdown.push(start_lex);

        while (i < data.size() && !pushdown.empty()) {
            if (is_term(pushdown.top())) {
                if (pushdown.top() == data[i].token) {
                    log(data, i, undef, pushdown);
                    pushdown.pop();
                    i++;
                } else {
                    error(term_mismatch, data[i].num_row, pushdown.top(), data[i].token);
                    log(data, i, term_mismatch, pushdown);
                    pushdown.pop();
                }
            } else {
                auto it = table[pushdown.top()].find(data[i].token);
                auto empty_cell = make_pair(string(""), string(""));

                if (it != table[pushdown.top()].end()) {
                    if (table[pushdown.top()][data[i].token] == empty_cell) {
                        error(void_cell, data[i].num_row, pushdown.top(), data[i].token);
                        log(data, i, void_cell, pushdown);
                        //найти в first или follow след символ входа
                        auto _it = follow[pushdown.top()].find(data[i].token);

                        if (_it != follow[pushdown.top()].end())
                            pushdown.pop();
                        else
                            i++;
                    } else {
                        log(data, i, undef, pushdown);
                        string nonTerm = pushdown.top();
                        pushdown.pop();
                        string prod = table[nonTerm][data[i].token].second;
                        //сюда можно вставить проверку на e для надежности
                        int pos = prod.size() - 1;
                        string lexem;

                        foreach_lexems_revert(&pos, prod, lexem)
                            if (lexem != "")
                                pushdown.push(lexem);                     
                    }
                } else {
                    error(unknown_term, data[i].num_row, pushdown.top(), data[i].token);
                    log(data, i, unknown_term, pushdown);  
                    i++;
                }
            }
        }
        //if (pushdown.empty() && i >= data.size())
            
        cout << "Error count = " << err_count << endl;
    }

    void put_to_file(ofstream &output, string &s) {
        if (output.is_open()) {
            output << s;
        }
    }

    void print_output_table() {
        stringstream out_table(o_table);
        string token;
        int max1 = 0, max2 = 0, max3 = 0;

        while (getline(out_table, token, '\n')) {
            stringstream t(token);
            int stick_count = 0;
            string piece;

            while (getline(t, piece, '|')) {
                if (stick_count == 0)
                    max1 = max(max1, (int)piece.size());
                else if (stick_count = 1)
                    max2 = max(max2, (int)piece.size());
                else if (stick_count = 2)
                    max3 = max(max3, (int)piece.size());
                stick_count++;
            }
        }
        out_table = stringstream(o_table);
        ofstream output("output_table.txt");
        
        cout << "Table\n";
        while (getline(out_table, token, '\n')) {
            stringstream t(token);
            int stick_count = 0;
            string piece;

            while (getline(t, piece, '|')) {
                if (stick_count == 0) {
                    cout << right << setw(max1) << piece << "||";
                    output << right << setw(max1) << piece << "||";
                }
                else if (stick_count = 1) {
                    cout << left << setw(max2) << piece << "||";
                    output << left << setw(max2) << piece << "||";
                }
                else if (stick_count = 2) {
                    cout << setw(max3) << piece;
                    output << setw(max3) << piece;
                }
                stick_count++;
            }
            cout << endl;
            output << endl;
        }
        output.close();
    }
};

int main() {
    Analyzier an;
    vector<data_token> data;
    string grammar_file;
    string source_code_file;
    //0 - run test, 1 - run cfile
    int run = 1;

    if (run == 0) {
        grammar_file = "testgrammar.txt";
        source_code_file = "testfile.txt";
    } else if (run == 1) {
        grammar_file = "cgrammar.txt";
        source_code_file = "cfile.txt";
    }

    an.get_grammars(grammar_file);
    an.print_grammars();
    an.print_firsts();
    an.print_follows();
    an.print_table();
    
    an.tokenize(data, source_code_file);
    an.analyze(data);
    cout << an.o_table << endl;
    an.print_output_table();

    return 0;
}