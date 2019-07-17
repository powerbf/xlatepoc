DEBUG_FLAGS=-g -O0
CXX_FLAGS=$(DEBUG_FLAGS)

OUTPUT_DIRS=locale/de/LC_MESSAGES
SOURCE_DIR=.
BUILD_DIR=.

SOURCES:=$(wildcard $(SOURCE_DIR)/*.cc)
OBJECTS:=$(patsubst $(SOURCE_DIR)/%.cc,$(BUILD_DIR)/%.o,$(SOURCES))

COMMON_OBJS:=$(filter-out %poc.o,$(OBJECTS))
MAIN_OBJS:=$(filter %poc.o,$(OBJECTS))
EXES:=$(patsubst %.o,%,$(MAIN_OBJS))

LIBS=

LANGS:=$(patsubst %/,%,$(patsubst po/%,%,$(sort $(dir $(wildcard po/*/)))))
MOFILES:=$(foreach lang,$(LANGS),$(patsubst po/$(lang)/%.po,locale/$(lang)/LC_MESSAGES/%.mo,$(wildcard po/$(lang)/*.po)))

.PHONY: all clean translations debug-make

.PRECIOUS: %.o

all: $(OUTPUT_DIRS) $(EXES) translations
	@echo "done."

debug-make:
	@echo "SOURCES=$(SOURCES)"
	@echo "OBJECTS=$(OBJECTS)"
	@echo "COMMON_OBJS=$(COMMON_OBJS)"
	@echo "MAIN_OBJS=$(MAIN_OBJS)"
	@echo "EXES=$(EXES)"

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
