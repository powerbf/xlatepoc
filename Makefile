DEBUG_FLAGS=-g -O0
CXX_FLAGS=$(DEBUG_FLAGS)

OUTPUT_DIRS=locale/de/LC_MESSAGES
SOURCE_DIR=.
BUILD_DIR=.

EXES:=xlatepoc monsterpoc
EXES:=$(patsubst %,$(BUILD_DIR)/%,$(EXES))

COMMON_OBJS:=translate.o xlate.o stringutil.o unicode.o
COMMON_OBJS:=$(patsubst %.o,$(BUILD_DIR)/%.o,$(COMMON_OBJS))

LIBS=

LANGS:=$(patsubst %/,%,$(patsubst po/%,%,$(sort $(dir $(wildcard po/*/)))))
MOFILES:=$(foreach lang,$(LANGS),$(patsubst po/$(lang)/%.po,locale/$(lang)/LC_MESSAGES/%.mo,$(wildcard po/$(lang)/*.po)))

.PHONY: all clean translations

.PRECIOUS: %.o

all: $(OUTPUT_DIRS) $(EXES) translations
	@echo "done."

define GEN_MO_RULE
locale/$(lang)/LC_MESSAGES/%.mo: po/$(lang)/%.po
	msgfmt --output-file=$$@ $$<
endef

$(foreach lang,$(LANGS),$(eval $(GEN_MO_RULE)))

translations: $(MOFILES)
	@echo "languages: $(LANGS)"

$(OUTPUT_DIRS):
	mkdir -p $(OUTPUT_DIRS)

$(BUILD_DIR)/%poc: $(BUILD_DIR)/%poc.o $(COMMON_OBJS)
	g++ -o $@ $(DEBUG_FLAGS) $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
	g++ -c -o $@ $(CXX_FLAGS) $^

clean:
	rm -rf $(EXES) *.o locale
