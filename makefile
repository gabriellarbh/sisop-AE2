all: sort2
sort2: ./obj/sort2.o
	gcc ./obj/sort2.o -o ./bin/sort2 

./obj/sort2.o: ./src/v2.c
	gcc -c ./src/v2.c -o ./obj/sort2.o

clear:
	rm obj/*.o
	rm bin/*

	

