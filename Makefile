#
#
BINDIR = ./bin

S_EXES = example_server
C_EXES = test_client
LIB_NET = olptnet
LIB_MSG= protocol

RM :=rm -f 

NET_OBJS =src/net/tcp_socket_addr.o \
	src/net/async_acceptor.o  \
	src/net/connection_handler.o \
	src/net/stream_buffer.o \
	src/net/event_dispatcher.o \
	src/net/service_loop.o  \
	src/net/service_loop_thread.o \
	src/net/thpool_service_loop.o \
	src/net/net_base.o \
	src/net/timer_handler.o 
	
MSG_OBJS =src/protocol/packet_message.o
	
S_OBJS=src/server/server_main.o \
	src/server/log4z.o \
	src/common/exception_error.o \
	src/server/acceptor_server.o \
	src/server/service_handler.o  

C_OBJS =src/client/main.o

CXXFLAGS = -g -Wall
CPPFLAGS = -I./src
LIBS =-L./lib -lpthread -lrt

all: libs server test client

dir:
	if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR) ; fi;

libs:  dir $(LIB_NET) $(LIB_MSG)
	
$(LIB_NET): $(NET_OBJS)
	ar -cr $(BINDIR)/lib$@.a  $^
	
$(LIB_MSG): $(MSG_OBJS)
	ar -cr $(BINDIR)/lib$@.a  $^

server:   $(S_OBJS)
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(S_EXES)  $^ -L$(BINDIR)  -l$(LIB_MSG) -l$(LIB_NET) $(LIBS)

client:  
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(C_EXES)  src/client/main.cc -L$(BINDIR) -l$(LIB_MSG) -l$(LIB_NET) 

test:
	g++ $(CXXFLAGS) $(CPPFLAGS) src/test/protocol_unittest.cpp  -o $(BINDIR)/protocol_unittest -L$(BINDIR) -l$(LIB_MSG)
	
clean:
	$(RM) $(NET_OBJS) $(MSG_OBJS) $(S_OBJS)  $(C_OBJS)
	$(RM) $(BINDIR)/*
#
#
