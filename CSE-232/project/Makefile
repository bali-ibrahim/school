CC=gcc
# DEPS = oddEven.h
DEPS =
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< -O3 -ggdb -m64

main.exe: $(OBJ)
	$(CC) -o $@ $^ -m64


.PHONY: clean

clean:
	rm -f ./*.o *~ core ./*~ 