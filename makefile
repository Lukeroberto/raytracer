raytracer : ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -lm -lSDL2 -O3 -march=native -Ofast -ffast-math

debug: ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -lm -lSDL2 -std=c2x -pg -gdwarf-4 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address

test : ./src/test.c ./src/unit_tests.c ./include/*.h
	#gcc -o test ./src/unit_tests.c -I./include -lm -lSDL2 -std=c2x -fsanitize=undefined,address,leak -pg

	gcc -o test ./src/test.c -I./include -lm -lSDL2 -O3 -march=native -Ofast -ffast-math #-g -std=c2x -fsanitize=undefined,address -g3
	#gcc -o test ./src/test.c -I./include -lm -lSDL2 -g -std=c2x -fsanitize=undefined,address,leak -pg

clean : 
	rm raytracer test
