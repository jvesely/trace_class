
PROJECT=trace_class
OBJS = \
	address_table.o \
	main.o \
	feature_vector.o \
	instruction.o \
	processor.o

CFLAGS= -Wall -Wextra -std=c99 -O2
LDFAGS=


$(PROJECT): $(OBJS)
	gcc $^ -lz -o $@

clean:
	rm -vf $(PROJECT) $(OBJS) Makefile.depend

Makefile.depend:
	@gcc -MM $(OBJS:.o=.c) > Makefile.depend

-include Makefile.depend
