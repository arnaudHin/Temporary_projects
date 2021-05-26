#
# Hello Robot C - Makefile du package comTelco des sources.
# @author Arnaud Hincelin
# Organisation des sources.

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)


#
# Règles du Makefile.
#

# Compilation.
all: $(OBJ)

.c.o:
	@$(CC) -c $(CCFLAGS) $< -o $@
	
# Nettoyage.
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)













