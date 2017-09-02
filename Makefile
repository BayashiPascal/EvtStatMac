OPTIONS_DEBUG=-ggdb -g3 -Wall
OPTIONS_RELEASE=-O3 
OPTIONS=$(OPTIONS_DEBUG)
INCPATH=/home/bayashi/Coding/Include
LIBPATH=/home/bayashi/Coding/Include

all : haircut disease epidemic cashier evtstatmac

clean:
	rm *.o haircut disease epidemic cashier evtstatmac
	
haircut : haircut.o evtstatmac.o Makefile
	gcc $(OPTIONS) evtstatmac.o haircut.o -o haircut -lm

disease : disease.o evtstatmac.o Makefile
	gcc $(OPTIONS) evtstatmac.o disease.o -o disease -lm

epidemic : epidemic.o evtstatmac.o Makefile
	gcc $(OPTIONS) $(LIBPATH)/tga.o evtstatmac.o epidemic.o -o epidemic -lm

cashier : cashier.o evtstatmac.o Makefile
	gcc $(OPTIONS) $(LIBPATH)/gset.o evtstatmac.o cashier.o -o cashier -lm

evtstatmac : evtstatmac_main.o evtstatmac.o Makefile
	gcc $(OPTIONS) evtstatmac.o evtstatmac_main.o -o evtstatmac -lm

haircut.o : evtstatmac.h haircut.c Makefile
	gcc $(OPTIONS) -c haircut.c

disease.o : evtstatmac.h disease.c Makefile
	gcc $(OPTIONS) -c disease.c

epidemic.o : evtstatmac.h epidemic.c Makefile $(INCPATH)/tga.h
	gcc $(OPTIONS) -I$(INCPATH) -c epidemic.c

cashier.o : evtstatmac.h cashier.c Makefile $(INCPATH)/gset.h
	gcc $(OPTIONS) -I$(INCPATH) -c cashier.c

evtstatmac_main.o : evtstatmac_main.c evtstatmac.h Makefile
	gcc $(OPTIONS) -c evtstatmac_main.c

evtstatmac.o : evtstatmac.c evtstatmac.h Makefile
	gcc $(OPTIONS) -c evtstatmac.c

valgrind :
	valgrind -v --track-origins=yes --leak-check=full --gen-suppressions=yes --show-leak-kinds=all ./evtstatmac
