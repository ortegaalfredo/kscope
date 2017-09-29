all: kscope-gui libkscope.so

clean:
	rm kscope-gui libkscope.so

CFLAGS= -Wall -O3 -march=native
CFLAGSD= -Wall -O0 -g -march=native

kscope-gui : kscope-gui.c kscope.c
	gcc $(CFLAGS) -o kscope-gui kscope-gui.c kscope.c -lSDL -lfftw3 -lm

libkscope.so: kscope.c
	gcc $(CFLAGS) -fpic -shared -o libkscope.so  kscope.c
