DEBUG_FLAGS=-g -O0
CXX_FLAGS=$(DEBUG_FLAGS)

EXE=xlatepoc
OBJS=unicode.o stringutil.o xlate.o translate.o main.o 
MOFILES=locale/de/LC_MESSAGES/messages.mo locale/de/LC_MESSAGES/entities.mo locale/de/LC_MESSAGES/context-map.mo
LIBS=

.PHONY: all clean

all: $(EXE) $(MOFILES)
	

$(EXE): $(OBJS)
	g++ -o $@ $(DEBUG_FLAGS) $^ $(LIBS)

%.o: %.cc
	g++ -c $(CXX_FLAGS) $^

locale/de/LC_MESSAGES/%.mo: po/de/%.po
	msgfmt --output-file=$@ $<

clean:
	rm -rf $(EXE) *.o locale/*/*/*.mo
