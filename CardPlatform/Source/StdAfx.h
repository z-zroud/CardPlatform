#pragma once

#include<Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <winscard.h>
#include "..\..\DuiLib_R\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG 
#   pragma comment(lib, "../Lib/DuiLib_R_d.lib")
#else
#   pragma comment(lib, "../Lib/DuiLib_R.lib")
#endif

#pragma comment(lib,"Des0.lib")
#pragma comment(lib,"WINSCARD.LIB")
#pragma comment(lib,"sqlite3.lib")



