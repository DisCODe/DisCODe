#include "TCPServer.hpp"

#include <fcntl.h>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "DisCODeException.hpp"

namespace Common {


int defaultCompletionHook(const unsigned char *, int size) {
	return size > 0 ? size : 1;
}

TCPServer::TCPServer(int port, int max_cons, int buffer_size) : m_buffer_size(buffer_size)
{
	const int yes = 1;

	m_reply_buffer = NULL;
	m_tmp_buffer = NULL;

	memset(&m_addr, 0, sizeof(m_addr));

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == -1) {
		std::cout << "Socket creation failed." << std::endl;
		return;
	}

	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);

	int bind_return = bind(m_sock, (struct sockaddr *) &m_addr, sizeof(m_addr));

	if (bind_return == -1) {
		std::cout << "Bind failed." << std::endl;
		return;
	}

	int listen_return = listen(m_sock, max_cons);

	if (listen_return == -1) {
		std::cout << "Listen failed." << std::endl;
		return;
	}

	FD_SET(m_sock, &m_socks);
	m_maxfd = m_sock;

	m_reply_buffer = new unsigned char[m_buffer_size];
	m_tmp_buffer = new unsigned char[m_buffer_size];

	m_completion_hook = defaultCompletionHook;
}

TCPServer::~TCPServer()
{
	if (m_sock != -1)
		close( m_sock);

	/// \todo Remove all buffers

	delete [] m_reply_buffer;
	delete [] m_tmp_buffer;

	m_reply_buffer = NULL;
	m_tmp_buffer = NULL;
}

void TCPServer::acceptNewClient() {
	socklen_t addrlen;
	int newfd;
	struct sockaddr_storage remoteaddr; // client address
	char remoteIP[INET6_ADDRSTRLEN];

	std::cout << "Accept..." << std::endl;
	// handle new connections
	addrlen = sizeof remoteaddr;
	newfd = accept(m_sock, (struct sockaddr *) &remoteaddr, &addrlen);
	if (newfd == -1) {
		perror("Accept failed.");
	} else {
		FD_SET(newfd, &m_socks); // add to master set
		if (newfd > m_maxfd) {
			// keep track of the max
			m_maxfd = newfd;
		}

		std::cout << "New connection from "
				<< inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr), remoteIP, INET6_ADDRSTRLEN)
				<< " on " << newfd << std::endl;

		prepareNewClient(newfd);
	}
}

void TCPServer::handleClient(int i) {
	DataBuffer buffer = m_buffers[i];
	int nbytes;

	// handle data from a client
	nbytes = recv(i, buffer.buf+buffer.size, m_buffer_size-buffer.size, 0);

	if (nbytes < 0) {
		// got error
		//perror("recv failed!");
	} else if (!nbytes) {
		// connection closed
		std::cout << "Socket " << i << " hung up" << std::endl;
		close(i); // bye!
		FD_CLR(i, &m_socks); // remove from master set
	} else {
		//std::cout << "Read " << nbytes << " bytes." << std::endl;
		// we got some data from a client
		buffer.size += nbytes;

		int expected_packet_size = m_completion_hook(buffer.buf, buffer.size);
		int skip = 0;
		int reply_size;

		// handle all completed packets from buffer
		while (expected_packet_size <= buffer.size) {
			std::cout << "Expected packet size: " << expected_packet_size << std::endl;
			if ( (reply_size = m_service_hook(buffer.buf+skip, expected_packet_size, m_reply_buffer, m_buffer_size)) > 0) {
				std::cout << "Got reply in TCPServer. Sending...\n";
				int ss = m_reply_buffer[0] * 256 + m_reply_buffer[1];
				std::cout << "Reply size: " << reply_size << "=" << ss << std::endl;
				std::cout << m_reply_buffer+2 << std::endl;
				send(i, m_reply_buffer, reply_size, MSG_NOSIGNAL);
			}
			skip += expected_packet_size;
			buffer.size -= expected_packet_size;

			expected_packet_size = m_completion_hook(buffer.buf+skip, buffer.size);
		}

		// check, if any packet was processed
		if (skip > 0) {
			// copy remaining portion of packet to the begining of buffer
			// it's safer to copy it to temporary buffer and then exchange
			// buffers than just calling memcpy, because source and destination
			// can be overlapped (memmove would work, but it's much slower)

			// copy remaining data to the temporary buffer
			memcpy(m_tmp_buffer, buffer.buf+skip, buffer.size);

			// exchange temporary buffer with client buffer
			unsigned char * tmp = buffer.buf;
			buffer.buf = m_tmp_buffer;
			m_tmp_buffer = tmp;
		}
	}
}

void TCPServer::start()
{

	struct timeval tv;
	fd_set read_fds;

	m_running = true;

	while (m_running) {
		tv.tv_sec = 0;
		tv.tv_usec = 250000;
		read_fds = m_socks;

		int select_return = select(m_maxfd + 1, &read_fds, NULL, NULL, &tv);

		if (select_return == -1) {
			perror("Select failed!");

			switch(errno) {
			case EBADF:
				// An invalid file descriptor was given in one of the sets. (Perhaps a file descriptor that was already closed, or one on which an error has occurred.)
				std::cout << "An invalid file descriptor was given in one of the sets. (Perhaps a file descriptor that was already closed, or one on which an error has occurred.)\n";
				break;
			case EINTR:
				// A signal was caught; see signal(7).
				break;
			case EINVAL:
				// nfds is negative or the value contained within timeout is invalid.
				break;
			case ENOMEM:
				// unable to allocate memory for internal tables.
				throw Common::DisCODeException("TCPServe: Unable to allocate memory for internal tables");
			}
		}
		if (select_return == 0) {
			//std::cout << "Select timed out." << std::endl;
			continue;
		}

		// run through the existing connections looking for data to read
		for (int i = 0; i <= m_maxfd; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				// check, if server listening socket is ready
				if (i == m_sock) {
					acceptNewClient();
				} else {
					handleClient(i);
				} // END handle data from client
			} // END got new incoming connection

		}
	}

}

void TCPServer::stop() {
	m_running = false;
}

void TCPServer::setServiceHook(service_hook_t h)
{
	m_service_hook = h;
}

void TCPServer::setCompletionHook(completion_hook_t h)
{
	m_completion_hook = h;
}

void TCPServer::setNonBlocking(bool b)
{
	int opts;

	opts = fcntl(m_sock, F_GETFL);

	if (opts < 0) {
		return;
	}

	if (b)
		opts = (opts | O_NONBLOCK);
	else
		opts = (opts & ~O_NONBLOCK);

	fcntl(m_sock, F_SETFL, opts);
}

// get sockaddr, IPv4 or IPv6:
void *TCPServer::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return NULL;
}


void TCPServer::prepareNewClient(int client_sock) {
	unsigned char * buf = new unsigned char[m_buffer_size];
	DataBuffer buffer;
	buffer.size = 0;
	buffer.buf = buf;
	m_buffers[client_sock] = buffer;
}

}
