CFLAGS = -g

objects = main.o dictionary.o utility.o server.o

spellCheck : $(objects)
	cc -pthread -o spellCheck $(objects) $(CFLAGS)

main.o : dictionary.h utility.h server.h

.PHONY : clean
clean :
	rm spellCheck $(objects)
