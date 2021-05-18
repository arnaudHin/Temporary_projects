#
# Hello Robot C - Makefile du package commando des sources.
# @author Arnaud Hincelin
# Makefile Mère
all:
	@echo ---------compilation du module commando---------
	#cd commando; make; done 
	make -C commando
	@echo ---------compilation du module telco---------
	#cd telco; make; done 
	make -C telco

# Nettoyage.
.PHONY: clean

clean:
	@echo ---------clean du module commando---------
	@(cd commando && $(MAKE) $@)
	@echo ---------clean du module telco---------
	@(cd telco && $(MAKE) $@)

