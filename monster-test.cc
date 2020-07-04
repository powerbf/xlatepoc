#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "xlate.h"
#include "localize.h"

#include "monsters-inc.h"
#include "english.h"

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

    for (monster_type i = MONS_PROGRAM_BUG; i < NUM_MONSTERS; i++)
    {
        const char *msgid = NULL;
        const char *fmtstr = NULL;
        const char *monstr = NULL;

        const monsterentry* mon_def = get_monster_data(i);
        if (mon_def == NULL)
        {
            printf("ERROR: Null monster data\n");
        }
        else if (mon_def->genus == MONS_PROGRAM_BUG)
        {
            continue;
        }

        cout << endl << "Monster: " << mon_def->name << endl;

        vector<string> variants;

        if (mons_is_unique(i))
        {
            variants.push_back(mon_def->name);
        }
        else {
            variants.push_back(string("the ") + mon_def->name);
            variants.push_back(article_a(mon_def->name));
        }

        for (vector<string>::iterator it = variants.begin(); it != variants.end(); ++it)
        {
            string sentence;

            // nominative
            sentence = localize_sentence("%s hits you.", it->c_str());
            cout << sentence << endl;

            // accusative
            sentence = localize_sentence("You miss %s.", it->c_str());
            cout << sentence << endl;

            // dative
            sentence = localize("You command %s to wait here.", it->c_str());
            cout << sentence << endl;
        }

    }

    return 0;
}
