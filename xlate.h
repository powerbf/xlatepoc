/**
 * @file  xlate.h
 * @brief Low-level translation routines.
 **/

#pragma once

#ifdef NO_TRANSLATE

// compile without translation
#define init_xlate(lang)
#define get_xlate_language() ""
#define dcxlate(domain, context, msgid) msgid
#define dcnxlate(domain, context, msgid1, msgid2, n) (n==1 ? msgid1 : msgid2)

#else

#include <stddef.h>
#include <string>

// initialize
void init_xlate(const char *lang);

const char* get_xlate_language();

// translate with domain and context
//
// domain = translation file (optional, default="messages")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid = English text to be translated
const char* dcxlate(const char *domain, const char *context, const char *msgid);

// translate with domain, context and number
// returns the plural form corresponding to number
//
// domain = translation file
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid1 = English singular text
// msgid2 = English plural text
// n = the count of whatever it is
const char* dcnxlate(const char *domain, const char *context,
                     const char *msgid1, const char *msgid2, unsigned long n);

#endif

// translate with context (use default domain)
#define cxlate(context, msgid) dcxlate(NULL, context, msgid)

// translate with domain (no context)
#define dxlate(domain, msgid) dcxlate(domain, NULL, msgid)

// translate with default domain and no context
#define xlate(msgid) dcxlate(NULL, NULL, msgid)

// translate with context and number (use default domain)
#define cnxlate(context, msgid1, msgid2, n) dcnxlate(NULL, context, msgid1, msgid2, n)

// translate with domain and number (no context)
#define dnxlate(context, msgid1, msgid2, n) dcnxlate(domain, NULL, msgid1, msgid2, n)

// translate with number (use default domain and no context)
#define nxlate(msgid1, msgid2, n) dcnxlate(NULL, NULL, msgid1, msgid2, n)
