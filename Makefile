CC     = gcc
CFLAGS = -O3 -Wall
LDLIBS = -lm

mandel: mandel.c
	$(CC) $(CFLAGS) mandel.c -o mandel $(LDLIBS)

run: mandel
	./mandel

clean:
	rm -f mandel

.PHONY: run clean
