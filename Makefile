OBJECT_FILES = drawLogic.o main.o gameLogic.o

all: $(OBJECT_FILES)

main.o: main.c drawLogic.h
	gcc -c -o main.o main.c

gameLogic.o: gameLogic.c gameLogic.h
	gcc -c -o gameLogic.o gameLogic.c

drawLogic.o: drawLogic.c drawLogic.h gameLogic.h
	gcc -c -o drawLogic.o drawLogic.c

clean:
	rm -f $(OBJECT_FILES)
