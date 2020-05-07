CC=gcc
# DEPS = oddEven.h
DEPS =
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main.exe: $(OBJ)
	$(CC) -o $@ $^


.PHONY: clean

clean:
	rm -f ./*.o *~ core ./*~ 