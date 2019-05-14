ZMQTOP=../..

_SOCKET="_sckpubsub"
EXES=pub_env sub_env

all: $(EXES)

%: %.cpp
	g++ -std=c++11 -D_SOCKET='$(_SOCKET)' -o $@ -I$(ZMQTOP)/include $< -L$(ZMQTOP)/lib -lzmq

clean:
	$(RM) $(EXES) $(_SOCKET)

