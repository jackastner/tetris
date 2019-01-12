EXECUTABLE = tetris
OBJECT_FILES = drawLogic.o main.o gameLogic.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	gcc -g -o $(EXECUTABLE) $(OBJECT_FILES) `sdl2-config --libs` -lSDL2_ttf

main.o: src/main.c src/drawLogic.h
	gcc -g -c -o main.o src/main.c

gameLogic.o: src/gameLogic.c src/gameLogic.h
	gcc -g -c -o gameLogic.o src/gameLogic.c

drawLogic.o: src/drawLogic.c src/drawLogic.h src/gameLogic.h
	gcc -g -c -o drawLogic.o src/drawLogic.c `sdl2-config --cflags`

clean:
	rm -f $(OBJECT_FILES) $(EXECUTABLE)
