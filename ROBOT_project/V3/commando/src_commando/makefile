#
# Hello Robot C - Makefile du package telco des sources.
# @author Arnaud Hincelin
# Organisation des sources.

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

CCFLAGS += -I..

#
# Règles du Makefile.
#

# Compilation.
all: $(OBJ)

.c.o:
	@$(CC) -c $(CCFLAGS) $< -o $@
	@make -C comCommando
	
# Nettoyage.
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)
	@(cd comCommando && $(MAKE) $@)













