#Copyright Raducanu George-Cristian
build:
	gcc -g -Wall -Wextra -Wshadow main.c -o image_editor
clean:
	rm -f *.o image_editor