CFILES := src/*.c
LIBS   := -lpthread -ldl
OUTPUT := hasher

all:
	gcc -Wall $(CFILES) -o $(OUTPUT)  $(LIBS)

debug:
	gcc -Wall $(CFILES) -g -o $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)
