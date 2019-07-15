/**
 * @file
 * @brief Auxiliary functions to make savefile versioning simpler.
 * DUMMY
**/

#pragma once

#include <cstdio>

enum tag_type   // used during save/load process to identify data blocks
{
    TAG_NO_TAG = 0,                     // should NEVER be read in!
    TAG_CHR = 1,                        // basic char info
    TAG_YOU,                            // the main part of the save
    TAG_LEVEL,                          // a single dungeon level
    TAG_GHOST,                          // ghost
    NUM_TAGS,

    // Returned when a known tag was deliberately not read. This value is
    // never saved and can safely be changed at any point.
    TAG_SKIP
};

/* ***********************************************************************
 * writer API
 * *********************************************************************** */

class writer
{
public:
    writer() {}

    ~writer() {}

    void writeByte(unsigned char byte);
    void write(const void *data, size_t size);
    long tell();

    bool succeeded() const { return true; }

};

void marshallByte    (writer &, int8_t);
void marshallShort   (writer &, int16_t);
void marshallInt     (writer &, int32_t);
void marshallFloat   (writer &, float);
void marshallUByte   (writer &, uint8_t);
void marshallBoolean (writer &, bool);
void marshallString  (writer &, const string &);
void marshallString4 (writer &, const string &);
#ifndef XLATE_POC
void marshallCoord   (writer &, const coord_def &);
void marshallItem    (writer &, const item_def &, bool info = false);
void marshallMonster (writer &, const monster&);
void marshall_level_id(writer& th, const level_id& id);
#endif
void marshallUnsigned(writer& th, uint64_t v);
void marshallSigned(writer& th, int64_t v);

/* ***********************************************************************
 * reader API
 * *********************************************************************** */

class reader
{
public:
    reader(const string &filename, int minorVersion = TAG_MINOR_INVALID);

    unsigned char readByte();
    void read(void *data, size_t size);
    void advance(size_t size);
    int getMinorVersion() const;
    void setMinorVersion(int minorVersion);
    bool valid() const;
    void fail_if_not_eof(const string &name);
    void close();

    string filename() const { return _filename; }

    void set_safe_read(bool setting) { _safe_read = setting; }

private:
    string _filename;
    int _minorVersion;
    bool _safe_read;
};

class short_read_exception : exception {};

int8_t      unmarshallByte    (reader &);
int16_t     unmarshallShort   (reader &);
int32_t     unmarshallInt     (reader &);
float       unmarshallFloat   (reader &);
uint8_t     unmarshallUByte   (reader &);
bool        unmarshallBoolean (reader &);
string      unmarshallString  (reader &);
void        unmarshallString4 (reader &, string&);
#ifndef XLATE_POC
coord_def   unmarshallCoord   (reader &);
void        unmarshallItem    (reader &, item_def &item);
void        unmarshallMonster (reader &, monster& item);
dungeon_feature_type unmarshallFeatureType(reader &);
level_id    unmarshall_level_id(reader& th);
#endif

uint64_t unmarshallUnsigned(reader& th);
template<typename T>
static inline void unmarshallUnsigned(reader& th, T& v)
{
    v = (T)unmarshallUnsigned(th);
}

int64_t unmarshallSigned(reader& th);
template<typename T>
static inline void unmarshallSigned(reader& th, T& v)
{
    v = (T)unmarshallSigned(th);
}

/* ***********************************************************************
 * Tag interface
 * *********************************************************************** */

void tag_read(reader &inf, tag_type tag_id);
void tag_write(tag_type tagID, writer &outf);
void tag_read_char(reader &th, uint8_t format, uint8_t major, uint8_t minor);

#ifndef XLATE_POC
vector<ghost_demon> tag_read_ghosts(reader &th);
void tag_write_ghosts(writer &th, const vector<ghost_demon> &ghosts);
#endif

/* ***********************************************************************
 * misc
 * *********************************************************************** */

string make_date_string(time_t in_date);
