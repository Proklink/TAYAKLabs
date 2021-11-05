#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
using namespace std;

class StateMachine {
	map<pair<string, char>, set<string>> state_machine_table;
	vector<string> unused_states;
	set<string> used_states;
	set<char> alphabet;
	string check_line;

public:

	string make_state(const set<string> &states)
	{
		string name = "";
		
		for (const string& st : states)
			name += st;

		for (const char& sym : alphabet)
			for (const string& st : states)
			{
				pair<string, char> tmp_pair = make_pair(st, sym);
				auto table_iter = state_machine_table.find(tmp_pair);

				if (table_iter != state_machine_table.end())
				{
					set<string> tmp_set = table_iter->second;
					state_machine_table[make_pair(name, sym)].insert(tmp_set.begin(), tmp_set.end());
				}
			}

		return name;
	}

	void print()
	{
		cout << "Table of state machine:\n";
		for (const auto &table_elem : state_machine_table)
			cout << table_elem.first.first << "," << table_elem.first.second << "=" << *table_elem.second.begin() << "\n";
	}

	void determination()
	{
		while (!unused_states.empty())
		{
			string Qcur = unused_states[0];
			size_t states_cnt = 0;

			for (const char& sym : alphabet)
			{
				pair<string, char> tmp_pair = make_pair(Qcur, sym);
				auto table_iter = state_machine_table.find(tmp_pair);

				if (table_iter == state_machine_table.end()) 
					continue;//для данного состояния не задано данного символа

				set<string> tmp_set = table_iter->second;
				string state = make_state(tmp_set);

				if (used_states.find(state) == used_states.end()) 
					unused_states.push_back(state);

				state_machine_table[tmp_pair].clear();
				state_machine_table[tmp_pair].insert(state);
				used_states.insert(Qcur);
			}
			unused_states.erase(unused_states.begin());
		}

	}

	void get_automate(const string& file_name)
	{
		ifstream file(file_name);
		string source_state, final_state, str;
		size_t line_count = 0;
		bool is_determ = true;
		char sym;

		while (getline(file, str)) 
		{
			size_t comma = str.find(',');
			size_t last_eq = str.find_last_of('=');
			
			source_state = str.substr(0, comma);
			if (line_count == 0)
				unused_states.push_back(source_state);
			
			sym = str.substr(comma + 1, 1)[0];
			alphabet.insert(sym); 

			final_state = str.substr(last_eq + 1);

			state_machine_table[make_pair(source_state, sym)].insert(final_state); 
		}
		file.close();
	}

	bool is_determ() {
		for (const auto &nd_elem : state_machine_table)
			if (nd_elem.second.size() > 1)
				return false;

		return true;
	}

	int str_check()
	{
		string Qcur = "q0";

		for (size_t i=0; i < check_line.size(); i++)
		{
			pair<string, char> tmp_pair = make_pair(Qcur, check_line[i]);

			if (state_machine_table.find(tmp_pair) == state_machine_table.end()) 
				return -1;

			Qcur = *state_machine_table[tmp_pair].begin();

			if (i == (check_line.size() - 1)) {
				if (Qcur.find('f') != -1)
					return 0;
				else
					return -1;
			}
		}
		return -1;
	}

	void get_str()
	{
		ifstream file("string.txt");
		int ret;

		getline(file, check_line);
		file.close();
	}

};

int main()
{
	int ret;
	StateMachine st_m;
	st_m.get_automate("var2.txt");
	st_m.get_str();

	if (!st_m.is_determ()) {
		cout << "Source state machine is not derministic\n";
		st_m.determination();
	} else {
		cout << "Source state machine is derministic\n";
	}

	st_m.print();

	ret = st_m.str_check();
	if (!ret)
		cout << "Valid string\n";
	else
		cout << "Not valid string\n";

	return 0;
}
