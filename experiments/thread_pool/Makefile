CC=g++
CFLAGS= -g -Wall -D_REENTRANT -D_GNU_SOURCE -fPIC -std=c++11
SOFLAGS= -shared
LDFLAGS= -lpthread

TARGET=nkthreadpool.so
TESTR=testr

CFLAGS += -I.. -I.
LDFLAGS += -L..

#------------------------------------------------------------------
LIBOBJS=nkthreadpool.o \
		nkqueue.o

TESTOBJS=./test/example.o

#------------------------------------------------------------------
all: $(TARGET) $(TESTR)

$(TARGET): $(LIBOBJS)
	$(CC) $(SOFLAGS) $^ -o $@

$(TESTR): $(TESTOBJS) $(LIBOBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(TESTR) $(LIBOBJS) $(TESTOBJS)

#------------------------------------------------------------------
#make rule

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

%.o : %.cc
	$(CC) $(CFLAGS) -c $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

