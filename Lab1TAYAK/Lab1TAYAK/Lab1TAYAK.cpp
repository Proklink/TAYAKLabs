#include <iostream>
#include <string>
#include "String.h"

using namespace std;

string String::func = "pow";
map<string, uint8_t> String::priorities = { {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {func, 3} };

int main()
{
    String source;
        
    source.get_source_string();

    try {
        cout << "Result = " << source.calculate();
    }
    catch (string mes) {
        cout << mes << endl;
    }

    return 0;
}
