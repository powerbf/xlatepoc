#pragma once

struct monster_info;

// various elemental colour schemes... used for abstracting random
// short lists. When adding colours, please also add their names in
// str_to_colour!
enum element_type
{
    ETC_FIRE = 32,      // fiery colours (must be first and > highest colour)
    ETC_FIRST = ETC_FIRE,
    ETC_ICE,            // icy colours
    ETC_EARTH,          // earthy colours
    ETC_ELECTRICITY,    // electrical side of air
    ETC_AIR,            // non-electric and general air magic
    ETC_POISON,         // used only for venom mage and stalker stuff
    ETC_WATER,          // used only for the elemental
    ETC_MAGIC,          // general magical effect
    ETC_MUTAGENIC,      // transmute, poly, radiation effects
    ETC_WARP,           // teleportation and anything similar
    ETC_ENCHANT,        // magical enhancements
    ETC_HEAL,           // holy healing (not necromantic stuff)
    ETC_HOLY,           // general "good" god effects
    ETC_DARK,           // darkness
    ETC_DEATH,          // assassin/necromancy stuff
    ETC_UNHOLY,         // demonology stuff
    ETC_VEHUMET,        // vehumet's oddball colours
    ETC_BEOGH,          // Beogh altar colours
    ETC_CRYSTAL,        // colours of crystal
    ETC_BLOOD,          // colours of blood
    ETC_SMOKE,          // colours of smoke
    ETC_SLIME,          // colours of slime
    ETC_JEWEL,          // colourful
    ETC_ELVEN,          // used for colouring elf fabric items
    ETC_DWARVEN,        // used for colouring dwarf fabric items
    ETC_ORCISH,         // used for colouring orc fabric items
    ETC_FLASH,          // flashy colours
    ETC_FLOOR,          // colour of the area's floor
    ETC_ROCK,           // colour of the area's rock
    ETC_MIST,           // colour of mist
    ETC_SHIMMER_BLUE,   // shimmering colours of blue
    ETC_DECAY,          // colour of decay/swamp
    ETC_SILVER,         // colour of silver
    ETC_GOLD,           // colour of gold
    ETC_IRON,           // colour of iron
    ETC_BONE,           // colour of bone
    ETC_ELVEN_BRICK,    // colour of the walls in the Elven Halls
    ETC_WAVES,          // cyan, with regularly occurring lightcyan waves
    ETC_TREE,           // colour of trees on land
    ETC_RANDOM,         // any colour (except BLACK)
    ETC_TORNADO,        // twisting swirls of grey
    ETC_LIQUEFIED,      // ripples of yellow and brown.
#if TAG_MAJOR_VERSION == 34
    ETC_MANGROVE,       // colour of trees on water
#endif
    ETC_ORB_GLOW,       // halo coming from the Orb of Zot
    ETC_DISJUNCTION,    // halo from Disjunction
    ETC_DITHMENOS,      // Dithmenos altar colours
    ETC_ELEMENTAL,      // Cycling elemental colours
    ETC_INCARNADINE,    // Draining clouds coloured like raw flesh
#if TAG_MAJOR_VERSION == 34
    ETC_SHINING,        // shining gold (Gozag)
    ETC_PAKELLAS,       // Pakellas altar colours
#endif
    ETC_WU_JIAN,        // Wu Jian Chinese-inspired colours
    ETC_AWOKEN_FOREST,  // Angry trees.
    ETC_DISCO = 96,
    ETC_FIRST_LUA = ETC_DISCO, // colour indices have to be <128

    NUM_COLOURS
};

