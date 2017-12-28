CFILES := src/*.c
LIBS   := -lpthread -ldl
ARGS   := -g3
OUTPUT := hasher

all:
	gcc -Wall $(CFILES) -o $(OUTPUT)  $(LIBS)

debug:
	gcc -Wall $(CFILES) -g -o $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)
