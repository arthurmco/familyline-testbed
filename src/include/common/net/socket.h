/* 
   Socket definitions for both client and server
   
   Copyright (C) 2017 Arthur M
*/

#ifndef TSOCKET_H
#define TSOCKET_H

#ifndef _WIN32
typedef int socket_t;
#else
#define NOMINMAX //prevents Windows from redefining std::min
#include <winsock2.h>
typedef SOCKET socket_t;
typedef SSIZE_T ssize_t;
#endif



#endif /* TSOCKET_H */
