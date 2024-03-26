/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Author: Jim Faulkner <newspost@sdf.lonestar.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"
#include "../ui/ui.h"

/**
*** Public Routines
**/

int socket_create(const char *address, int port) {
	int on;
	int error;
	char service[5];	/* Port shouldn't be > 65535 (5 chars) */

	int sockfd = -1;

	struct addrinfo		*res, *aip;
	struct addrinfo		hints;

	/* Get host address. */
	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_ALL | AI_ADDRCONFIG;
	hints.ai_socktype = SOCK_STREAM;

	/* Convert port to a type acceptable to getaddrinfo() */
	sprintf(service, "%d", port);

	error = getaddrinfo(address, service, &hints, &res);

	if (error != 0) {
		return FAILED_TO_RESOLVE_HOST;
	}

	/* Try all returned addresses until one works */
	for (aip = res; aip != NULL; aip = aip->ai_next) {

		/* Open socket.  Address type depends on what
		 * we got from getaddrinfo().
		 */
		 
		 sockfd = socket(aip->ai_family, aip->ai_socktype,
		 		 aip->ai_protocol);

		 if (sockfd == -1) {
		 	freeaddrinfo(res);
		 	return (FAILED_TO_CREATE_SOCKET);
		 }
		 
		/* Connect to host. */
		if (connect(sockfd, aip->ai_addr, aip->ai_addrlen) == -1) {
			(void) close(sockfd);
			sockfd = FAILED_TO_CREATE_SOCKET;
			continue;
		}
		break;
	}

	on = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
		(char*) &on, sizeof(on)) < 0) {
			close(sockfd);
			sockfd = FAILED_TO_CREATE_SOCKET;
	}

	freeaddrinfo(res);

	if (sockfd < 0)
		return FAILED_TO_CREATE_SOCKET;
		
        return sockfd;
}

void socket_close(int sockfd) {
	if (sockfd >= 0)
		close(sockfd);
}

/* returns the number of bytes written */
long socket_write(int sockfd, const char *buffer, long length) {
	long retval;

	retval =  write(sockfd, buffer, length);
	if(retval < 0){
		ui_socket_error(errno);
		return retval; /* never happens */
	}
	else{
		return retval;
	}
}

/* returns the number of bytes read */
long socket_getline(int sockfd, char *buffer) {
	long retval;
	char *pi;
	long i;
	long read_count = 0;

	i = 0;
	pi = buffer;
	while (read_count < STRING_BUFSIZE - 1) {
		retval = read(sockfd, pi, 1);
		if(retval < 0)
			ui_socket_error(errno);
		read_count += retval;
		pi++;
		if (buffer[i] == '\n')
			break;
		i++;
	}
	*pi = '\0';

	return read_count;
}
