#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xlate.h"
#include "translate.h"

using namespace std;

static const char* monsters[] = {"orc", "elf"};

string translate_action(const char* message, const char* article, const char* monster, const char* context)
{
	string the_monster = string(article) + " " + monster;
	const char* mon_msgid = the_monster.c_str();

	const char* xlated_msg = xlate(message);
	const char* xlated_mon = dcxlate("entities", context, mon_msgid);

	char buf[2048];
	sprintf(buf, xlated_msg, xlated_mon);
	return string(buf);
}

int main (int argc, char* argv[]) {

	char* language = (argc >= 2 ? argv[1] : NULL);

	init_xlate(language);

	// check locale
	const char* locale = setlocale(LC_ALL, NULL);
	printf("====================\n");
	printf("Usage: xlatepoc [language]\n  where language = en, de\n\n");
	printf("Locale is %s\n", locale);
	printf("Language is %s\n", get_xlate_language());
	printf("====================\n\n");

	cout << xlate("Hello, world\n");
	//printf(xlate("Hello, world\n"));

	for (int i = 0; i <= 1; i++) {
		const char* msgid = NULL;
		const char* fmtstr = NULL;
		const char* monstr = NULL;
		printf("\n");

		string sentence;
		//sentence = translate_action("%s attacks you.", "The", monsters[i], "doing");
		sentence = translate_sentence(string("The ")+monsters[i], "attacks", "you");
		cout << sentence << endl;
		//sentence = translate_action("You attack %s.", "the", monsters[i], "attacked");
		sentence = translate_sentence("You", "attack", string("the ")+monsters[i]);
		cout << sentence << endl;
		sentence = translate_action("You command %s to wait here.", "the", monsters[i], "commanded");
		cout << sentence << endl;
	}
	
	printf("\n");
	printf(xlate("You kill %s!"), dcxlate("entities", "killed", "Natasha"));
	printf("\n");
    return 0;
}
