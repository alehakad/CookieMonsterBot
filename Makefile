all:
	gcc -ansi -pedantic-errors -Wall -Wextra -DNDEBUG -oFast board.c board_test.c bitmap.c -o board.out

debug:
	gcc -ansi -pedantic-errors -Wall -Wextra -g board.c board_test.c bitmap.c -o board.out