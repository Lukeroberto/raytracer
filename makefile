raytracer : ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -I./libraries -lm -lSDL2 -O3 -march=native -Ofast -ffast-math

debug: ./src/main.c ./include/*.h
	gcc -o raytracer ./src/main.c -I./include -I./libraries -lm -lSDL2 -std=c2x -pg -gdwarf-4 -Wall -Wpedantic -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -pedantic -fsanitize=undefined,address

test : ./tests/unit_tests.c ./include/*.h
	gcc -o test ./tests/unit_tests.c -I./include -I./libraries -lm -lSDL2 -std=c2x -pg -gdwarf-4 -Wall -Wpedantic -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address 

clean : 
	rm -f raytracer test
