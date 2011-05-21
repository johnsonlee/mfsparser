#
# Copyright (c) 2011, www.zergle.org. All Rights Reserved.
#

OBJDIR   := .obj
SRCDIR   := src
TARGET   := mfsparser

DEFINES  += -D__DEBUG__
CFLAGS   := -ggdb -O0 -g3 -Wall -I$(PWD)/include
LDFLAGS  := 

SOURCES  := $(SRCDIR)/bytebuffer.c \
            $(SRCDIR)/crc.c        \
            $(SRCDIR)/mfsparser.c  \
            $(SRCDIR)/stack.c      \
            $(SRCDIR)/mfstypes.c

OBJECTS  := $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SOURCES)))

all: $(TARGET)

run: clean all
	./$(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(DEFINES) $(CFLAGS) -o $@ -c $<

clean:
	@rm -rvf $(OBJDIR)
	@rm -vf $(TARGET)
	
