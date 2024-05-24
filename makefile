all: test

clean:
	rm -vf *.o
	rm -vf testUtils.exe
	rm -vf sdlUtils.a

sdlUtils.o: sdlUtils.h sdlUtils.c
	gcc -c sdlUtils.c

test.o: sdlUtils.h test.c
	gcc -c test.c

library: sdlUtils.o
	ar -rc libsdlUtils.a sdlUtils.o

lib: library

test: sdlUtils.o test.o
	gcc -o testUtils test.o sdlUtils.o -lSDL2

