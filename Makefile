
PROJECT=trace_class
OBJS = \
	address_table.o \
	feature_vector.o \
	instruction.o \
	main.o \
	processor.o

CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS= -lz


$(PROJECT): $(OBJS)
	gcc $^ $(LDFLAGS) -o $@

clean:
	rm -vf $(PROJECT) $(OBJS) Makefile.depend

Makefile.depend:
	@gcc -MM $(OBJS:.o=.c) > Makefile.depend

-include Makefile.depend
