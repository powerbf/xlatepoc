#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "localize.h"

#include "monsters-inc.h"
#include "english.h"
#include "stringutil.h"

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

    init_localization(language);

    // check locale
    string locale = getlocale();
    cout << "====================\n";
    cout << "Usage: monster-test [language]\n  where language = en, de\n\n";
    cout << "Locale is " << locale << endl;
    cout << "Language is " << get_localization_language() << endl;
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

        string english_name(mon_def->name);

        cout << endl << "Monster: " << english_name << endl;

        vector<string> variants;
        vector<LocalizationArg> args;

        if (mons_is_unique(i))
        {
            variants.push_back(english_name);
        }
        else {
            variants.push_back(string("the ") + english_name);
            variants.push_back(article_a(english_name));
        }

        for (vector<string>::iterator it = variants.begin(); it != variants.end(); ++it)
        {
            string sentence;

            // nominative
            args.clear();
            args.push_back(LocalizationArg("%s hits you."));
            args.push_back(LocalizationArg("monsters", *it));
            sentence = localize_sentence(args);
            cout << sentence << endl;

            // accusative
            args.clear();
            args.push_back(LocalizationArg("You miss %s."));
            args.push_back(LocalizationArg("monsters", *it));
            sentence = localize_sentence(args);
            cout << sentence << endl;

            // dative
            args.clear();
            args.push_back(LocalizationArg("You command %s to wait here."));
            args.push_back(LocalizationArg("monsters", *it));
            sentence = localize_sentence(args);
            cout << sentence << endl;
        }

        if (mons_is_unique(i))
        {
            continue;
        }

        string singular_en = article_a(english_name);
        string plural_en = string("%d ") + pluralise(english_name);

        args.clear();
        args.push_back(LocalizationArg("%s come into view."));
        args.push_back(LocalizationArg("monsters", singular_en, plural_en, 2));
        string sentence = localize_sentence(args);
        cout << sentence << endl;

        args.clear();
        args.push_back(LocalizationArg("You see %s."));
        args.push_back(LocalizationArg("monsters", singular_en, plural_en, 1));
        sentence = localize_sentence(args);
        cout << sentence << endl;

        args.clear();
        args.push_back(LocalizationArg("You see %s."));
        args.push_back(LocalizationArg("monsters", singular_en, plural_en, 2));
        sentence = localize_sentence(args);
        cout << sentence << endl;
}

    return 0;
}
