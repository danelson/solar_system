CC=gcc
CFLAGS=-I/usr/X11R6/include -L/usr/X11R6/lib -I/opt/local/include -L/opt/local/lib -I./include -bind_at_load

planets: planets.o tiff-util.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu -ltiff

clean:
	rm -f *.o *~ a.out