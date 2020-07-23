#This is a hack to pass arguments to the run command and probably only 
#works with gnu make. 
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


all: ftpServer

#The following lines contain the generic build options
CC=gcc
CLIBS=-pthread
CPPFLAGS=
CFLAGS=-g -Werror-implicit-function-declaration

#List all the .o files here that need to be linked 
OBJS=ftpServer.o usage.o dir.o utils.o sock.o connection.o command.o service.o thread_pool.o

thread_pool.o: thread_pool.h

command.o: command.c command.h connection.h sock.h dir.h

connection.o: connection.c connection.h sock.h utils.h

sock.o: sock.c sock.h

utils.o: utils.c utils.h

service.o: service.c service.h sock.h connection.h thread_pool.h

usage.o: usage.c usage.h

dir.o: dir.c dir.h

ftpServer.o: ftpServer.c dir.h usage.h service.h

ftpServer: $(OBJS) 
	$(CC) -o ftpServer $(OBJS) $(CLIBS) 

clean:
	rm -f *.o
	rm -f ftpServer

### ignore the below, for the hack above
.PHONY: run
run: ftpServer  
	./ftpServer $(RUN_ARGS)
