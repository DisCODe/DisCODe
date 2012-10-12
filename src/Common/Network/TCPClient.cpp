#include "TCPClient.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>
#include <iostream>
#include <cstdio>

namespace Common {

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

TCPClient::TCPClient(int buffer_size) : m_sock(-1), m_buffer_size(buffer_size), m_buf(NULL), m_size(0)
{
	m_buf = new unsigned char[m_buffer_size];
	m_tmp_buf = new unsigned char[m_buffer_size];
}

TCPClient::~TCPClient()
{
	close(m_sock);
	delete [] m_buf;
	delete [] m_tmp_buf;
}

bool TCPClient::connect(const std::string & host, const std::string & port)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
		std::cout << "getaddrinfo: " << gai_strerror(rv) << std::endl;
		return false;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((m_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		if (::connect(m_sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(m_sock);
			perror("client: connect");
			continue;
		}
		break;
	}
	if (p == NULL) {
		std::cout << "client: failed to connect\n";
		return false;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof s);
	std::cout << "client: connecting to " << s << std::endl;

	freeaddrinfo(servinfo); // all done with this structure
	return true;
}

int TCPClient::recv(int msec_timeout)
{
	int recvd = 0, res;
	bool ready = false;
	int select_return;
	fd_set sock;

	struct timeval tv, *ptv;
	if (msec_timeout >= 0) {
		tv.tv_sec = msec_timeout / 1000;
		tv.tv_usec = msec_timeout % 1000 * 1000;
		ptv = &tv;
	} else {
		ptv = NULL;
	}


	while (!ready) {
		FD_ZERO(&sock);
		FD_SET(m_sock, &sock);
//		std::cout << "Waiting on select...\n";
		select_return = select(m_sock+1, &sock, NULL, NULL, ptv);

		if (select_return == -1) {
			perror("Select failed!");
			return -1;
		}
		if (select_return == 0) {
			// select timed out.
			return m_size;
		}

//		std::cout << "Got sth...\n";
		if (FD_ISSET(m_sock, &sock)) {
			recvd = ::recv(m_sock, m_buf+m_size, m_buffer_size - m_size, NULL);
			m_size += recvd;

			int expected_packet_size = m_completion_hook(m_buf, m_size);
			int skip = 0;

			// handle all completed packets from buffer
			while (expected_packet_size <= m_size) {

				m_service_hook(m_buf+skip, expected_packet_size);
				skip += expected_packet_size;
				m_size -= expected_packet_size;

				ready = true;

				if (m_size > 0)
					expected_packet_size = m_completion_hook(m_buf+skip, m_size);
				else
					break;
			}

			// check, if any packet was processed
			if (skip > 0) {
				// copy remaining portion of packet to the begining of buffer
				// it's safer to copy it to temporary buffer and then exchange
				// buffers than just calling memcpy, because source and destination
				// can be overlapped (memmove would work, but it's much slower)

				// copy remaining data to the temporary buffer
				memcpy(m_tmp_buf, m_buf+skip, m_size);

				// exchange temporary buffer with client buffer
				unsigned char * tmp = m_buf;
				m_buf = m_tmp_buf;
				m_tmp_buf = tmp;
			}
		}

	}

	return recvd;
}

int TCPClient::send(const unsigned char * msg, int size)
{
	int total = 0;
	// how many bytes we've sent
	int bytesleft = size; // how many we have left to send
	int n;

	while (total < size) {
		n = ::send(m_sock, msg + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

void TCPClient::setServiceHook(service_hook_t h)
{
	m_service_hook = h;
}

void TCPClient::setCompletionHook(completion_hook_t h) {
	m_completion_hook = h;
}

}
