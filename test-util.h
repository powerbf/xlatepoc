/*
 * test-util.h
 * Test utilities
 */

#pragma once

#include <string>
using std::string;

void check_result(const string& test_name, const string& expected, const string& actual);
