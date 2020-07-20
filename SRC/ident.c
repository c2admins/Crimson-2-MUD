/* *********************************************************************
*  file: ident.c , Ident module.        			       *
*  Idents IPs without causing lag.                                     *
********************************************************************* */

#define __IDENT_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "db.h"
#include "ident.h"
#include "ban.h"

/* max time in seconds to make someone wait before entering game */
#define IDENT_TIMEOUT 10
#define IDENT_PORT    113


void nonblock(int s)
{
	int flags;

	flags = fcntl(s, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if (fcntl(s, F_SETFL, flags) < 0) {
		perror("SYSERR: Fatal error executing nonblock (comm.c)");
		exit(1);
	}
}
/* log the errno/WSALastError() message */
void logerror(const char *msg)
{
	main_log(msg);
}
/* start the process of looking up remote username */
void ident_start(struct descriptor_data * d, long addr)
{
	int sock;
	struct sockaddr_in sa;

	void nonblock(int s);

	/* if (!ident) { STATE(d) = CON_ASKNAME; d->ident_sock =
	 * INVALID_SOCKET; return; } */
	d->idle_tics = 0;

	/*
         * create a nonblocking socket, and start
         * the connection to the remote machine
         */

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		logerror("socket");
		d->ident_sock = INVALID_SOCKET;
		STATE(d) = CON_ASKNAME;
		return;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = ntohs(IDENT_PORT);
	sa.sin_addr.s_addr = addr;

	nonblock(sock);
	d->ident_sock = sock;

	errno = 0;

	if (connect(sock, (struct sockaddr *) & sa, sizeof(sa)) < 0) {
		if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
			/* connection in progress */
			STATE(d) = CON_IDCONING;
			return;
		}

		/* connection failed */
		else if (errno != ECONNREFUSED)
			logerror("ident connect");

		STATE(d) = CON_ASKNAME;
	}

	else			/* connection completed */
		STATE(d) = CON_IDCONED;
}


void ident_check(struct descriptor_data * d, int pulse)
{
	fd_set fd, efd;
	int rc, rmt_port, our_port, len;
	char user[256], *p;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	struct timeval null_time;
	extern int port;

	/*
         * Each pulse, this checks if the ident is ready to proceed to the
         * next state, by calling select to see if the socket is writeable
         * (connected) or readable (response waiting).
         */

	switch (STATE(d)) {
	case CON_IDCONING:
		/* waiting for connect() to finish */

		if (d->ident_sock != INVALID_SOCKET) {
			FD_ZERO(&fd);
			FD_ZERO(&efd);
			FD_SET(d->ident_sock, &fd);
			FD_SET(d->ident_sock, &efd);
		}

		if ((rc = select(d->ident_sock + 1, (fd_set *) 0, &fd,
				 &efd, &null_time)) == 0)
			break;

		else if (rc < 0) {
			logerror("ident check select (conning)");
			STATE(d) = CON_ASKNAME;
			break;
		}

		if (FD_ISSET(d->ident_sock, &efd)) {
			/* exception, such as failure to connect */
			STATE(d) = CON_ASKNAME;
			break;
		}

		STATE(d) = CON_IDCONED;

	case CON_IDCONED:
		/* connected, write request */

		sprintf(buf, "%d, %d\n\r", ntohs(d->peer_port), port);

		len = strlen(buf);
		if (write(d->ident_sock, buf, len) != len) {
			if (errno != EPIPE)	/* read end closed (no remote
						 * identd) */
				logerror("ident check write (conned)");

			STATE(d) = CON_ASKNAME;
			break;
		}

		STATE(d) = CON_IDREADING;

	case CON_IDREADING:
		/* waiting to read */

		if (d->ident_sock != INVALID_SOCKET) {
			FD_ZERO(&fd);
			FD_ZERO(&efd);
			FD_SET(d->ident_sock, &fd);
			FD_SET(d->ident_sock, &efd);
		}

		if ((rc = select(d->ident_sock + 1, &fd, (fd_set *) 0,
				 &efd, &null_time)) == 0)
			break;

		else if (rc < 0) {
			logerror("ident check select (reading)");
			STATE(d) = CON_ASKNAME;
			break;
		}

		if (FD_ISSET(d->ident_sock, &efd)) {
			STATE(d) = CON_ASKNAME;
			break;
		}

		STATE(d) = CON_IDREAD;

	case CON_IDREAD:
		/* read ready, get the info */

		if ((len = read(d->ident_sock, buf, sizeof(buf) - 1)) < 0)
			logerror("ident check read (read)");

		else {
			buf[len] = '\0';
			if (sscanf(buf, "%u , %u : USERID :%*[^:]:%255s",
				   &rmt_port, &our_port, user) != 3) {

				/* check if error or malformed */
				if (sscanf(buf, "%u , %u : ERROR : %255s",
					   &rmt_port, &our_port, user) == 3) {
					sprintf(buf2, "Ident error from %s: \"%s\"", d->host, user);
					main_log(buf2);
				}
				else {
					/* strip off trailing newline */
					for (p = buf + len - 1; p > buf && ISNEWL(*p); p--);
					p[1] = '\0';

					sprintf(buf2, "Malformed ident response from %s: \"%s\"",
						d->host, buf);
					main_log(buf2);
				}
			}
			else {

				/*
				 * Combine our username, '@', and host name and copy it into
				 * d->host.  The hostname is the most important, so it gets
				 * priority.  Any extra space we fill with as much of the
				 * user name as possible, and an '@'.  The total length must
				 * be equal to or less than HOST_LENGTH.
				 */

				len = sizeof(d->host) - strlen(d->host);

				if (len > 0) {
					strncpy(buf2, user, len - 1);
					buf2[len - 1] = '\0';
					strcat(buf2, "@");
					strcat(buf2, d->host);
					strcpy(d->host, buf2);
				}

				/* if len <= 0, no space for username */
			}
		}

		STATE(d) = CON_ASKNAME;

	case CON_ASKNAME:
		/* ident complete, ask for name */

		/* close up the ident socket, if one is opened. */
		if (d->ident_sock != INVALID_SOCKET) {
			close(d->ident_sock);
			d->ident_sock = INVALID_SOCKET;
		}
		d->idle_tics = 0;

		/* extra ban check */
		if (bn1100_isbanned(d->host) == BAN_ALL) {
			sprintf(buf, "Connection attempt denied from [%s]", d->host);
			main_log(buf);
			spec_log(buf, SYSTEM_LOG);
			co2500_close_single_socket(d);
			return;
		}

		SEND_TO_Q("\x1B[2K\n\rBy what name do you wish to be known? ", d);
		STATE(d) = CON_GET_NAME;
		return;

	default:
		return;
	}

	/*
         * Print a dot every second so the user knows he hasn't been forgotten.
         * Allow the user to go on anyways after waiting IDENT_TIMEOUT seconds.
         */
	if ((pulse % 16) == 0) {
		SEND_TO_Q(".", d);

		if (d->idle_tics++ >= IDENT_TIMEOUT)
			STATE(d) = CON_ASKNAME;
	}
}


/* returns 1 if waiting for ident to complete, else 0 */
int waiting_for_ident(struct descriptor_data * d)
{
	switch (STATE(d)) {
		case CON_IDCONING:
		case CON_IDCONED:
		case CON_IDREADING:
		case CON_IDREAD:
		case CON_ASKNAME:
		return 1;

	default:
		return 0;
	}

	return 0;
}
