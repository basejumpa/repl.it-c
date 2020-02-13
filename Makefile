.SUFFIXES:

include test/_test.mk

.DEFAULT: main

.PHONY: main

main: main.exe
./main.exe

SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)

main.exe: ${OBJECTS}

-include $(OBJECTS:%.o=%.d)
