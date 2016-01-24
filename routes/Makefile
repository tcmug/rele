
INCLUDEDIR = ../src
CC = g++
CFLAGS = -std=c++11 -Wall -O2 -g $(INC) -I $(INCLUDEDIR)
STATICLIBS =
LIBS = -L../lib -lpthread -lrele
LDFLAGS	= -shared $(LIBS) $(STATICLIBS)

all: myroute.so

myroute.so: tmp/myroute.o
	$(CC) $(LDFLAGS) -o $@ $^

tmp/myroute.o: src/myroute.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f tmp/* *.so
