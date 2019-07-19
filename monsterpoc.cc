#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xlate.h"
#include "localize.h"

#include "monsters-inc.h"

using namespace std;


string getlocale()
{
    const char *locale = setlocale(LC_ALL, NULL);
    return (locale == NULL ? "" : locale);
}

/*
static const monsterentry* get_monster_data(monster_type mc)
{
    // TODO: optimise
    size_t num_entries = sizeof(mondata) / sizeof(monsterentry);
    for (size_t i = 0; i < num_entries; i++)
    {
        const monsterentry* entry = &(mondata[i]);
        if (entry->mc == mc)
        {
            return entry;
        }
    }
    return NULL;
}*/

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

    for (monster_type i = MONS_ORC; i <= MONS_ORC; i++)
    {
        const char *msgid = NULL;
        const char *fmtstr = NULL;
        const char *monstr = NULL;
        printf("\n");

        const monsterentry* mon_def = get_monster_data(i);
        if (mon_def == NULL)
        {
            printf("ERROR: NUll monster data\n");
        }

        string the_monster = string("the ") + mon_def->name;
        string a_monster = string("an ") + mon_def->name;

        string sentence;

        sentence = localize("You see %s.", a_monster.c_str());
        cout << sentence << endl;

        sentence = localize_sentence("%s shouts.", the_monster.c_str());
        cout << sentence << endl;

        sentence = localize("You command %s to wait here.", the_monster.c_str());
        cout << sentence << endl;

        sentence = localize_sentence("%s hits you.", the_monster.c_str());
        cout << sentence << endl;

        sentence = localize_sentence("You miss %s.", the_monster.c_str());
        cout << sentence << endl;
    }

    return 0;
}
