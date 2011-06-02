#
# Copyright (c) 2011, www.zergle.org. All Rights Reserved.
#

OBJDIR   := .obj
SRCDIR   := src
TARGET   := mfsparser

DEFINES  := 
CFLAGS   := -O0 -g3 -Wall -I$(PWD)/include
LDFLAGS  := 

SOURCES  := $(SRCDIR)/bytestream.c \
            $(SRCDIR)/crc.c        \
            $(SRCDIR)/mfsparser.c  \
            $(SRCDIR)/stack.c      \
            $(SRCDIR)/main.c       \
            $(SRCDIR)/mfstypes.c

OBJECTS  := $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SOURCES)))

all: mfsparser libmfs.so

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

libmfs.so: $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CC) $(LDFLAGS) -shared $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(DEFINES) $(CFLAGS) -o $@ -c $<

clean:
	@rm -rvf $(OBJDIR)
	@rm -vf $(TARGET)
	
