CFILES := src/main.c src/algos.c
LIBS   := -lssl -lcrypto -lpthread
OUTPUT := hasher

all:
	gcc -Wall $(CFILES) -o $(OUTPUT)  $(LIBS)

debug:
	gcc -Wall $(CFILES) -g $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)
