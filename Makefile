
PROJECT=trace_class
OBJS = \
	main.o \
	instruction.o
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFAGS=


$(PROJECT): $(OBJS)
	gcc $^ -o $@

clean:
	rm -vf $(PROJECT) $(OBJS) Makefile.depend

-include Maefile.depend
