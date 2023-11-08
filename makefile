raytracer : ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -lm -lSDL2 -Wall -Wextra -pedantic-errors -std=c2x -pg

test : ./src/test.c ./include
	gcc -o test ./src/test.c -I./include -lm

clean : 
	rm raytracer
