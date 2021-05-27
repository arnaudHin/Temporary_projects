#
# RemoteUI - Makefile
# @author Arnaud Hincelin
# Organisation des sources.

# Compilateur.
INTOXDIR = $(realpath ../infox_prose-x86_64-v0.3/)
export CC = gcc

# options de compilation
export CCFLAGS += -O0 
export CCFLAGS += -DNDEBUG # avec debuggage : -g -DDEBUG # sans debuggage : -DNDEBUG
export CCFLAGS += -MMD -MP # gestion automatique des dépendances
export CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
export CCFLAGS += -std=c99 -Wall -Wextra
export LDFLAGS += -lrt -pthread

# options de compilation pour l'utilisation de Intox/Infox
export CCFLAGS += -DINTOX
export CCFLAGS += -I$(INTOXDIR)/include/infox/prose/
export LDFLAGS += -L$(INTOXDIR)/lib/ -linfox

# Un niveau de package est accessible.
SRC = $(wildcard src_commando/*.c)
SRC += $(wildcard src_commando/comCommando/*.c)

# Pour ajouter un second niveau :		
# SRC += $(wildcard */*/*.c)

OBJ = $(SRC:.c=.o)

# Point d'entrée du programme.
MAIN = main.c

OBJ_MAIN = main.o

# Gestion automatique des dépendances.
DEP = $(MAIN:.c=.d)

# Exécutable à générer.
EXEC = remoteUI.elf

# Inclusion depuis le niveau du package.
CCFLAGS += -I.

#
# Règles du Makefile.
#


# Compilation.
all: $(EXEC)

$(EXEC): $(OBJ) $(OBJ_MAIN)
	@echo "----------Edition des liens--------------"
	$(CC) $(CCFLAGS) $(OBJ) $(OBJ_MAIN) -MF $(DEP) -o bin/$(EXEC) $(LDFLAGS)

$(OBJ):
	#@echo "----------Compilation--------------"
	@make -C src_commando

$(OBJ_MAIN) : $(MAIN)
	@$(CC) -c $(CCFLAGS) $(MAIN) -o $(OBJ_MAIN)

# Nettoyage.
.PHONY: clean

clean:
	@echo "----------Nettoyage--------------"
	@(cd src_commando && $(MAKE) $@)
	@rm -f $(DEP)
	@rm -f bin/$(EXEC)
	@rm -f $(OBJ_MAIN)

-include $(DEP)

