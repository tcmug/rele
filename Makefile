

CC = g++
#INCLUDEDIR =
CFLAGS = -std=c++11 -Wall -O2 $(INC) -I./local/include
STATICLIBS =
LIBS = -L./local/lib -L./lib -lpthread -lcrypto -lssl -ltls

LDFLAGS = $(LIBS) $(STATICLIBS)

EXEFLAGS = -lrele
LIBFLAGS = -shared



all: lib/librele.so
all: rele


##
## LIBRARY
##

lib/librele.so: tmp/response.o \
		tmp/router.o \
		tmp/request.o \
		tmp/route.o \
		tmp/dynamic_source.o \
		tmp/logger.o
	$(CC) $(LIBFLAGS) $(LDFLAGS) -o $@ $^

tmp/response.o: src/response.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/request.o: src/request.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/route.o: src/route.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/router.o: src/router.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/logger.o: src/logger.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

##
## EXECUTABLE
##

rele:	tmp/main.o \
		tmp/buffer.o \
		tmp/server_thread.o \
		tmp/server_process.o \
		tmp/thread.o \
		tmp/socket.o \
		tmp/route.o \
		tmp/dynamic_source.o \
		tmp/logger.o \
		tmp/ssl_socket.o
	$(CC) $(LDFLAGS) $(EXEFLAGS) -o $@ $^

tmp/server_thread.o: src/server_thread.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/server_process.o: src/server_process.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/buffer.o: src/buffer.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/main.o: src/main.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/thread.o: src/thread.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/socket.o: src/socket.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/ssl_socket.o: src/ssl_socket.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

tmp/dynamic_source.o: src/dynamic_source.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

docs:
	mkdir -p doc/html && naturaldocs -i src -o HTML doc/html -p doc

tests: test_sockets

test_sockets:
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_sockets tests/test_sockets.cpp src/ssl_socket.cpp src/socket.cpp

clean:
	rm -rf rele doc tmp/* lib/*
