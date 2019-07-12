DEBUG_FLAGS=-g -O0
CXX_FLAGS=$(DEBUG_FLAGS)

OUTPUT_DIRS=locale/de/LC_MESSAGES
SOURCE_DIR=.
BUILD_DIR=.

EXE=$(BUILD_DIR)/xlatepoc

SOURCES:=$(wildcard $(SOURCE_DIR)/*.cc)
OBJECTS:=$(patsubst $(SOURCE_DIR)/%.cc,$(BUILD_DIR)/%.o,$(SOURCES))

LIBS=

LANGS:=$(patsubst %/,%,$(patsubst po/%,%,$(sort $(dir $(wildcard po/*/)))))
MOFILES:=$(foreach lang,$(LANGS),$(patsubst po/$(lang)/%.po,locale/$(lang)/LC_MESSAGES/%.mo,$(wildcard po/$(lang)/*.po)))

.PHONY: all clean translations

all: $(OUTPUT_DIRS) $(EXE) translations
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

$(EXE): $(OBJECTS)
	g++ -o $@ $(DEBUG_FLAGS) $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
	g++ -c -o $@ $(CXX_FLAGS) $^

clean:
	rm -rf $(EXE) *.o locale
