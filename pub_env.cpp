// Pubsub envelope publisher

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

  // Set sending High-Water Mark (HWM) to zero => no HWM
  if (zmq_setsockopt (publisher, ZMQ_SNDHWM, &int0, sizeof(int0))) {
    cerr << "E: SNDHWM failed" << strerror (errno) << endl;
  }
  // Set receiving HWM to zero
  // - So this publisher can receive flood of subscription requests?
  if (zmq_setsockopt (publisher, ZMQ_RCVHWM, &int0, sizeof(int0))) {
    cerr << "E: RCVHWM failed" << strerror (errno) << endl;
  }
# endif

  // Use default socket name of _sckpubsub if _SOCKET macro is undefined
# ifndef _SOCKET
# define _SOCKET "_sckpubsub"
# endif//_SOCKET

  // Bind publisher socket
  zmq_bind (publisher, "ipc://" _SOCKET);

  // Publish messages to with envelopes 0,65535, 8192,8191, 16384,16383,
  // 32768,32767, 40960,40959, 57344,57343 repeating
  // - N.B. u16 rolls over after 65535 (57344u + 8192u => 0u
  for (uint16_t u16=0u; 1; u16+=8192u) {
  int rtn;
  string incoming_string;

    // Publish two messages, with envelopes u16 and (u16-1)
    for (uint16_t zo=0; zo<2; ++zo) {
    uint16_t u16val;
      u16val = u16 - zo;
      // Build message with an envelope and content
      incoming_string = to_string(u16val);
      incoming_string.insert(0, 1, (u16val>>8) && 0x0ffu);  // Envelope high byte
      incoming_string.insert(0, 1, u16val && 0x0ffu);       // Envelope low byte
      // Write [envelope + content] to COUT
      cout << incoming_string << "|";
      // Publish message
      rtn = zmq_send (publisher, incoming_string.c_str(), incoming_string.size(), 0);
      if (rtn < 1) { cout << "A... rtn=" << rtn << std::endl; }
    }
    cout << endl;
    sleep(1);
  }
  // We never get here, but clean up anyhow
  zmq_close (publisher);
  zmq_ctx_destroy (context);
  return 0;
}
