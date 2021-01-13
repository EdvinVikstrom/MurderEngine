NAME = libme-2020
BUILD = build
OUTNAME = libme.a
CC = g++

CFLAGS = -O3 -std=c++20
LIBS = 
INCS = --include=type.hpp
LPATHS = 
IPATHS = -I./include/lme
DEFS = 

PKG_CONFIG_PATH = ./include/lme:
CPKG = 
LPKG = 

EXTERN = 

COPTS = $(CFLAGS) \
	$(IPATHS) \
	$(INCS) \
	$(DEFS) \
	$(CPKG)
LOPTS = $(LPATHS) $(LIBS) $(LPKG)

SOURCES = ./src/file.cpp \
	./src/buffer.cpp \
	./src/string.cpp \
	./src/memory.cpp \
	./src/reference.cpp \
	./src/time.cpp

OBJECTS = $(SOURCES:%=$(BUILD)/%.o)
DEPENDS = $(OBJECTS:%.o=%.d)

.PHONY: $(NAME)
$(NAME): $(EXTERN) $(OUTNAME)

$(OUTNAME): $(OBJECTS)
	@ar rcs $@ $^

-include $(DEPENDS)

$(BUILD)/%.o: %
	@echo "[32m==> compiling source [33m[$<][0m"
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@ $< $(COPTS) -MMD

.PHONY: clean
clean:
	rm -f $(OUTNAME) $(OBJECTS) $(DEPENDS)
