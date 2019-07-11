/**
 * @file  translate.cc
 * @brief
**/

#include <sstream>
#include <string>
#include <cstring>
#include <strings.h>

#include "stringutil.h"
#include "translate.h"
#include "xlate.h"

using namespace std;

// compose the sentence after translation
static std::string compose_string(const char* fmt_string, const char* subject, const char* object, const char* instrument)
{
	// calculate required buffer length
	// NOTE: this works just as well for UTF-8 as for ASCII because strlen will see it as ASCII and count bytes
	int bufflen = strlen(fmt_string) + 1;
	bufflen += (subject == NULL ? 0 : strlen(subject));
	bufflen += (object == NULL ? 0 : strlen(object));
	bufflen += (instrument == NULL ? 0 : strlen(instrument));
	// However, the buffer could overflow if someone did something stupid/malicious like putting numeric
	// format specifiers (%d, %g, etc.) in the translated string, so add a fudge factor to be on the safe side
	bufflen += 200;

	char* buff = new char[bufflen];

	if (subject != NULL && object != NULL && instrument != NULL)
	{
		sprintf(buff, fmt_string, subject, object, instrument);
	}
	else if (subject != NULL && object != NULL)
	{
		sprintf(buff, fmt_string, subject, object);
	}
	else if (subject != NULL && instrument != NULL)
	{
		sprintf(buff, fmt_string, subject, instrument);
	}
	else if (object != NULL && instrument != NULL)
	{
		sprintf(buff, fmt_string, object, instrument);
	}
	else if (subject != NULL)
	{
		sprintf(buff, fmt_string, subject);
	}
	else if (object != NULL)
	{
		sprintf(buff, fmt_string, object);
	}
	else if (instrument != NULL)
	{
		sprintf(buff, fmt_string, instrument);
	}
	else
	{
		strcpy(buff, fmt_string);
	}

	string result(buff);
	delete buff;

	result = uppercase_first(result);
	return result;
}

// derive object context from the (English) verb
// e.g. if the verb is "drop" then the context for the object is "dropped"
static std::string derive_object_context(const std::string& verb)
{
	if (ends_with(verb, "ed"))
	{
		return verb;
	}

	// handle some special cases first
	if (starts_with(verb, "drop"))
	{
		return string("dropped");
	}
	else if (verb == "pick up" || verb == "picks up" || verb == "picked up")
	{
		return string("picked up");
	}
	else if (starts_with(verb, "hit"))
	{
		return string("hit");
	}
	else if (starts_with(verb, "drink"))
	{
		return string("drunk");
	}
	else if (starts_with(verb, "read"))
	{
		return string("read");
	}

	// handle the standard case
	std::string context;

	// remove third person suffix from verb
	if (ends_with(verb, "es"))
	{
		context = verb.substr(0, verb.length()-2);
	}
	else if (ends_with(verb, "s") && !ends_with(verb, "ss"))
	{
		context = verb.substr(0, verb.length()-1);
	}
	else
	{
		context = verb;
	}

	// add "ed" if not already present
	context += "ed";

	return context;
}

static string translate_entity(const string& entity, const string& context)
{
	string e = entity;
	if (starts_with(e, "The ") || starts_with(e, "A ") || starts_with(e, "An ") || starts_with(e, "Your "))
	{
		// convert first letter to lowercase so that translator only has to provide one translation
		e = lowercase_first(e);
	}

	const char* result = dcxlate("entities", context.c_str(), e.c_str());
	return string(result);
}

// build and translate a sentence of the form "<subject> <verb> <object> with <instrument>"
// (e.g. "you hit the orc with your long sword", "the orc hits you with a paralysis spell")
// it's assumed that verb is already in the correct form for the subject (e.g. hit or hits)
std::string translate_sentence(const string& subject, const string& verb, const string& object, const string& instrument)
{
	stringstream fmt_ss; // form building format string

	bool has_object = !object.empty();
	bool has_instrument = !instrument.empty();

	bool subject_is_you = (lowercase_string(subject) == "you");
	bool object_is_you = (lowercase_string(object) == "you");;

	const char* xlated_fmt_string = NULL;
	const char* xlated_subject = NULL;
	const char* xlated_object = NULL;
	const char* xlated_instrument = NULL;

	if (subject_is_you)
	{
		if (has_object && has_instrument)
		{
			fmt_ss << "You " << verb << " %s with %s.";
		}
		else if (has_object)
		{
			fmt_ss << "You " << verb << " %s.";
		}
		else if (has_instrument)
		{
			fmt_ss << "You " << verb << " with %s.";
		}
		else
		{
			fmt_ss << "You " << verb << ".";
		}
	}
	else if (object_is_you)
	{
		if (has_instrument)
		{
			fmt_ss << "%s " << verb << " you with %s.";
		}
		else
		{
			fmt_ss << "%s " << verb << " you.";
		}
	}
	else
	{
		// neither subject nor object is you
		if (has_object && has_instrument)
		{
			fmt_ss << "%s " << verb << " %s with %s.";
		}
		else if (has_object)
		{
			fmt_ss << "%s " << verb << " %s.";
		}
		else if (has_instrument)
		{
			fmt_ss << "%s " << verb << " with %s.";
		}
		else
		{
			fmt_ss << "%s " << verb << ".";
		}
	}

	if (!subject_is_you)
	{
		xlated_subject = translate_entity(subject, "doing").c_str();
	}

	if (has_object && !object_is_you)
	{
		std::string context = derive_object_context(verb);
		xlated_object = translate_entity(object, context).c_str();
	}

	if (has_instrument)
	{
		xlated_instrument = translate_entity(instrument, "with").c_str();
	}


	xlated_fmt_string = xlate(fmt_ss.str().c_str());
	string result = compose_string(xlated_fmt_string, xlated_subject, xlated_object, xlated_instrument);
	return result;
}

// build and translate a sentence of the form <subject> <verb> <object> (e.g. "you hit the orc", "the orc hits you")
// it's assumed that verb is already in the correct form for the subject (e.g. hit or hits)
std::string translate_sentence(const string& subject, const string& verb, const string& object)
{
	return translate_sentence(subject, verb, object, "");
}


// build and translate a sentence of the form <subject> <verb> (e.g. "you die")
// it's assumed that verb is already in the correct form for the subject (e.g. die or dies)
std::string translate_sentence(const string& subject, const string& verb)
{
	return translate_sentence(subject, verb, "", "");
}

