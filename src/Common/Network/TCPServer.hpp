#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/select.h>

#include <boost/function.hpp>

#include <map>

namespace Common {

/*!
 * \brief Simple single-thread multi-client TCP server.
 *
 * Every incoming message is checked for its completness and serviced by external
 * functions, which can be assigned with almost no restrictions.
 */
class TCPServer
{
protected:
	/*!
	 * Data buffer to gather data
	 */
	struct DataBuffer {
		/// Size of data
		int size;
		/// Buffer itself
		unsigned char * buf;
	};

public:
	/// Function descriptor of service hook
	typedef boost::function<int (const unsigned char *, int, unsigned char *, int)> service_hook_t;

	/// Function descriptor of completion hook
	typedef boost::function<int (const unsigned char *, int)> completion_hook_t;

	/*!
	 * Constructor.
	 *
	 * Start server, create socket and start listening.
	 *
	 * \param port port number to listen on
	 * \param max_cons maximum numbers of simultaneous connections allowed
	 * \param buffer_size size of input buffers for each client
	 */
	TCPServer(int port = 30000, int max_cons = 10, int buffer_size = 200000);

	/*!
	 * Destructor.
	 */
	virtual ~TCPServer();

	/*!
	 * Set function called when new packet arrives.
	 */
	void setServiceHook(service_hook_t h);

	/*!
	 * Set function used to determine packet completness.
	 */
	void setCompletionHook(completion_hook_t);

	/*!
	 * Start service loop.
	 *
	 * This method is blocking, so it should be called in separate thread.
	 */
	void start();

	/*!
	 * Stop service loop.
	 */
	void stop();

protected:

	/*!
	 * Prepare buffers for new client.
	 *
	 * \param client_sock client's socket descriptor
	 */
	void prepareNewClient(int client_sock);

	/*!
	 * Accept new client.
	 *
	 * Creates socket for new connection, adds it to socket list
	 * and creates all required buffers.
	 */
	void acceptNewClient();

	/*!
	 * Handle data sent from client.
	 *
	 * \param i client socket descriptor
	 */
	void handleClient(int i);

private:
	void setNonBlocking(bool b);

	// get sockaddr, IPv4 or IPv6:
	void *get_in_addr(struct sockaddr *sa);


	/// Server socket (one on which it listens for incoming connections)
	int m_sock;

	/// Server address
	sockaddr_in m_addr;

	/// List of all opened sockets (server socket as well as client sockets)
	fd_set m_socks;

	/// Maximum socket descriptor
	int m_maxfd;

	/// Function called when new data comes
	service_hook_t m_service_hook;

	/// Function called to determine incoming packet completness
	completion_hook_t m_completion_hook;

	/// Flag indicating server state
	volatile bool m_running;

	/// Maximum size of buffer for single client
	int m_buffer_size;

	/// Buffers for all clients
	std::map<int, DataBuffer> m_buffers;

	/// Buffer in which clients stores reply
	unsigned char * m_reply_buffer;

	/// Temporary buffer
	unsigned char * m_tmp_buffer;
};

}

#endif /* TCPSERVER_HPP_ */
