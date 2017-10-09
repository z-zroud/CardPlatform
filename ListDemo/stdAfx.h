#pragma once

#include<Windows.h>
#include "..\DuiLib_R\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG 
#   pragma comment(lib, "../Lib/DuiLib_R_d.lib")
#else
#   pragma comment(lib, "../Lib/DuiLib_R.lib")
#endif
