#pragma once

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif

#include "EngineInfo.h"
#include "ClientInfo.h"
#include "Utils/MathUtils.h"
#include "Utils/StringUtils.h"

using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
