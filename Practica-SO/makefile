all: Dozer Gekko

LlistaPDIAccio.o: LlistaPDIAccio.c LlistaPDIAccio.h Tipus_dozer.h
	gcc -Wall -Wextra -c LlistaPDIAccio.c

Fitxer_dozer.o: Fitxer_dozer.c Fitxer_dozer.h Tipus_dozer.h LlistaPDIAccio.h
	gcc -Wall -Wextra -c Fitxer_dozer.c

Shell.o: Shell.c Shell.h Tipus_dozer.h
	gcc -Wall -Wextra -c Shell.c

dozer.o: dozer.c LlistaPDIAccio.h Tipus_dozer.h Fitxer_dozer.h Shell.h
	gcc -Wall -Wextra -c dozer.c

Fitxer_gekko.o: Fitxer_gekko.c Fitxer_gekko.h Tipus_gekko.h
	gcc -Wall -Wextra -c Fitxer_gekko.c

LlistaPDIVenta.o: LlistaPDIVenta.c LlistaPDIVenta.h Tipus_gekko.h
	gcc -Wall -Wextra -c LlistaPDIVenta.c

gekko.o: gekko.c Tipus_gekko.h Fitxer_gekko.h LlistaPDIVenta.h
	gcc -Wall -Wextra -c gekko.c

Gekko: Fitxer_gekko.o LlistaPDIVenta.o gekko.o
	gcc Fitxer_gekko.o LlistaPDIVenta.o gekko.o -Wall -Wextra -lpthread -o Gekko

Dozer: LlistaPDIAccio.o Fitxer_dozer.o Shell.o dozer.o
	gcc LlistaPDIAccio.o Fitxer_dozer.o Shell.o dozer.o -Wall -Wextra -lpthread -o Dozer

clean:
	rm -rf *.o Dozer
	rm -rf *.o Gekko

