raytracer : ./src/main.c 
	gcc -o raytracer ./src/main.c -I./include -lm

tests : ./src/test.c
	gcc -o test ./src/test.c -I./include -lm

clean : 
	rm raytracer
