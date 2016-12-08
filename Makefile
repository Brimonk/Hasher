CFILES := src/pntlm.c src/algos.c
LIBS   := -lssl -lcrypto -lpthread

all:
	gcc -Wall $(CFILES) -o pntlm $(LIBS)

debug:
	gcc -Wall $(CFILES) -g -o pntlm $(LIBS)

clean:
	rm -f pntlm
