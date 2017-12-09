/* 
   Socket definitions for both client and server
   
   Copyright (C) 2017 Arthur M
*/

#ifndef TSOCKET_H
#define TSOCKET_H

#ifndef _WIN32
typedef int socket_t;
#else
#include <Windows.h>
typedef SOCKET socket_t;
#endif



#endif /* TSOCKET_H */
