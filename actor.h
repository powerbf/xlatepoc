/*
 * actor.h
 * Partial mock of Crawl actor class
 */

#pragma once

#include <string>
using std::string;

#include "description-level-type.h"
#include "monster-type.h"

class actor
{
public:
    virtual ~actor() {};

    monster_type type;
    //mid_t        mid; // monster instance id

    virtual bool is_player() const = 0;
    bool is_monster() const { return !is_player(); }

    // Visibility as required by messaging. In usual play:
    //   Does the player know what's happening to the actor?
    bool observable() const { return visible; };

    virtual string name(description_level_type type,
                        bool force_visible = false,
                        bool force_article = false) const = 0;

    // XLATE_POC only
    bool visible;
    actor() : visible(true) {}

};
