OBJDIR = $(GARFIELD_HOME)/Object
SRCDIR = $(GARFIELD_HOME)/Source
INCDIR = $(GARFIELD_HOME)/Include
HEEDDIR = $(GARFIELD_HOME)/Heed
LIBDIR = $(GARFIELD_HOME)/Library
ROOT_BASE = ../root

SRC_STANDARD       = tools.cpp Particles.cpp
SRC_TOOLS          = $(wildcard Tools/*.cpp)
SRC_SSH            = $(wildcard SSH/*.cpp)
SRC_SIGNALS        = $(wildcard Signals/*.cpp)
SRC_TRTELECTRONICS = $(wildcard TRTElectronics/*.cpp)
SRC_TRT            = $(wildcard TRT/*.cpp)
SRC_JOBS           = $(wildcard Jobs/*.cpp)

OBJ_STANDARD       = $(SRC_STANDARD:%.cpp=%.o)
OBJ_TOOLS          = $(SRC_TOOLS:%.cpp=%.o)
OBJ_SSH            = $(SRC_SSH:%.cpp=%.o)
OBJ_SIGNALS        = $(SRC_SIGNALS:%.cpp=%.o)
OBJ_TRTELECTRONICS = $(SRC_TRTELECTRONICS:%.cpp=%.o)
OBJ_TRT            = $(SRC_TRT:%.cpp=%.o)
OBJ_JOBS           = $(SRC_JOBS:%.cpp=%.o)

OBJ_BASIC = $(OBJ_TOOLS) $(OBJ_SIGNALS) $(OBJ_TRTELECTRONICS) $(OBJ_STANDARD) $(OBJ_JOBS)

# Contains all programs which require only very basic libraries excluding Garfield for example
PRG_BASIC = shapermc blrmc lowintmc highintmc lowdiscmc highdiscmc test l ghspice createsignalpulses h cs
PRG_TRT = field tail gtail createiontable ptail ctail g
PRG_TRTSSH =

CXX = g++

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-long-long -pedantic \
	-O3 -fno-common -c \
	-I$(INCDIR) -I$(HEEDDIR) -I. \
	-I$(BOOST_INCDIR) \
	-I. \
	-I../libssh/include \
        -I../root/include \
	-D USEROOT \
	$(GSL_INC) -c

# Debug flags
LDFLAGS = `${ROOT_BASE}/bin/root-config --glibs` 
LDFLAGS += -lGeom -lm -Wall
LDFLAGS += -L$(LIBDIR)
LDGARFFLAGS = $(LDFLAGS) -lGarfield -lgfortran
LDGARFSSHFLAGS = $(LDFLAGS) -lGarfield -L../libssh/build/src -lssh

all: $(PRG_BASIC)

.PHONY: clean

$(PRG_TRTSSH):%:%.cpp $(OBJ_BASIC) $(OBJ_TRT) $(OBJ_SSH)
	$(CXX) $(CFLAGS) $@.cpp
	$(CXX) -o $@ $@.o $(OBJ_BASIC) $(OBJ_TRT) $(OBJ_SSH) $(LDGARFSSHFLAGS)

$(PRG_TRT):%:%.cpp $(OBJ_BASIC) $(OBJ_TRT)
	$(CXX) $(CFLAGS) $@.cpp
	$(CXX) -o $@ $@.o $(OBJ_BASIC) $(OBJ_TRT) $(LDGARFFLAGS)

$(PRG_BASIC):%:%.cpp $(OBJ_BASIC)
	$(CXX) $(CFLAGS) $@.cpp
	$(CXX) -o $@ $@.o $(OBJ_BASIC) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) $(OBJ_BASIC) $(OBJ_TRT)
