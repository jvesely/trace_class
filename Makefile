
PROJECT=trace_class
OBJS = \
	main.o \
	feature_vector.o \
	instruction.o \
	processor.o

CFLAGS= -Wall -Wextra -std=c99 -O2
LDFAGS=


$(PROJECT): $(OBJS)
	gcc $^ -o $@

clean:
	rm -vf $(PROJECT) $(OBJS) Makefile.depend

-include Maefile.depend
