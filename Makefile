make: invertedIndex.c invertedIndex.h
	gcc -g -Wall -Werror -fsanitize=address -o invertedIndex invertedIndex.c

clean: invertedIndex
	rm invertedIndex
