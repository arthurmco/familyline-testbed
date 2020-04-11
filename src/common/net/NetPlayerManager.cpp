#include <common/net/NetPlayerManager.hpp>
#include <common/net/NetServer.hpp>


#ifdef _WIN32

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define usleep(x) Sleep(x/1000)

#endif

using namespace familyline::Net;
using namespace familyline::logic;

