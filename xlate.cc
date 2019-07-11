/**
 * @file  xlate.cc
 * @brief Low-level translation routines.
 * This implementation uses gnu gettext, but references to gettext are deliberately kept
 * within this file to allow easy change to a different implementation.
 **/

#ifndef NO_TRANSLATE

#include <cstring>
#include <clocale>
#include <libintl.h>
#include "xlate.h"
using namespace std;

static const char GETTEXT_CTXT_GLUE = '\004';

static string language;

// initialize
void init_xlate(const char *lang)
{
    // must do this to apply user's locale because C++ sets locale to "C" by default, which won't handle unicode
    // this also probably won't work if the user's locale is not unicode (TODO: test that)
    setlocale(LC_ALL, "");

    language = (lang == NULL ? "" : lang);
    setenv("LANGUAGE", language.c_str(), 1);

    bindtextdomain("context-map", "./locale");
    bindtextdomain("messages", "./locale");
    bindtextdomain("entities", "./locale");

    // set default domain
    textdomain("messages");
}

const char* get_xlate_language()
{
    return language.c_str();
}

// skip translation if language is English (or unspecified which implies English)
static inline bool skip_translation()
{
    return (language.empty() || language == "en");
}

// map usage context to translation context
// (for example, in German, "attacked" would map to "accusative", but "commanded" would map to "dative")
static const char* map_context(const char *ctx_in)
{
    const char *ctx_out = dgettext("context-map", ctx_in);
    if (ctx_out == NULL || strlen(ctx_out) == 0)
    {
        ctx_out = ctx_in;
    }
    return ctx_out;
}

// translate with domain and context
//
// domain = translation file (optional, default="messages")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid = English text to be translated
//
// NOTE: this is different to dpgettext in two ways:
//  1) dpgettext can only handles string literals (not variables). This can handle either.
//  2) if context is NULL or empty then this falls back to contextless gettext
const char* dcxlate(const char *domain, const char *context, const char *msgid)
{
    if (skip_translation())
    {
        return msgid;
    }

    const char *translation = NULL;
    const char *mapped_context = map_context(context);

    if (mapped_context != NULL && strlen(mapped_context) != 0)
    {
        // check for translation in specific context
        string ctx_msgid = string(mapped_context) + GETTEXT_CTXT_GLUE + msgid;
        translation = dgettext(domain, ctx_msgid.c_str());
        if (translation != NULL && strcmp(translation, ctx_msgid.c_str()) == 0)
        {
            translation = NULL;
        }
    }

    if (translation == NULL)
    {
        // check for translation in global context
        translation = dgettext(domain, msgid);
        if (translation == NULL)
        {
            translation = msgid;
        }
    }

    return translation;
}

// translate with domain, context and number
// select the plural form corresponding to number
//
// domain = translation file (optional, default="messages")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid1 = English singular text
// msgid2 = English plural text
// n = the count of whatever it is
//
// NOTE: this is different to dpngettext in two ways:
//  1) dpngettext can only handles string literals (not variables). This can handle either.
//  2) if context is NULL or empty then this falls back to contextless ngettext
const char* dcnxlate(const char *domain, const char *context,
        const char *msgid1, const char *msgid2, unsigned long n)
{
    if (skip_translation())
    {
        return (n == 1 ? msgid1 : msgid2);
    }

    const char *translation = NULL;

    const char *mapped_context = map_context(context);

    if (mapped_context != NULL && strlen(mapped_context) != 0)
    {
        // check for translation in specific context
        string mctx = string(mapped_context);
        string ctx_msgid1 = mctx + GETTEXT_CTXT_GLUE + msgid1;
        string ctx_msgid2 = mctx + GETTEXT_CTXT_GLUE + msgid2;
        translation = dngettext(domain, ctx_msgid1.c_str(), ctx_msgid2.c_str(),
                n);
        if (translation != NULL)
        {
            if (strcmp(translation, ctx_msgid1.c_str()) == 0
                    || strcmp(translation, ctx_msgid2.c_str()) == 0)
            {
                translation = NULL;
            }
        }
    }

    if (translation == NULL)
    {
        // look in global domain
        translation = dngettext(domain, msgid1, msgid2, n);
    }

    if (translation == NULL)
    {
        // still no joy - fall back on English
        translation = (n == 1 ? msgid1 : msgid2);
    }

    return translation;
}

#endif
