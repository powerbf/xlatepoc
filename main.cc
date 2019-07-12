#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xlate.h"
#include "translate.h"

using namespace std;

static const char *monsters[] =
{ "orc", "elf" };

string translate_action(const char *message, const char *article,
        const char *monster, const char *context)
{
    string the_monster = string(article) + " " + monster;
    const char *mon_msgid = the_monster.c_str();

    string xlated_msg = xlate(message);
    string xlated_mon = dcxlate("entities", context, mon_msgid);

    char buf[2048];
    sprintf(buf, xlated_msg.c_str(), xlated_mon.c_str());
    return string(buf);
}

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

    cout << xlate("Hello, world\n");
    //printf(xlate("Hello, world\n"));

    for (int i = 0; i <= 1; i++)
    {
        const char *msgid = NULL;
        const char *fmtstr = NULL;
        const char *monstr = NULL;
        printf("\n");

        string the_monster = string("the ") + monsters[i];
        string a_monster = string("an ") + monsters[i];

        string sentence;

        sentence = translate_sentence("you", "see", a_monster);
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "shouts");
        cout << sentence << endl;

        sentence = translate_action("You command %s to wait here.", "the", monsters[i], "commanded");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "attacks", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "misses", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "closely misses", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "barely misses", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "hits", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "slashes", "you");
        cout << sentence << endl;

        sentence = translate_sentence(the_monster, "kills", "you");
        cout << sentence << endl;

        sentence = translate_sentence("You", "attack", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "miss", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "closely miss", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "barely miss", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "hit", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "slash", the_monster);
        cout << sentence << endl;

        sentence = translate_sentence("You", "kill", the_monster);
        cout << sentence << endl;
    }

    printf("\n");
    printf(xlate("You kill %s!").c_str(), dcxlate("entities", "killed", "Natasha").c_str());
    printf("\n");
    return 0;
}
