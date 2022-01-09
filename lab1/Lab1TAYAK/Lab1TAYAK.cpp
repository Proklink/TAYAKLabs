#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "Calc.h"
#include "Tokenizer.h" 

using namespace std;

int main()
{
    Tokenizer tk;
    tk.get_source_string();
    bool ret = tk.split();

    cout << "Tokens:\n";
    for (string token : tk.tokens)
        cout << token << endl;
    
    if (!ret) {
        cout << "split " << ret << endl;
        return -1;
    }

    Calc calc(&tk.tokens);
    cout << "Result = " << calc.calculate() << endl;

    return 0;
}
