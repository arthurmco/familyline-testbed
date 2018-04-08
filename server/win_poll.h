/**
 * poll(2) and ppoll(2) adapters for Windows.
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef WIN_POLL_H
#define	WIN_POLL_H

#ifdef _WIN32
#define NOMINMAX //prevents Windows from redefining std::min
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>

#include <time.h> // timespec

typedef int sigset_t;

 // Our own version of poll, for Windows.
int ppoll(struct pollfd *fds, size_t nfds,
	const struct timespec *timeout_ts, const sigset_t *sigmask);

// Our own version of poll, for Windows.
int poll(struct pollfd *fds, size_t nfds, INT timeout);

#endif // WIN32


#endif // WIN_POLL_H
