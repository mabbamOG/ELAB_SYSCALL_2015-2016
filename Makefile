#  run with -j# (# is number of threads for multithreaded compilation)

# DIRECTORIES
SRCDIR = src
BINDIR = bin
IDIR = $(SRCDIR)/include
ODIR = $(BINDIR)/obj

# FILES
SRCS = $(wildcard $(SRCDIR)/*.c); # all source files in source dir
OBJS = $(addprefix $(ODIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS))))) # all corresponding object files
#HDRS = lib_ipc.h lib_io.h lib_error.h
MAIN = $(BINDIR)/main.x # all executable files to be made

# COMPILERS AND FLAGS
CCFLAGS = -c -Wall -ggdb -I$(IDIR) # (-c no linking) (-Wall smart warnings) (-ggdb gdb debugger info) (-Idir look for .h in dir)
CC = gcc $(CCFLAGS)
LD = gcc $(LDFLAGS)

all: $(MAIN)
	@echo "==> ALL DONE!"


$(MAIN): $(OBJS)
	@echo "==> linking $@..."
	@mkdir -p "$(BINDIR)"
	@$(LD) $^ -o $@

$(ODIR)/%.o: $(SRCDIR)/%.c # add %.h??? main does not have one!
	@echo "==> compiling $@..."
	@mkdir -p "$(ODIR)"
	@$(CC) $< -o $@

 # WARNING: THE .H FILES ARE NOT ANALYZED FOR CHANGES!! MUST ADD THEM TO DEPENDENCIES!!!

# %.o: $(IDIR)/%.h


clean:
	@rm -rf $(ODIR)
	@rm -rf $(BINDIR)
#doc:
#	...
help:
	@echo "run: make or make <command>"
	@echo "available commands: all clean help doc debug"

debug:
	@echo "$(SRCS)"
	@echo "$(OBJS)"

.PHONY: all clean #doc help debug # tasks that will always run, ignoring if files called like them exist
