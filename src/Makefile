CFLAGS=-Wall

all: clean test web probe autoprobe libnumberpic.so

web:  libnumberpic.so web.o
	gcc web.o -o web -L. -lnumberpic -L/opt/local/lib -Wl,-rpath,.

test: libnumberpic.so test.o
	gcc test.o -o test -L. -lnumberpic -L/opt/local/lib -Wl,-rpath,.

web.o: web.c libnumberpic.h
	gcc -c ${CFLAGS} -g web.c -o web.o -I/opt/local/include

test.o: test.c libnumberpic.h
	gcc -c ${CFLAGS} -g test.c -o test.o -I/opt/local/include

libnumberpic.so: libnumberpic.o
	gcc -shared libnumberpic.o -o libnumberpic.so -L/opt/local/lib -lpng

libnumberpic.o: libnumberpic.c libnumberpic.h
	gcc -fpic -c ${CFLAGS} libnumberpic.c -o libnumberpic.o -I/opt/local/include

probe: libnumberpic.so probe.o
	gcc -O3 -s probe.o -o probe -L. -lnumberpic -L/opt/local/lib -Wl,-rpath,.

probe.o: probe.c probe.h libnumberpic.h
	gcc -c ${CFLAGS} -g -O3 probe.c -o probe.o -I/opt/local/include

autoprobe: libnumberpic.so autoprobe.o
	gcc -O3 -s autoprobe.o -o autoprobe -L. -lnumberpic -L/opt/local/lib -Wl,-rpath,.

autoprobe.o: probe.c probe.h libnumberpic.h
	gcc -c ${CFLAGS} -g -O3 autoprobe.c -o autoprobe.o -I/opt/local/include

run_probe: probe
	taskset -c 0 ./probe

plot: Probe-numbers[5].csv Probe-numbers[5].csv plotter.py
	python3 plotter.py

clean:
	rm -f test test.o libnumberpic.o libnumberpic.so web.o autoprobe.o autoprobe web probe probe.o Probe-with* Probe-numbers*
