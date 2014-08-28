VERSION=v2.4

prefix=/usr/local

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

SYS=posix
#SYS=mingw

CRYPTO=OPENSSL
#CRYPTO=POLARSSL
#CRYPTO=GNUTLS
LIBZ=-lz
LIB_GNUTLS=-lgnutls -lhogweed -lnettle -lgmp $(LIBZ)
LIB_OPENSSL=-lssl -lcrypto $(LIBZ)
LIB_POLARSSL=-lpolarssl $(LIBZ)
CRYPTO_LIB=$(LIB_$(CRYPTO))
DEF_=-DNO_CRYPTO
CRYPTO_DEF=$(DEF_$(CRYPTO))

DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)
OPT=-O2
CFLAGS=-Wall $(XCFLAGS) $(INC) $(DEF) $(OPT)
LDFLAGS=-Wall $(XLDFLAGS)

bindir=$(prefix)/bin
sbindir=$(prefix)/sbin
mandir=$(prefix)/man

BINDIR=$(DESTDIR)$(bindir)
SBINDIR=$(DESTDIR)$(sbindir)
MANDIR=$(DESTDIR)$(mandir)

LIBS_posix=
LIBS_darwin=
LIBS_mingw=-lws2_32 -lwinmm -lgdi32
LIB_RTMP=-Llibrtmp -lrtmp
LIBS=$(LIB_RTMP) $(CRYPTO_LIB) $(LIBS_$(SYS)) $(XLIBS)

THREADLIB_posix=-lpthread
THREADLIB_darwin=-lpthread
THREADLIB_mingw=
THREADLIB=$(THREADLIB_$(SYS))
SLIBS=$(THREADLIB) $(LIBS)

ALLINCLUDE=../..
RTMPINCLUDE=../../librtmp
APPINCLUDE=../../app
MYAPPINCLUDE=../../myapp
DOCINFLUDE=../../doc


LIBRTMP=$(RTMPINCLUDE)/librtmp.a
INCRTMP=$(RTMPINCLUDE)/rtmp_sys.h $(RTMPINCLUDE)/rtmp.h $(RTMPINCLUDE)/log.h $(RTMPINCLUDE)/amf.h

EXT_posix=
EXT_darwin=
EXT_mingw=.exe
EXT=$(EXT_$(SYS))

PROGS=rtmpdump rtmpgw rtmpsrv rtmpsuck sendflvrtmp

all:	$(LIBRTMP) $(PROGS)

$(PROGS): $(LIBRTMP)

install:	$(PROGS)
	-mkdir -p $(BINDIR) $(SBINDIR) $(MANDIR)/man1 $(MANDIR)/man8
	cp rtmpdump$(EXT) $(BINDIR)
	cp rtmpgw$(EXT) rtmpsrv$(EXT) rtmpsuck$(EXT) $(SBINDIR)
	cp $(DOCINFLUDE)/rtmpdump.1 $(MANDIR)/man1
	cp $(DOCINFLUDE)/rtmpgw.8 $(MANDIR)/man8
	@cd $(RTMPINCLUDE); $(MAKE) install

clean:
	rm -f *.o rtmpdump$(EXT) rtmpgw$(EXT) rtmpsrv$(EXT) rtmpsuck$(EXT) sendflvrtmp$(EXT)
	@cd $(RTMPINCLUDE); $(MAKE) clean

FORCE:

$(LIBRTMP): FORCE
	@cd ${RTMPINCLUDE}; $(MAKE) all

rtmpdump: rtmpdump.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o $(LIBS) -I$(RTMPINCLUDE) -L$(RTMPINCLUDE)

rtmpsrv: rtmpsrv.o thread.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o thread.o $(SLIBS) -I$(RTMPINCLUDE) -L$(RTMPINCLUDE)

rtmpsuck: rtmpsuck.o thread.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o thread.o $(SLIBS) -I$(RTMPINCLUDE) -L$(RTMPINCLUDE)

rtmpgw: rtmpgw.o thread.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o thread.o $(SLIBS) -I$(RTMPINCLUDE) -L$(RTMPINCLUDE)
	
sendflvrtmp: sendflvrtmp.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o $(SLIBS) -I$(RTMPINCLUDE) -L$(RTMPINCLUDE) -Wl,-rpath=./librtmp:.:$(RTMPINCLUDE)/librtmp 

rtmpgw.o: $(APPINCLUDE)/rtmpgw.c $(INCRTMP) Makefile
	$(CC) $(CFLAGS) -c -o $@ $(APPINCLUDE)/rtmpgw.c -I$(ALLINCLUDE)
rtmpdump.o: $(APPINCLUDE)/rtmpdump.c $(INCRTMP) Makefile
	$(CC) $(CFLAGS) -c -o $@ $(APPINCLUDE)/rtmpdump.c -I$(ALLINCLUDE)
rtmpsrv.o: $(APPINCLUDE)/rtmpsrv.c $(INCRTMP) Makefile
	$(CC) $(CFLAGS) -c -o $@ $(APPINCLUDE)/rtmpsrv.c -I$(ALLINCLUDE)
rtmpsuck.o: $(APPINCLUDE)/rtmpsuck.c $(INCRTMP) Makefile
	$(CC) $(CFLAGS) -c -o $@ $(APPINCLUDE)/rtmpsuck.c -I$(ALLINCLUDE)
thread.o: $(APPINCLUDE)/thread.c $(APPINCLUDE)/thread.h
	$(CC) $(CFLAGS) -c -o $@ $(APPINCLUDE)/thread.c -I$(ALLINCLUDE)
sendflvrtmp.o: $(MYAPPINCLUDE)/sendflvrtmp.c $(INCRTMP) Makefile
	$(CC) $(CFLAGS) -c -o $@ $(MYAPPINCLUDE)/sendflvrtmp.c -I$(ALLINCLUDE)
