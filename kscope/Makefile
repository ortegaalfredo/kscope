all: sdlview time fftview showmax

clean:
	rm sdlview time fftview showmax

CFLAGS= -Wall -O3 -march=native
CFLAGSD= -Wall -O0 -g -march=native

sdlview : sdlview.c
	gcc $(CFLAGS) -o sdlview sdlview.c -lSDL

fftview : fftview.c
	gcc $(CFLAGS) -o fftview fftview.c -lSDL -lfftw3 -lm

time : time.c
	gcc $(CFLAGS) -o time time.c

showmax : showmax.c
	gcc $(CFLAGS) -o showmax showmax.c
