#
# Hello Robot C - Makefile du package telco
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
SRC_TELCO  = $(wildcard src/*.c)

# Pour ajouter un second niveau :		
# SRC += $(wildcard */*/*.c)

OBJ_TELCO = $(SRC_TELCO:.c=.o)

#ajout des binaires de commando -> pour le linkage
COMMANDO = $(wildcard ../commando/src/*.o)

# Point d'entrée du programme.
MAIN_TELCO = main.c

OBJ_MAIN_TELCO = main.o

# Gestion automatique des dépendances.
DEP_TELCO = $(MAIN_TELCO:.c=.d)

# Exécutable à générer.
EXEC_TELCO = telco.elf

# Inclusion depuis le niveau du package.
CCFLAGS += -I.

#
# Règles du Makefile.
#


# Compilation.
# '@' devant une commande => commande silencieuse, n'est pas affichée
all: $(EXEC_TELCO)

$(EXEC_TELCO): $(OBJ_TELCO) $(OBJ_MAIN_TELCO) 
	$(CC) $(CCFLAGS) $(OBJ_TELCO) $(COMMANDO) $(OBJ_MAIN_TELCO) -MF $(DEP_TELCO) -o bin/$(EXEC_TELCO) $(LDFLAGS)

$(OBJ_TELCO):
	#cd src; make; done 
	make -C src

$(OBJ_MAIN_TELCO) : $(MAIN_TELCO)
	$(CC) -c $(CCFLAGS) $(MAIN_TELCO) -o $(OBJ_MAIN_TELCO)



# Nettoyage.
.PHONY: clean

clean:
	@(cd src && $(MAKE) $@)
	@rm -f $(DEP_TELCO)
	@rm -f bin/$(EXEC_TELCO)
	@rm -f $(OBJ_MAIN_TELCO)
#ou @(cd src && $(MAKE) $@)

-include $(DEP_TELCO)

