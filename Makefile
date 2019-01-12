EXECUTABLE = tetris
OBJECT_FILES = drawLogic.o main.o gameLogic.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	gcc -g -o $(EXECUTABLE) $(OBJECT_FILES) `sdl2-config --libs` -lSDL2_ttf

main.o: main.c drawLogic.h
	gcc -g -c -o main.o main.c

gameLogic.o: gameLogic.c gameLogic.h
	gcc -g -c -o gameLogic.o gameLogic.c

drawLogic.o: drawLogic.c drawLogic.h gameLogic.h
	gcc -g -c -o drawLogic.o drawLogic.c `sdl2-config --cflags`

clean:
	rm -f $(OBJECT_FILES) $(EXECUTABLE)
