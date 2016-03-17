CPP = gcc 
OPTS = `pkg-config --cflags --libs gio-2.0`

all:
	$(CPP)  utils.c main.c -o ../gino $(OPTS)

clean:
	rm gino
