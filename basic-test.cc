#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "localize.h"
#include "test-util.h"

using namespace std;

#define BUFSIZE 8000

string getlocale()
{
    const char *locale = setlocale(LC_ALL, NULL);
    return (locale == NULL ? "" : locale);
}

int main(int argc, char *argv[])
{
    const double PI = 3.141592653585;
    const long double PI_LONG = 3.141592653589793238462643383279L;

    string result;

    init_localization("en_AU");

    // check locale
    string locale = getlocale();
    cout << "====================\n";
    cout << "Locale is " << locale << endl;
    cout << "Language is " << get_localization_language() << endl;
    cout << "====================\n\n";

    result = localize("Hello, world!");
    check_result("basic test", "Greetings, globe!", result);

    result = localize("");
    check_result("empty string", "", result);

    result = localize("%s", "");
    check_result("empty string 2", "", result);

    result = localize("%llu, %lu, %u, %hu", 6LL, 5L, 4, 3);
    check_result("unsigned ints", "6, 5, 4, 3", result);

    result = localize("%lld, %ld, %d, %hd", -6LL, -5L, -4, -3);
    check_result("signed ints", "-6, -5, -4, -3", result);

    result = localize("%.10f, %.5e", PI , PI);
    check_result("doubles", "3.1415926536, 3.14159e+00", result);

    result = localize("%.15Lf, %.5Le", PI_LONG , PI_LONG);
    check_result("long doubles", "3.141592653589793, 3.14159e+00", result);

    result = localize("%c", 'A');
    check_result("char", "A", result);

    result = localize("%d%% \\{per annum\\}", 1);
    check_result("escape sequences", "1% {per annum}", result);

    result = localize(LocalizationArg("a flip flop", "%d flip flops", 1));
    check_result("singular", "a thong", result);

    result = localize(LocalizationArg("a flip flop", "%d flip flops", 2));
    check_result("dual", "a pair of thongs", result);

    result = localize(LocalizationArg("a flip flop", "%d flip flops", 3));
    check_result("plural", "3 thongs", result);

    result = localize(LocalizationArg("a flip flop"));
    check_result("singular xlate", "a thong", result);

    // this will fail - you must use nxlate
    //result = xlate("%d flip flops");
    //check_result("plural xlate", "%d thongs", result);

    // test arg order change
    vector<LocalizationArg> args;
    args.push_back(LocalizationArg("%s hits %s."));
    args.push_back(LocalizationArg("the arrow"));
    args.push_back(LocalizationArg("the orc"));
    result = localize_sentence(args);
    check_result("arg order", "The orc is hit by the arrow.", result);
    return 0;
}
