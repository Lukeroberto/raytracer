raytracer : ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -lm -lSDL2 -O3 -g -ffast-math

debug: ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -lm -lSDL2 -std=c2x -g3 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address 

test : ./src/test.c ./include
	gcc -o test ./src/test.c -I./include -lm

clean : 
	rm raytracer
