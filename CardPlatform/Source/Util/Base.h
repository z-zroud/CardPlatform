#pragma once
#include <string>
using namespace std;

namespace Base
{
    string GenTransDate();
    string GenTransTime();
    bool CompareDate(string grateDate, string lessDate);
}