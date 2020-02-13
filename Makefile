.SUFFIXES:

include test/_test.mk

.DEFAULT_GOAL := main

.PHONY: main
main: main.exe
	./main.exe

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

main.exe: $(OBJECTS)
	gcc -o main.exe $(OBJECTS)

-include $(OBJECTS:%.o=%.d)

.PHONY: clean
clean: clean_local
	rm -f *.exe
