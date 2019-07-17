/*
 * monster.cc
 * Partial mock of Crawl monster class
 */

#include "AppHdr.h"
#include "monsters-inc.h"
#include "mon-info.h"
#include "libutil.h"
#include "stringutil.h"

// Default constructor
monster::monster()
    : base_monster(MONS_NO_MONSTER)
{
    type = MONS_NO_MONSTER;
}

// Destructor
monster::~monster()
{
}

/**
 * Does this monster have a proper name?
 *
 * @return  Whether the monster has a proper name, e.g. "Rupert" or
 *          "Bogric the orc warlord". Should not include 'renamed' vault
 *          monsters, e.g. "decayed bog mummy" or "bag of meat".
 */
bool monster::is_named() const
{
    return mons_is_unique(type)
           || (!mname.empty() && !testbits(flags, MF_NAME_ADJECTIVE |
                                                  MF_NAME_REPLACE));
}

bool monster::has_base_name() const
{
    // Any non-ghost, non-Pandemonium demon that has an explicitly set
    // name has a base name.
    return !mname.empty() && !ghost;
}

static string _invalid_monster_str(monster_type type)
{
    string str = "INVALID MONSTER ";

    switch (type)
    {
    case NUM_MONSTERS:
        return str + "NUM_MONSTERS";
    case MONS_NO_MONSTER:
        return str + "MONS_NO_MONSTER";
    case MONS_PLAYER:
        return str + "MONS_PLAYER";
    case RANDOM_DRACONIAN:
        return str + "RANDOM_DRACONIAN";
    case RANDOM_BASE_DRACONIAN:
        return str + "RANDOM_BASE_DRACONIAN";
    case RANDOM_NONBASE_DRACONIAN:
        return str + "RANDOM_NONBASE_DRACONIAN";
    case WANDERING_MONSTER:
        return str + "WANDERING_MONSTER";
    default:
        break;
    }

    str += make_stringf("#%d", (int) type);

    if (type < 0)
        return str;

    if (type > NUM_MONSTERS)
    {
        str += make_stringf(" (NUM_MONSTERS + %d)",
                            int (NUM_MONSTERS - type));
        return str;
    }

    int          i;
    monster_type new_type;
    for (i = 0; true; i++)
    {
        new_type = (monster_type) (((int) type) - i);

        if (invalid_monster_type(new_type))
            continue;
        break;
    }
    str += make_stringf(" (%s + %d)",
                        mons_type_name(new_type, DESC_PLAIN).c_str(),
                        i);

    return str;
}

static string _mon_special_name(const monster& mon, description_level_type desc,
                                bool force_seen)
{
    if (desc == DESC_NONE)
        return "";

    const bool arena_submerged = false;

    if (mon.type == MONS_NO_MONSTER)
        return "DEAD MONSTER";
    //else if (mon.mid == MID_YOU_FAULTLESS)
    //    return "INVALID YOU_FAULTLESS";
    else if (invalid_monster_type(mon.type) && mon.type != MONS_PROGRAM_BUG)
        return _invalid_monster_str(mon.type);

    // Handle non-visible case first.
    if (!force_seen && !mon.observable() && !arena_submerged)
    {
        switch (desc)
        {
        case DESC_THE: case DESC_A: case DESC_PLAIN: case DESC_YOUR:
            return "something";
        case DESC_ITS:
            return "something's";
        default:
            return "it (buggy)";
        }
    }

    if (desc == DESC_DBNAME)
    {
        monster_info mi(&mon, MILEV_NAME);
        return mi.db_name();
    }

    return "";
}

string monster::name(description_level_type desc, bool force_vis,
                     bool force_article) const
{
    string s = _mon_special_name(*this, desc, force_vis);
    if (!s.empty() || desc == DESC_NONE)
        return s;

    monster_info mi(this, MILEV_NAME);
    // i.e. to produce "the Maras" instead of just "Maras"
    if (force_article)
        mi.mb.set(MB_NAME_UNQUALIFIED, false);
    return mi.proper_name(desc)
#ifdef DEBUG_MONINDEX
    // This is incredibly spammy, too bad for regular debug builds, but
    // I keep re-adding this over and over during debugging.
           + (Options.quiet_debug_messages[DIAG_MONINDEX]
              ? string()
              : make_stringf("«%d:%d»", mindex(), mid))
#endif
    ;
}

