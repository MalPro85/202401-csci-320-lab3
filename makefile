OBJECTS = lab3.o main.o

validate_sudoku: $(OBJECTS)
	gcc -pthread -o $@ $(OBJECTS) 

lab3.o:lab3.c lab3.h
	gcc -c lab3.c

main.o: main.c lab3.h
	gcc -c main.c

clean:
	-rm *.o core validate_sudoku
