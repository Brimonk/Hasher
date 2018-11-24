CC := cc
SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)
LINKER  := 
ARGS   := -Wall -march=native
DEBUGARGS   := -g3
TARGET := hasher

all: $(TARGET)

%.o: %.c
	$(CC) -c $(ARGS) $(DEBUGARGS) -o $@ $<

clean: clean-obj clean-bin

clean-obj:
	rm -f $(OBJECTS)

clean-bin:
	rm -f $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(ARGS) -o $(TARGET) $(OBJECTS) $(LINKER)
