/**
 * @file  translate.h
 * @brief High-level translation routines (as opposed to xlate.h which is low-level)
**/

#pragma once

#include <string>

// build and translate a sentence of the form <subject> <verb> (e.g. "you die")
// it's assumed that verb is already in the correct form for the subject (e.g. die or dies)
std::string translate_sentence(const std::string& subject, const std::string& verb);

// build and translate sentence of the form <subject> <verb> <object> (e.g. "you hit the orc", "the orc hits you")
// it's assumed that verb is already in the correct form for the subject (e.g. hit or hits)
std::string translate_sentence(const std::string& subject, const std::string& verb, const std::string& object);

// build and translate a sentence of the form <subject> <verb> <object> with <instrument>
// (e.g. "you hit the orc with your long sword", "the orc hits you with a paralysis spell")
// it's assumed that verb is already in the correct form for the subject (e.g. hit or hits)
std::string translate_sentence(const std::string& subject, const std::string& verb, const std::string& object, const std::string& instrument);

