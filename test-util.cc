/*
 * test-util.cc
 *
 *  Created on: 20 Jul 2019
 *      Author: brian
 */

#include <iostream>
using namespace std;

void check_result(const string& test_name, const string& expected, const string& actual)
{
    cout << (actual == expected ? "PASS" : "FAIL");
    cout << ": " << test_name << ": ";
    cout << "expected=[" << expected << "], ";
    cout << "actual=[" << actual << "]" << endl;
}

