/*
 * monster.h
 * Partial mock of Crawl monster class
 */

#pragma once

#include <memory>
using namespace std;

#include "actor.h"
#include "mon-flags.h"

class monster : public actor
{
public:
    monster();
    ~monster();

    bool is_player() const override { return false; }

    string name(description_level_type type, bool force_visible = false,
                bool force_article = false) const override;

    // Returns true if the monster is named with a proper name, or is
    // a player ghost.
    bool is_named() const;

    // Does this monster have a base name, i.e. is base_name() != name().
    // See base_name() for details.
    bool has_base_name() const;

    string mname;
    monster_type  base_monster;        // zombie base monster, draconian colour
    monster_flags_t flags;             // bitfield of boolean flags
    //unique_ptr<ghost_demon> ghost;     // Ghost information.
    unique_ptr<int> ghost;

    union
    {
        // These must all be the same size!
        unsigned int number;   ///< General purpose number variable
        int blob_size;         ///< num of slimes/masses in this one
        int num_heads;         ///< Hydra-like head number
        int ballisto_activity; ///< How active is this ballistomycete?
        int spore_cooldown;    ///< Can this make ballistos (if 0)
        int mangrove_pests;    ///< num of animals in shambling mangrove
        int prism_charge;      ///< Turns this prism has existed
        int battlecharge;      ///< Charges of battlesphere
        int move_spurt;        ///< Sixfirhy/jiangshi/kraken black magic
        mid_t tentacle_connect;///< mid of monster this tentacle is
                               //   connected to: for segments, this is the
                               //   tentacle; for tentacles, the head.
    };
    int           colour;

};
