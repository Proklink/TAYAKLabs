#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <algorithm>

using namespace std;

class src_and_pd {
public:
    src_and_pd(string src, string pd) : source(src), pushdown(pd) {}

    string source;
    string pushdown;
};

class source_line {
    string line;
    int curr_ind;
public:

    source_line(string line) {
        this->line = line;
        curr_ind = 0;
    }

    void reset(int has_change) {
        if (curr_ind > 0)
                curr_ind -= has_change;
    }

    char get() {
        return line[curr_ind];
    }

    void pop() {
        curr_ind++;
    }

    string get_remains() {
        return line.substr(curr_ind, line.size() - curr_ind + 1);
    }

    bool is_empty() {
        return curr_ind == line.size();
    }

    int size() {
        return line.size();
    }
};

class pushdown_automaton {
private:
    map<char, vector<string>> grammars;
    vector<string> pushdowns_tree;
    set<char> terms;
    set<char> nonTerms;
    
public:

    bool is_nonTerm(char c) {
        return (c > 64 && c < 91);
    }

    void parsing_rule(string &rule) {
        for (char c : rule)
            if (c < 65 || c > 90)
                terms.insert(c);
            else
                nonTerms.insert(c);

    }

    bool get_grammars(string &filename) {
        string line;
        ifstream file(filename);

        if (!file.is_open())
            return false;

        while (getline(file, line))
        {
            vector<string> rules;
            int angle_bracket = line.find('>');
            int stick = line.find('|');
            int prev_stick = angle_bracket;
            string nonTerm = line.substr(0, 1);

            while (stick > 0) {
                string rule = line.substr(prev_stick + 1, stick - prev_stick - 1);

                parsing_rule(rule);
                rules.push_back(rule);
                prev_stick = stick;
                stick = line.find('|', stick + 1);
            }

            string ended_rule = line.substr(prev_stick + 1, line.size() - angle_bracket - 1);

            if (ended_rule != "");
                rules.push_back(ended_rule);

            if (pushdowns_tree.size() == 0)
                pushdowns_tree.push_back(nonTerm);
            nonTerms.insert(nonTerm[0]);

            grammars.emplace(nonTerm[0], rules);
        }
        file.close();
        
        return true;
    }

    vector<string> substitution_nonTerm(string &pushdown, int nonTermIndex) {
        //выделяем часть строки до нетерминального символа 
        string leftPart_pushdown = pushdown.substr(0, nonTermIndex); 
        //получаем все элементы по правилу элемента pushdown[i]
        vector<string> nodes = grammars[pushdown[nonTermIndex]];

        //добавляем ко всем элементам, полученным по правилу то, что стояло 
        //в предыдущем элементе (pushdown) до pushdown[i]
        for (int j = 0; j < nodes.size(); j++)
            nodes[j].insert(nodes[j].begin(), leftPart_pushdown.begin(), leftPart_pushdown.end());

        return nodes;
    }

    void check_line(source_line &line) {
        //будем продвигаться по вектору всех вохможных магазинов, формируя его находу
        //будем формировать след образом: B>a|c {aB, b} -> {aa, ac, b}
        vector<src_and_pd> chain;
        bool success = false;

        if (check_pushdown(pushdowns_tree[0], line, chain, success))
            cout << "String is right";
        else 
            cout << "String is wrong";
    }

    bool check_pushdown(string pushdown, source_line& line, vector<src_and_pd> &chain, bool &success) {
        //счетчик количества изменений исходной строки line
        int has_change = 0;

        for (int i = pushdown.size() - 1; i >= 0; i--) {
            //Если символ в магазине нетерминальный, 
            if (is_nonTerm(pushdown[i])) {
                /*//то разложим данный магазин на несколько других, у которых начало будет совпадать
                //символами магазина pushdown до pushdown[i]*/
                vector<string> pushdowns = substitution_nonTerm(pushdown, i);
                bool exit = false;

                //Каждый из получившихся магазинов - новые ветви, которые мы перебором проверяем
                for (string str : pushdowns) {
                    /*//для правил, которые могут зацикливаться, нужно учитывать, что количество
                    //символом в правиле должно быть <= количества символов в заданной строке */
                    if (str.size() > line.size()) {
                        exit = false;
                    } else {
                        //Проверяем вновь получившийся магазин
                        exit = check_pushdown(str, line, chain, success);
                    }
                    /*//поскольку эта функция возвращает bool, то в случае, если в каком то из 
                    //ответвлений вернётся true, перебор в данном цикле сразу завершается*/
                    if (exit)
                        break;
                }
                /*//Если выше получим true, цикл сразу завершается, чтобы в следующих 
                //итерациях не получить неверный false
                //После того, как мы обошли все дочерние магазины, (то есть после того, как
                //мы вышли из цикла обхода по pushdowns) надо проверить были ли совпадения
                //символов какого-нибудь из магазинов с исходной строкой при том, что 
                //все вернули false. Если все вернули false, а изменения были (то есть
                //были найдены совпаденя символов какого-нибудь из магазинов с исходной 
                //строкой), то нам надо вернуть исходную строку к тому состоянию, которое
                //она имела до обхода всех дочерних pushdowns.*/
                if (!exit) {
                    if (has_change)
                        line.reset(has_change);
                    return false;
                }
                if (success)
                    break;
            } else {
                if (pushdown[i] != line.get()) {
                    if (has_change)
                        line.reset(has_change);
                    return false;
                } else {
                    //Нашли совпадение. Стираем совпадающие символы из исходной строки
                    line.pop();
                    //и из магазина
                    pushdown.erase(pushdown.begin() + i);
                    /*//фиксируем изменения на случай, если дальше эта функция и её
                    //"братские"(вершины дерева, имеющие общего родителя) узлы
                    //дерева вернут false*/
                    has_change++;

                    /*//признак конца поиска - нулевые исходные строки и магазин, но чтобы выйти из
                    //, возможно, большого стэка рекурсивно вызванных функций, сделаем 
                    //переменную-сигнал success, по которой будем выбираться из рекурсии*/
                    if (line.is_empty() && pushdown.empty()) {
                        success = true;
                        break;
                    }

                    if (line.is_empty() && !pushdown.empty()) {
                        if (has_change)
                            line.reset(has_change);
                        return false;
                    }
                }

            }
        }

        chain.push_back(src_and_pd(line.get_remains(), pushdown));
        return true;
    }

};



int main() {

    pushdown_automaton pa;
    string filename = "test2.txt";
    source_line sourceline("1111111111a");

    pa.get_grammars(filename);
    pa.check_line(sourceline);

    return 0;
}