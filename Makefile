#  run with -j# (# is number of threads for multithreaded compilation)

# DIRECTORIES
SRCDIR = src
BINDIR = bin
IDIR = $(SRCDIR)/include
ODIR = $(BINDIR)/obj
DOCDIR = doc

# FILES
SRCS = $(wildcard $(SRCDIR)/*.c); # all source files in source dir
OBJS = $(addprefix $(ODIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS))))) # all corrensponding object files
MAIN = $(BINDIR)/main.x # all executable files to be made
HDRS = $(wildcard $(IDIR)/*.h); # all header files in header dir

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

$(ODIR)/%.o: $(SRCDIR)/%.c $(IDIR)/%.h
	@echo "==> compiling $@..."
	@mkdir -p "$(ODIR)"
	@$(CC) $< -o $@

# main.c is the only file not dependant on its own header file!
$(ODIR)/main.o: $(SRCDIR)/main.c
	@echo "==> compiling $@..."
	@mkdir -p "$(ODIR)"
	@$(CC) $< -o $@

clean:
	@rm -rf $(ODIR)
	@rm -rf $(BINDIR)
	@rm -rf $(DOCDIR)

doc:
	@doxygen

help:
	@echo "run: make or make <command>"
	@echo "available commands: all clean help doc"

.PHONY: all clean doc help # tasks that will always run, ignoring if files called like them exist
