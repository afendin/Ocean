BINARIES := Ocean
all: $(BINARIES)

CFLAGS := $(CFLAGS) -g -Wall -Werror -D_POSIX_THREAD_SEMANTICS

THREADLIBS := Lock.o thread.o CV.o

clean:
	rm -f *.o $(BINARIES)

Ocean: main.cc $(THREADLIBS)
	g++ $(CFLAGS) $^ -lpthread -lrt -o $@

Lock.o: 3rdparty/Lock.cc
	g++ -c $(CFLAGS) $< -o $@

thread.o: 3rdparty/thread.c
	gcc -c $(CFLAGS) $< -o $@

CV.o: 3rdparty/CV.cc
	g++ -c $(CFLAGS) $< -o $@
