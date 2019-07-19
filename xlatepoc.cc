#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xlate.h"
#include "localize.h"

using namespace std;

string getlocale()
{
    const char *locale = setlocale(LC_ALL, NULL);
    return (locale == NULL ? "" : locale);
}

int main(int argc, char *argv[])
{

    string language = (argc >= 2 ? argv[1] : "");

    init_xlate(language);

    // check locale
    string locale = getlocale();
    cout << "====================\n";
    cout << "Usage: xlatepoc [language]\n  where language = en, de\n\n";
    cout << "Locale is " << locale << endl;
    cout << "Language is " << get_xlate_language() << endl;
    cout << "====================\n\n";

    cout << localize("Hello, world\n");
    cout << localize("") << endl;
    cout << localize("Empty string: %s", "") << endl;
    cout << localize("Signed short, int, long, long long: %hd, %d, %ld, %lld", (short)-3, -4, -5L, -6LL) << endl;
    cout << localize("Unsigned short, int, long, long long: %hu, %u, %lu, %llu", (unsigned short)3, 4U, 5UL, 6ULL) << endl;
    cout << localize("Doubles: %.20f, %e", 3.141592653585 , 3.141592653589) << endl;
    cout << localize("Long doubles: %.20Lf, %Le", 3.141592653585L , 3.141592653589L) << endl;
    cout << localize("Char: %c", 'A') << endl;
    cout << localize("Escapes: %d%% \\{per annum\\}", 1) << endl;
    return 0;
}
