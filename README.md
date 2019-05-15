# New

cf. https://lists.zeromq.org/pipermail/zeromq-dev/2019-May/032968.html

Two changes to make this work:

1. Put setsockopt/SNDHWM/RCVHWM calls before connect and bind
2. Subscriber needs to set SNDHWM to 0 (or 65536?) so more than 1000 ZMQ_SUBSCRIBES make it to the publisher


# Old

I duplicated Benjy's results (libzmq 4.3.2; master from Github)

  https://lists.zeromq.org/pipermail/zeromq-dev/2019-May/032968.html

That is, on starting pub_env and sub_env, in either order, sub_env only picks up envelopes 512, 511, and 1023 - N.B. sub_env subscribes to envelopes 24 through 2600, inclusive, but normally does not not receive any envelope above 1023.

HOWEVER, I did also stumble onto another odd behavior:  if pub_evn and sub_env are running, and I kill pub_env and restart it, then sub_env picks ups 512, 1024, 1536, and 2048, but not 511, 1023, 1535 or 2047.
