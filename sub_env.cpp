//  Pubsub envelope subscriber

// gcc -o % -D_SOCKET='"_sckpubsub"' -Izmqtop/include %.c -Lzmqtop/lib -lzmq

#include <string>
#include <iostream>

#include <string.h>
#include <errno.h>

#include "zmq.h"


using namespace std;

int main (void)
{
  //  Prepare context and subscriber socket
  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);

  //  Connect subscriptersocket
# ifndef _SOCKET
# define _SOCKET "_sckpubsub"
# endif//_SOCKET
  zmq_connect (subscriber, "ipc://" _SOCKET);

  int int0 = 0;

  // Set receiving High-Water Mark (HWM) to zero => no HWM
  if (zmq_setsockopt (subscriber, ZMQ_RCVHWM, &int0, sizeof(int0))) {
    cerr << "E: RCVHWM failed" << strerror (errno) << endl;
  }
  // Set sending HWM to zero
  // - this should have no effect as this is the subscriber, but just in case:
  if (zmq_setsockopt (subscriber, ZMQ_SNDHWM, &int0, sizeof(int0))) {
    cerr << "E: SNDHWM failed" << strerror (errno) << endl;
  }

  // Subscribe to envelopes from 24u to 2600u
  // - If only first 1000 subcription "take," then only envelopes with
  //   values 24 through 1023 will be received
  // - Publisher is sending 0,65535, 512,511, 1023,1024, 1536,1535, 2048,2047
  // - So 1023 should be received, but not 1024 or above
  for (uint16_t u16 = 24u; u16< 2600u; ++u16) {
    if (zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, &u16, 2)) {
      cerr << "E: subscriptions failed" << strerror (errno) << endl;
    }
  }

  while (1) {
  char buffer[256];
  uint16_t* pu16 = (uint16_t*) buffer;
  //  Read envelope and content
  int sz = zmq_recv (subscriber, buffer, 254, 0);
    if (-1 == sz) {
      printf("sz==-1\n");
      continue;
    }
    buffer[sz<255?sz:255] = '\0'; // Terminate content

    // Write envelope (Unsigned 16-bit integer) and content to COUT
    cout << "size=" << sz << "; envelope=" << *pu16 << "; content=[" << (buffer+sizeof(uint16_t)) << "]" << endl;
  }
  //  We never get here, but clean up anyhow
  zmq_close (subscriber);
  zmq_ctx_destroy (context);
  return 0;
}
