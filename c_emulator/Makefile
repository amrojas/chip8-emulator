SDLFLAGS = $(shell sdl2-config --libs --cflags)

play: main.o chip8.o screen.o
	gcc -o play main.o chip8.o $(SDLFLAGS)

main.o: main.c chip8.c chip8.h
	gcc -c main.c

chip8.o: chip8.c chip8.h
	gcc -c chip8.c

screen.o: screen.c
	gcc -c screen.c

clean:
	rm -f *.o play
