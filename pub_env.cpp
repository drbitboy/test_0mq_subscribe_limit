//  Pubsub envelope publisher

// gcc -o % -D_SOCKET='"_sckpubsub"' -Izmqtop/include %.c -Lzmqtop/lib -lzmq

#include <string>
#include <iostream>

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "zmq.h"

using namespace std;

int main (void)
{
// Create context and publisher socket
void *context = zmq_ctx_new ();
void *publisher = zmq_socket (context, ZMQ_PUB);

int int0 = 0;

  // Bind publisher socket
# ifndef _SOCKET
# define _SOCKET "_sckpubsub"
# endif//_SOCKET
  zmq_bind (publisher, "ipc://" _SOCKET);

  // Set sending High-Water Mark (HWM) to zero => no HWM
  if (zmq_setsockopt (publisher, ZMQ_SNDHWM, &int0, sizeof(int0))) {
    cerr << "E: SNDHWM failed" << strerror (errno) << endl;
  }
  // Set receiving HWM to zero
  // - this should have no effect as this is the publisher, but just in case:
  if (zmq_setsockopt (publisher, ZMQ_RCVHWM, &int0, sizeof(int0))) {
    cerr << "E: RCVHWM failed" << strerror (errno) << endl;
  }

  // Publish messages to with envelopes 0,65535, 512,511, 1024,1023
  // 1536,1535, 2048,2047
  for (uint16_t u16=0; 1; u16 = (u16+512)%2560) {
  int rtn;
  string incoming_string;

    // Publish two messages, with envelopes u16 and (u16-1)
    for (uint16_t zo=0; zo<2; ++zo) {
    uint16_t u16val;
      u16val = u16 - zo;
      //  Build message with an envelope and content
      incoming_string = to_string(u16val);
      incoming_string.insert(0, 1, u16val>>8);   // Envelope high byte
      incoming_string.insert(0, 1, u16val%256);  // Envelope low byte
      // Write [envelope + content] to COUT
      cout << incoming_string << "|";
      // Publish message
      rtn = zmq_send (publisher, incoming_string.c_str(), incoming_string.size(), 0);
      if (rtn < 1) { cout << "A... rtn=" << rtn << std::endl; }
    }
    cout << endl;
    sleep(1);
  }
  //  We never get here, but clean up anyhow
  zmq_close (publisher);
  zmq_ctx_destroy (context);
  return 0;
}
