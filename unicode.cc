/**
 * @file
 * @brief Conversions between Unicode and local charsets, string
 *        manipulation functions that act on character types.
 *        (Subset of unicode.cc from Dungeon Crawl Stone Soup)
**/

//#include "AppHdr.h"

#include "unicode.h"

#include <climits>
//#include <clocale>
//#include <cstdio>
#include <cstring>
#include <string>

//#include "syscalls.h"

#define ASSERT(x)

// there must be at least 4 bytes free, NOT CHECKED!
int wctoutf8(char *d, char32_t s)
{
    if (s < 0x80)
    {
        d[0] = s;
        return 1;
    }
    if (s < 0x800)
    {
        d[0] = ( s >>  6)         | 0xc0;
        d[1] = ( s        & 0x3f) | 0x80;
        return 2;
    }
    if (s < 0x10000)
    {
        d[0] = ( s >> 12)         | 0xe0;
        d[1] = ((s >>  6) & 0x3f) | 0x80;
        d[2] = ( s        & 0x3f) | 0x80;
        return 3;
    }
    if (s < 0x110000)
    {
        d[0] = ( s >> 18)         | 0xf0;
        d[1] = ((s >> 12) & 0x3f) | 0x80;
        d[2] = ((s >>  6) & 0x3f) | 0x80;
        d[3] = ( s        & 0x3f) | 0x80;
        return 4;
    }
    // Invalid char marker (U+FFFD). Make sure we handled it above.
    ASSERT(s != 0xFFFD);
    return wctoutf8(d, 0xFFFD);
}

int utf8towc(char32_t *d, const char *s)
{
    if (*s == 0)
    {
        *d = 0;
        return 0;
    }
    if (!(*s & 0x80))
    {
        *d = *s;
        return 1;
    }
    if ((*s & 0xc0) == 0x80)
    {   // bare tail, invalid
        *d = 0xFFFD;
        int bad = 0;
        do bad++; while ((s[bad] & 0xc0) == 0x80);
        return bad;
    }

    int cnt;
    char32_t c;
    if ((*s & 0xe0) == 0xc0)
        cnt=2, c = *s & 0x1f;
    else if ((*s & 0xf0) == 0xe0)
        cnt=3, c = *s & 0x0f;
    else if ((*s & 0xf8) == 0xf0)
        cnt=4, c =*s & 0x07;
    /* valid UTF-8, invalid Unicode
    else if ((*s & 0xfc) == 0xf8)
        cnt=5, c = *s & 0x03;
    else if ((*s & 0xfe) == 0xfc)
        cnt=6, c = *s & 0x01;
    */
    else
    {   // 0xfe or 0xff, invalid
        *d = 0xFFFD;
        return 1;
    }

    for (int i = 1;  i < cnt; i++)
    {
        if ((s[i] & 0xc0) != 0x80)
        {   // only tail characters are allowed here, invalid
            *d = 0xFFFD;
            return i;
        }
        c = (c << 6) | (s[i] & 0x3f);
    }

    if (c < 0xA0                        // illegal characters
        || (c >= 0xD800 && c <= 0xDFFF) // UTF-16 surrogates
        || (cnt == 3 && c < 0x800)      // overlong characters
        || (cnt == 4 && c < 0x10000)    // overlong characters
        || c > 0x10FFFF)                // outside Unicode
    {
        c = 0xFFFD;
    }
    *d = c;
    return cnt;
}

#ifdef TARGET_OS_WINDOWS
// don't pull in wstring templates on other systems
wstring utf8_to_16(const char *s)
{
    wstring d;
    char32_t c;

    while (int l = utf8towc(&c, s))
    {
        s += l;
        if (c >= 0x10000)
        {
            c -= 0x10000;
            d.push_back(0xD800 + (c >> 10));
            d.push_back(0xDC00 + (c & 0x3FF));
        }
        else
            d.push_back(c);
    }
    return d;
}
#endif

#ifndef TARGET_OS_WINDOWS
static
#endif
string utf16_to_8(const utf16_t *s)
{
    string d;
    char32_t c;

    while (*s)
    {
        if (*s >= 0xD800 && *s <= 0xDBFF)
            if (s[1] >= 0xDC00 && s[1] <= 0xDFFF)
            {
                c = (((char32_t)s[0]) << 10) + s[1] - 0x35fdc00;
                s++;
            }
            else
                c = 0xFFFD; // leading surrogate without its tail
        else if (*s >= 0xDC00 && *s <= 0xDFFF)
            c = 0xFFFD;     // unpaired trailing surrogate
        else
            c = *s;
        s++;

        char buf[4];
        int l = wctoutf8(buf, c);
        for (int i = 0; i < l; i++)
            d.push_back(buf[i]);
    }

    return d;
}

string utf8_to_mb(const char *s)
{
#ifdef __ANDROID__
    return s;
#else
    string d;
    char32_t c;
    int l;
    mbstate_t ps;

    memset(&ps, 0, sizeof(ps));
    while ((l = utf8towc(&c, s)))
    {
        s += l;

        char buf[MB_LEN_MAX];
        int r = wcrtomb(buf, c, &ps);
        if (r != -1)
        {
            for (int i = 0; i < r; i++)
                d.push_back(buf[i]);
        }
        else
            d.push_back('?'); // TODO: try to transliterate
    }
    return d;
#endif
}

static string utf8_validate(const char *s)
{
    string d;
    char32_t c;
    int l;

    while ((l = utf8towc(&c, s)))
    {
        s += l;

        char buf[4];
        int r = wctoutf8(buf, c);
        for (int i = 0; i < r; i++)
            d.push_back(buf[i]);
    }
    return d;
}

string mb_to_utf8(const char *s)
{
#ifdef __ANDROID__
    // Paranoia; all consumers already use the same code so this won't do
    // anything new.
    return utf8_validate(s);
#else
    string d;
    wchar_t c;
    int l;
    mbstate_t ps;

    memset(&ps, 0, sizeof(ps));
    // the input is zero-terminated, so third argument doesn't matter
    while ((l = mbrtowc(&c, s, MB_LEN_MAX, &ps)))
    {
        if (l > 0)
            s += l;
        else
        {   // invalid input, mark it and try to recover
            s++;
            c = 0xFFFD;
        }

        char buf[4];
        int r = wctoutf8(buf, c);
        for (int i = 0; i < r; i++)
            d.push_back(buf[i]);
    }
    return d;
#endif
}

int wclen(char32_t c)
{
    char dummy[4];
    return wctoutf8(dummy, c);
}

