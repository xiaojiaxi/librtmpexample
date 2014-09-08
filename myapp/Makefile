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
INC=-I..

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
#LIB_RTMP=-Llibrtmp -lrtmp
LIB_RTMP=-L../librtmp -lrtmp
LIBS=$(LIB_RTMP) $(CRYPTO_LIB) $(LIBS_$(SYS)) $(XLIBS)

THREADLIB_posix=-lpthread
THREADLIB_darwin=-lpthread
THREADLIB_mingw=
THREADLIB=$(THREADLIB_$(SYS))
SLIBS=$(THREADLIB) $(LIBS)

#LIBRTMP=librtmp/librtmp.a
#INCRTMP=librtmp/rtmp_sys.h librtmp/rtmp.h librtmp/log.h librtmp/amf.h
LIBRTMP=../librtmp/librtmp.a
INCRTMP=../librtmp/rtmp_sys.h ../librtmp/rtmp.h ../librtmp/log.h ../librtmp/amf.h

EXT_posix=
EXT_darwin=
EXT_mingw=.exe
EXT=$(EXT_$(SYS))

PROGS=publishflv

all:	$(LIBRTMP) $(PROGS)

$(PROGS): $(LIBRTMP)

install:	$(PROGS)
	-mkdir -p $(BINDIR) 
	cp publishflv$(EXT) $(BINDIR)
	
clean:
	rm -f *.o publishflv$(EXT) 

FORCE:

$(LIBRTMP): FORCE
	@cd ../librtmp; $(MAKE) all

publishflv: publishflv.o myflv.o
	$(CC) $(LDFLAGS) -o $@$(EXT) $@.o myflv.o $(LIBS) 

publishflv.o: publishflv.c $(INCRTMP) Makefile
myflv.o: myflv.h Makefile
