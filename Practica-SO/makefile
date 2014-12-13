all: Dozer Gekko

LlistaPDI.o: LlistaPDI.c LlistaPDI.h Tipus_dozer.h
	gcc -Wall -Wextra -c LlistaPDI.c

Fitxer_dozer.o: Fitxer_dozer.c Fitxer_dozer.h Tipus_dozer.h LlistaPDI.h
	gcc -Wall -Wextra -c Fitxer_dozer.c

Shell.o: Shell.c Shell.h Tipus_dozer.h
	gcc -Wall -Wextra -c Shell.c

dozer.o: dozer.c LlistaPDI.h Tipus_dozer.h Fitxer_dozer.h Shell.h
	gcc -Wall -Wextra -c dozer.c

Fitxer_gekko.o: Fitxer_gekko.c Fitxer_gekko.h Tipus_gekko.h
	gcc -Wall -Wextra -c Fitxer_gekko.c

gekko.o: gekko.c Tipus_gekko.h Fitxer_gekko.h
	gcc -Wall -Wextra -lpthread -c gekko.c

Gekko: Fitxer_gekko.o gekko.o
	gcc Fitxer_gekko.o gekko.o -Wall -Wextra -lpthread -o Gekko

Dozer: LlistaPDI.o Fitxer_dozer.o Shell.o dozer.o
	gcc LlistaPDI.o Fitxer_dozer.o Shell.o dozer.o -Wall -Wextra -o Dozer

clean:
	rm -rf *.o Dozer
	rm -rf *.o Gekko

