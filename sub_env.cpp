// Pubsub envelope subscriber

// gcc -o % -D_SOCKET='"_sckpubsub"' -Izmqtop/include %.c -Lzmqtop/lib -lzmq

#include <string>
#include <iostream>

#include <string.h>
#include <errno.h>

#include "zmq.h"

using namespace std;

int main (void)
{
  // Prepare context and subscriber socket
  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);

  int int0 = 0;

  // Set sending High-Water Mark (HWM) to zero => no HWM
  // - So publisher can receive all subscriptions
  if (zmq_setsockopt (subscriber, ZMQ_SNDHWM, &int0, sizeof(int0))) {
    cerr << "E: SNDHWM failed" << strerror (errno) << endl;
  }

  // Set receiving (HWM) to zero => no HWM
  // - Not necessary (?) in this particular app as publisher is sending
  //   only two messages per second, so subscriber should be able to
  //   keep up
  if (zmq_setsockopt (subscriber, ZMQ_RCVHWM, &int0, sizeof(int0))) {
    cerr << "E: RCVHWM failed" << strerror (errno) << endl;
  }

  // Use default socket name of _sckpubsub if _SOCKET macro is undefined
# ifndef _SOCKET
# define _SOCKET "_sckpubsub"
# endif//_SOCKET

  // Connect subscriber socket
  zmq_connect (subscriber, "ipc://" _SOCKET);

  // Subscribe to envelopes from 0u to 65535u
  // - Publisher is sending 0,65535, 8192,8191, 16384,16383,
  //   32768,32767, 40960,40959, 49152,49151, 57344,57343
  // - So all of those should be received
  for (uint16_t u16 = 0u; 1; ++u16) {
    if (zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, &u16, 2)) {
      cerr << "E: subscriptions failed" << strerror (errno) << endl;
    }
    if (65535u == u16) break;
  }

  while (1) {
  char buffer[256];
  uint16_t* pu16 = (uint16_t*) buffer;
  // Read envelope and content
  int sz = zmq_recv (subscriber, buffer, 254, 0);
    if (-1 == sz) {
      printf("sz==-1\n");
      continue;
    }
    buffer[sz<255?sz:255] = '\0';   // Terminate content

    // Write envelope (Unsigned 16-bit integer) and content to COUT
    cout << "size=" << sz << "; envelope=" << *pu16 << "; content=[" << (buffer+sizeof(uint16_t)) << "]" << endl;
  }
  // We never get here, but clean up anyhow
  zmq_close (subscriber);
  zmq_ctx_destroy (context);
  return 0;
}
