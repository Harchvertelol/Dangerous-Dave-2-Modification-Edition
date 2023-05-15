VERSION = 2.0

ZLIBDIR=../zlib
ENETDIR=../enet
IRRNETDIR=1

ZLIBOBJ = $(ZLIBDIR)/adler32.o $(ZLIBDIR)/compress.o $(ZLIBDIR)/crc32.o $(ZLIBDIR)/deflate.o $(ZLIBDIR)/infback.o $(ZLIBDIR)/inffast.o $(ZLIBDIR)/inflate.o $(ZLIBDIR)/inftrees.o $(ZLIBDIR)/trees.o $(ZLIBDIR)/uncompr.o $(ZLIBDIR)/zutil.o
#ENETOBJ = $(ENETDIR)/host.o $(ENETDIR)/list.o $(ENETDIR)/memory.o $(ENETDIR)/packet.o $(ENETDIR)/peer.o $(ENETDIR)/protocol.o $(ENETDIR)/unix.o $(ENETDIR)/win32.o
ENETOBJ = $(ENETDIR)/host.o $(ENETDIR)/list.o $(ENETDIR)/callbacks.o $(ENETDIR)/packet.o $(ENETDIR)/peer.o $(ENETDIR)/protocol.o $(ENETDIR)/unix.o $(ENETDIR)/win32.o
IRRNETOBJ = CNetManager.o SPacket.o 
LINKOBJ := $(IRRNETOBJ) $(ENETOBJ) $(ZLIBOBJ)

###############
#Compiler flags
#CXXINCS = -I../../include -I../$(ENETDIR)/include/ -Izlib
CXXINCS = -I../include -I./$(ENETDIR)/include/ -I./NoIrrlicht/ -Izlib
CPPFLAGS = $(CXXINCS)
CXXFLAGS = -Wall -fexpensive-optimizations -O3

ifdef PROFILE
CXXFLAGS += -pg
endif
CFLAGS := -fexpensive-optimizations -O3

#Linux specific options
STATIC_LIB = libirrnet.a
LIB_PATH = ../lib/$(STATIC_LIB)
INSTALL_DIR = /usr/local/lib
staticlib install: SYSTEM = Linux
staticlib: LDFLAGS = --no-export-all-symbols --add-stdcall-alias
####################
all linux: staticlib

$(STATIC_LIB): $(LINKOBJ)
	$(AR) rs $@ $^

# Copies static lib into /lib/Linux
staticlib: $(STATIC_LIB)
	mv $^ $(LIB_PATH)

# Create dependency files for automatic recompilation
%.d:%.cpp
	$(CXX) $(CPPFLAGS) -MM -MF $@ $<

-include $(LINKOBJ:.o=.d)

clean:
	$(RM) $(LINKOBJ) $(SHARED_LIB).$(VERSION) $(STATIC_LIB) $(LINKOBJ:.o=.d)

#withirrlicht:
#CXXINCS += -Iirrlicht/include -DCOMPILE_WITH_IRRLICHT

#all staticlib clean
#	
#.PHONY: all staticlib clean
#
