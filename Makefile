CFLAGS = -Wall -W -g
OBJECTS = main.o command.o 
CC = gcc
TARGETS = pseudo-shell
RM = rm -rf

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGETS) $(OBJECTS)

main.o: main.c command.h 

command.o: command.c command.h 

clean:
	$(RM) $(TARGETS) $(OBJECTS)
