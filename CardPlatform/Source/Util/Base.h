#pragma once
#include <string>
using namespace std;

namespace Base
{
    string GenTransDate();
    string GenTransTime();
    bool CompareDate(string grateDate, string lessDate);
    string GetDataHexLen(string data);
    string Increase(string currentLen, int step);
}