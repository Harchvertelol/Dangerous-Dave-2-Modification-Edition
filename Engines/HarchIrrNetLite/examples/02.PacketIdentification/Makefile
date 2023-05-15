CXX = g++
Target := 02.PacketIdentification
TargetPath := ../bin/
Sources := main.cpp

CXXINCS = -I../../include/

#ifndef NDEBUG
CXXFLAGS_D = -g -Wall
#else
CXXFLAGS_R = -fexpensive-optimizations -O3 
#endif

LDIR =../../lib/
LIBS = -L$(LDIR) -lirrnet

debug: main.cpp
	$(CXX) $(CXXFLAGS_D) $(CXXINCS) -o $(Target) $(Sources) $(LIBS)
	mv $(Target) $(TargetPath)

release: main.cpp
	$(CXX) $(CXXFLAGS_R) $(CXXINCS) -o $(Target) $(Sources) $(LIBS)
	mv $(Target) $(TargetPath)

clean:
	rm -f *.o $(TargetPath)$(Target)

.PHONY: clean debug release
