#include "win_poll.h"

#ifdef _WIN32

// Our own version of poll, for Windows.
int ppoll(struct pollfd *fds, size_t nfds,
	const struct timespec *timeout_ts, const sigset_t *sigmask) {

	(void)sigmask;

	INT timeout = timeout_ts->tv_nsec / 1000000;
	return WSAPoll(fds, nfds, timeout);

}

// Our own version of poll, for Windows.
int poll(struct pollfd *fds, size_t nfds, INT timeout) {

	return WSAPoll(fds, nfds, timeout);

}

#endif
