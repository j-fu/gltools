######################################################
#
# You need to change only these machine specific data
#

GL_LIB=GL

######
#uncomment or correct these
LIBRARY_PATH=-L/usr/X11/lib
#INCLUDE_PATH=-I/usr/local/include
#GL_LIB=MesaGL

CC=cc
CCOPT=-O2


######################################################
#
# No change should be necessary from here on.
#

LIB=libgltools.a
CFLAGS= $(CCOPT) $(INCLUDE_PATH)
LOADLIBES=$(LIB) -lX11 -lXext -l$(GL_LIB) -lm
LDFLAGS=$(LIBRARY_PATH) $(LOADLIBES)

all: bin

OBJ=\
$(LIB)(glmesh.o)\
$(LIB)(glm-color.o)\
$(LIB)(glm-2d.o)\
$(LIB)(glm-3d.o)\
$(LIB)(glrnd.o)\
$(LIB)(gleps.o)\
$(LIB)(glprimitives.o)\
$(LIB)(glhls.o)\
$(LIB)(gltensview.o)\
$(LIB)(glwin.o)

BIN=glview glrexample glwexample-appctrl glwexample-evctrl

lib: $(OBJ)
	-if test -f *.o ; then ar cvr $(LIB) *.o ; fi
	-rm -f *.o
       
bin: lib $(BIN)

clean:
	rm -f *.o $(LIB) $(BIN)





