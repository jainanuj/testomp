
CC= gcc
ICC = icc
CPP= /usr/local/opt/llvm/bin/clang
CPPFLAGS = -I/usr/local/opt/llvm/include 
#LDFLAGS = -L/usr/local/opt/llvm/lib
LDFLAGS = -lgomp
CCOPTS= -Wall -g -DLOGGER_ALWAYS_FLUSH -qopenmp 

all: testomp

#C_OBJS=testomp.o

testomp_OBJS= testomp.o
#testomp_OBJS= penmp

testomp: $(testomp_OBJS)
	$(ICC) $(CCOPTS) -o testomp $(testomp_OBJS)

#
# Generic rules
#

%.o: %.c %.h
	$(ICC) $(CCOPTS) -c $<

%.o: %.c
	$(ICC) $(CCOPTS) -c $<

clean:
	rm -f *.o *.a *.il *.ti *\~ core core.* testomp 
