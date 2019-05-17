# Example of a PUB/SUB pair of sockets using two-byte envelopes, and the releveance of High-Water Mark (HWM) settings to subscriptions

Usage:

    ./pub_env [--no-receive-hwm]  ### Publisher
    ./sub_env [--no-send-hwm]     ### Subscriber

- These programs are meant to run simultaneously on the same host
  - E.g. use separate terminals
- Both programs will run until interrupted
- Order of execution of those commands is not important

- pub_env loops over sending pairs of messages, at 1Hz, with two-byte envelopes containing
  - 0,65536
  - 8192,8191
  - 16384,16383
  - ...
  - 57344,57343

- N.B. --no-receive-hwm option for publisher seems to have no effect

- sub_env subscribes to all possible two-byte 65,536 envelope
  - This will be unsuccessful if --no-send-hwm option is supplied
    - That option skips setting ZMQ_SNDHWM to 0 (=> infinite High-Water Mark)
      - Only first 1000 (default ZMQ_SNDHWM) ZMQ_SUBSCRIBEs will become effective
    - Only envelope 0 will be received, because
      - Only first 1000 subscriptions, for envelopes 0 through 999, will be received by subscriber
    - HOWEVER
      - If publisher is interrupted (e.g. Control-C), and restarted
      - Then subscriber will receive messages from all envelopes, 0 through 65535
      - This is, to this developer, an unexpected behavior

# New

cf. https://lists.zeromq.org/pipermail/zeromq-dev/2019-May/032968.html

Two changes to make this work:

1. Put setsockopt/SNDHWM/RCVHWM calls before connect and bind
2. Subscriber needs to set SNDHWM to 0 (or 65536?) so more than 1000 ZMQ_SUBSCRIBEs make it to the publisher


# Old

I duplicated Benjy's results (libzmq 4.3.2; master from Github)

  https://lists.zeromq.org/pipermail/zeromq-dev/2019-May/032968.html

That is, on starting pub_env and sub_env, in either order, sub_env only picks up envelopes 512, 511, and 1023 - N.B. sub_env subscribes to envelopes 24 through 2600, inclusive, but normally does not not receive any envelope above 1023.

HOWEVER, I did also stumble onto another odd behavior:  if pub_evn and sub_env are running, and I kill pub_env and restart it, then sub_env picks ups 512, 1024, 1536, and 2048, but not 511, 1023, 1535 or 2047.
