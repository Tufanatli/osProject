all: compile link run
compile:
	gcc -g -I ./include -o ./lib/fields.o -ggdb -c ./src/fields.c
	gcc -g -I ./include -o ./lib/main.o -ggdb -c ./src/main.c
link:
	gcc -g ./lib/*.o -o ./bin/program ./lib/libfdr.a
run:
	./bin/program ./dat/giris.dat