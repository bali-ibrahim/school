CC=gcc
DEPS = oddEven.h
OBJ = main.o odd.o even.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main: $(OBJ)
	$(CC) -o $@ $^


# all: main.o even.o odd.o
# 	gcc main.o even.o odd.o -o main.exe
# main.o: main.c
# 	gcc -c main.c
# odd.o: odd.c
# 	gcc -c odd.c
# even.o: even.c
# 	gcc -c even.c
