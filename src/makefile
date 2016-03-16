CPP = gcc 
OPTS = `pkg-config --cflags --libs gio-2.0`

all:
	$(CPP)  utils.c main.c -o ../bin/gino $(OPTS)

clean:
	rm gino
