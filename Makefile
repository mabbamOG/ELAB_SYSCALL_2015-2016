#  run with -j# (# is number of threads for multithreaded compilation)

# DIRECTORIES
SRCDIR = src
BINDIR = bin
IDIR = $(SCRDIR)/header
LDIR = $(BINDIR)/lib

# FILES
#SRCS = main.c lib_ipc.c lib_io.c lib_error.c
#OBJS = $(SRCS:.c=.o)
#HDRS = lib_ipc.h lib_io.h lib_error.h

# COMPILERS AND FLAGS
CCFLAGS = -c -Wall -ggdb -I$(IDIR) # (-c no linking) (-Wall smart warnings) (-ggdb gdb debugger info) (-Idir look for .h in dir)
LDFLAGS = -L$(LDIR) # (-L look for obj files in dir)
CC = gcc $(CCFLAGS)
LD = gcc $(LDFLAGS)

#
all: main.x
	@echo "All Done!"

main.x: %.o # ??? can be : %.o ? or OBJS?
	@echo "==>linking $@"
	@$(LD) $^ -o $(BINDIR)/$@

%.o: $(SRCDIR)/%.c # wildcard compilation: default for any .o target!!!    # add %.h??? main does not have one!
	@echo "==>compiling $@"
	@$(CC) $< -o $(LDIR)/$@
 # WARNING: THE .H FILES ARE NOT ANALYZED FOR CHANGES!! MUST ADD THEM TO DEPENDENCIES!!!

# %.o: $(IDIR)/%.h


clean:
	@rm $(LDIR)/*.o $(BINDIR)/*.x
#doc:
#	...
#help:
#	...

.PHONY: all clean #doc help # tasks that will always run, ignoring if files called like them exist
