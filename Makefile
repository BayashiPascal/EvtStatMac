OPTIONS_DEBUG=-ggdb -g3 -Wall
OPTIONS_RELEASE=-O3 
OPTIONS=$(OPTIONS_RELEASE)
INCPATH=/home/bayashi/Coding/Include
LIBPATH=/home/bayashi/Coding/Include

all : pbmake_wget haircut disease epidemic cashier evtstatmac
	
# Automatic installation of the repository PBMake in the parent folder
pbmake_wget:
	if [ ! -d ../PBMake ]; then wget https://github.com/BayashiPascal/PBMake/archive/master.zip; unzip master.zip; rm -f master.zip; sed -i '' 's@ROOT_DIR=.*@ROOT_DIR='"`pwd | gawk -F/ 'NF{NF-=1};1' | sed -e 's@ @/@g'`"'@' PBMake-master/Makefile.inc; mv PBMake-master ../PBMake; fi


clean:
	rm *.o haircut disease epidemic cashier evtstatmac
	
haircut : haircut.o evtstatmac.o Makefile $(LIBPATH)/pbmath.o $(LIBPATH)/gset.o $(LIBPATH)/bcurve.o
	gcc $(OPTIONS) evtstatmac.o haircut.o $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o -o haircut -lm

disease : disease.o evtstatmac.o Makefile $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o
	gcc $(OPTIONS) evtstatmac.o disease.o $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o -o disease -lm

epidemic : epidemic.o evtstatmac.o Makefile $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o
	gcc $(OPTIONS) $(LIBPATH)/tgapaint.o $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o evtstatmac.o epidemic.o -o epidemic -lm

cashier : cashier.o evtstatmac.o Makefile $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o
	gcc $(OPTIONS) $(LIBPATH)/gset.o $(LIBPATH)/pbmath.o $(LIBPATH)/bcurve.o evtstatmac.o cashier.o -o cashier -lm

evtstatmac : evtstatmac_main.o evtstatmac.o Makefile $(LIBPATH)/pbmath.o $(LIBPATH)/gset.o $(LIBPATH)/bcurve.o
	gcc $(OPTIONS) evtstatmac.o evtstatmac_main.o $(LIBPATH)/pbmath.o  $(LIBPATH)/bcurve.o $(LIBPATH)/gset.o -o evtstatmac -lm

haircut.o : evtstatmac.h haircut.c Makefile $(INCPATH)/pbmath.h
	gcc $(OPTIONS) -c haircut.c

disease.o : evtstatmac.h disease.c Makefile $(INCPATH)/pbmath.h
	gcc $(OPTIONS) -c disease.c

epidemic.o : evtstatmac.h epidemic.c Makefile $(INCPATH)/tgapaint.h $(LIBPATH)/pbmath.h
	gcc $(OPTIONS) -I$(INCPATH) -c epidemic.c

cashier.o : evtstatmac.h cashier.c Makefile $(INCPATH)/gset.h $(INCPATH)/pbmath.h
	gcc $(OPTIONS) -I$(INCPATH) -c cashier.c

evtstatmac_main.o : evtstatmac_main.c evtstatmac.h Makefile $(INCPATH)/pbmath.h
	gcc $(OPTIONS) -c evtstatmac_main.c

evtstatmac.o : evtstatmac.c evtstatmac.h Makefile $(INCPATH)/pbmath.h
	gcc $(OPTIONS) -c evtstatmac.c

valgrind :
	valgrind -v --track-origins=yes --leak-check=full --gen-suppressions=yes --show-leak-kinds=all ./evtstatmac
