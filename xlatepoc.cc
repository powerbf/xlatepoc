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
    return 0;
}
