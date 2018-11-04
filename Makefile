OBJECT_FILES = drawLogic.o

all: drawLogic.o

drawLogic.o: drawLogic.c drawLogic.h gameLogic.h
	gcc -c -o drawLogic.o drawLogic.c

clean:
	rm -f $(OBJECT_FILES)
