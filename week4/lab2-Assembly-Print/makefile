
all: print_int

print_int.o: print_int.s
	nasm -g -f elf64 -F dwarf print_int.s -o print_int.o

print_int: print_int.o
	gcc -g print_int.o -o print_int

clean:
	rm -f *.o *~ print_int
