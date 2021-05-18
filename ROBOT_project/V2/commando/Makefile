#
# Hello Robot C - Makefile du package commando
# @author Arnaud Hincelin
# Organisation des sources.

# Répertoire d'installation de la librairie Infox
# pour la compilation avec le simulateur Intox 
# INTOXDIR = $(realpath /<path>/<to>/infox_prose-<arch>-v0.3
# Exemple :
INTOXDIR = $(realpath ../infox_prose-x86_64-v0.3/)


# Compilateur.

export CC = gcc

# options de compilation
export CCFLAGS += -O0 
export CCFLAGS += -DNDEBUG # avec debuggage : -g -DDEBUG # sans debuggage : -DNDEBUG
export CCFLAGS += -MMD -MP # gestion automatique des dépendances
export CCFLAGS += -D_BSD_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
#warning car déclaration anticipée des énumerations : https://askcodez.com/c-declaration-anticipee-denumerations.html
#solution dans les options de compilation : enlever -pedantic et mettre -Wextra
#export CCFLAGS += -std=c99 -Wall -pedantic
export CCFLAGS += -std=c99 -Wall -Wextra
export LDFLAGS += -lrt -pthread

# options de compilation pour l'utilisation de Intox/Infox
export CCFLAGS += -DINTOX
export CCFLAGS += -I$(INTOXDIR)/include/
export LDFLAGS += -L$(INTOXDIR)/lib/ -linfox


# Un niveau de package est accessible.
SRC_COMMANDO  = $(wildcard src/*.c)

# Pour ajouter un second niveau :		
# SRC += $(wildcard */*/*.c)

OBJ_COMMANDO = $(SRC_COMMANDO:.c=.o)

# Point d'entrée du programme.
MAIN_COMMANDO = main.c

OBJ_MAIN_COMMANDO = main.o

# Gestion automatique des dépendances.
DEP_COMMANDO = $(MAIN_COMMANDO:.c=.d)

# Exécutable à générer.
EXEC_COMMANDO = commando.elf

# Inclusion depuis le niveau du package.
CCFLAGS += -I.

#
# Règles du Makefile.
#


# Compilation.
# '@' devant une commande => commande silencieuse, n'est pas affichée
all: $(EXEC_COMMANDO)

$(EXEC_COMMANDO): $(OBJ_COMMANDO) $(OBJ_MAIN_COMMANDO)
	$(CC) $(CCFLAGS) $(OBJ_COMMANDO) $(OBJ_MAIN_COMMANDO) -MF $(DEP_COMMANDO) -o bin/$(EXEC_COMMANDO) $(LDFLAGS)


$(OBJ_COMMANDO):
	#cd src; make; done
	make -C src
#creation de la lib statique de comando -> linkage pour telco	
#ar rcs libcommando.a src/pilot.o src/robot.o $(INTOXDIR)/lib/libinfox.a


$(OBJ_MAIN_COMMANDO) : $(MAIN_COMMANDO)
	$(CC) -c $(CCFLAGS) $(MAIN_COMMANDO) -o $(OBJ_MAIN_COMMANDO)



# Nettoyage.
.PHONY: clean

clean:
	@(cd src && $(MAKE) $@)
	@rm -f libcommando.a
	@rm -f $(DEP_COMMANDO)
	@rm -f bin/$(EXEC_COMMANDO)
	@rm -f $(OBJ_MAIN_COMMANDO)
#ou @(cd src && $(MAKE) $@)

-include $(DEP_COMMANDO)

