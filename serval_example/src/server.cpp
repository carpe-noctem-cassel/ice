#include <iostream>
#include <cstdio>
#include <cstring>
#include <signal.h>

#include <msp_cpp.h>

#define PAYLOAD_SIZE 16
#define PORT 8042
#define MSP_MESSAGE_SIZE 32

static int quit;

char outbuf[MSP_MESSAGE_SIZE];
int outlen;

size_t
io_handler(MSP_SOCKET sock, msp_state_t state,
		const uint8_t *payload, size_t len, void *context)
{
	int ret = 0;

	if (payload && len) {
		std::cout << "received payload with len:" << len << std::endl;

		for (int i = 0; i < len; i++)
			std::cout << payload[i];
		std::cout << std::endl;
		ret = len;
	}

	if (ret == len && (state & MSP_STATE_SHUTDOWN_REMOTE)) {
		std::cout << "received EOF" << std::endl;
	}

	/* respond to client */
	strcpy(outbuf, "Hello Client!");
	outlen = strlen(outbuf);

	if (outlen == 0) {
		msp_shutdown(sock);
	} else if (state & MSP_STATE_DATAOUT) {
		ssize_t sent = msp_send(sock, (uint8_t *)outbuf, outlen);

		/* if there was an error while sending? */
		if (sent == -1)
			msp_shutdown(sock);
	}

	/* quit program on error */
	if (state & (MSP_STATE_CLOSED | MSP_STATE_ERROR))
		quit = 1;

	return ret;
}


size_t 
listen_handler(MSP_SOCKET sock, msp_state_t state,
			const uint8_t *payload, size_t len, void *context)
{
	std::cout << "listen handler" << std::endl;

	if (state & (MSP_STATE_ERROR | MSP_STATE_CLOSED))
		quit = 1;
	else {
		// ... set up resources needed for the new connection ...
		msp_set_handler(sock, io_handler, NULL);

		if (payload && len)
			return io_handler(sock, state, payload, len, NULL);
	}

	return 0;
}


void
cleanup(int signal)
{
	quit = 1;
}


int
main()
{
	int mdp_sock;
	MSP_SOCKET msp_sock;
	struct mdp_sockaddr addr;
	struct sigaction act;
	struct timeval timeout;
 	time_ms_t now;
	time_ms_t next_time = 0;

	/* sample message to reply to clients */
	strcpy((char *)outbuf, "Hello World!");

	if ((mdp_sock = mdp_socket()) < 0) {
		std::cerr << "server: error creating mdp socket" << std::endl;
		return -1;
	}
	msp_sock = msp_socket(mdp_sock, 0);

	addr.sid = SID_ANY;
	addr.port = PORT;

	msp_set_local(msp_sock, &addr);
	msp_set_handler(msp_sock, listen_handler, NULL);
	msp_listen(msp_sock);
	if (!msp_socket_is_open(msp_sock)) {
		std::cerr << "server: error listening on msp socket" << std::endl;
		return -1;
	}

	/* cleanup on CTRL-C */
	memset(&act, 0, sizeof(act));
	act.sa_handler = &cleanup;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		fprintf(stderr, "error registering signal handler\n");
		return -1;
	}

	/* main loop */
	std::cout << "server: starting server..." << std::endl;
	quit = 0;
	while (!quit) {
		now = gettime_ms();
		if (now < next_time) {
			struct timeval timeout = time_ms_to_timeval(next_time - now);
			setsockopt(mdp_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			msp_recv(mdp_sock);
		}
		msp_processing(&next_time);
	}

	std::cout << "server: cleanup..." << std::endl;
	msp_close_all(mdp_sock);
	mdp_close(mdp_sock);

	return 0;
}
