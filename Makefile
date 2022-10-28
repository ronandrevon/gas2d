.SUFFIXES:                    # delete the default suffixes
.SUFFIXES: .cpp .hpp .h .o    # define new suffix list

# compilation options
PG=#-pg
DEBUG=-g
CXX      = g++
DEFINES  = #-DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CXXFLAGS = -Os -frtti -fexceptions -Wall -W $(DEFINES) $(DEBUG)  $(PG)
BIN=gas2d$(DEBUG)$(PG)
# linking options
LINK     = g++
LPATH    = -L/usr/lib
LIBS     = #-lGLU -lGL -lpthread \
           #-lQtGui -lQtCore -lQtOpenGL
LFLAGS   = -Wl,-O1
INCLUDE  = -I src

SRCDIR   = src
OBJDIR   = bin/obj
OBJECTS  = $(addprefix $(OBJDIR)/, \
           gas_simulator.o histogram.o \
           vector_algebra.o)



all : $(OBJECTS)
	$(LINK) $(LPATH) $(LFLAGS) $(INCLUDE) $(CXXFLAGS) -o bin/$(BIN) $(SRCDIR)/main.cpp $(OBJECTS) $(LIBS)

wrappy : $(OBJECTS)
	$(LINK) $(LPATH) $(LFLAGS) $(INCLUDE) $(CXXFLAGS) -o bin/wrap $(SRCDIR)/main_wrap.cpp $(OBJECTS) $(LIBS)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCL) -c  $< -o $@

.PHONY : clean
clean :
	# rm -rf bin/gas2d
	rm -rf $(OBJECTS)

# all:
# 	g++ main.cpp
