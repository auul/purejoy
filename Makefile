OUT := el
LIBFLAGS := -lm -lreadline

CC := gcc
CFLAGS := -O1
SRCDIR := src
OBJDIR := build

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

all: $(OUT)

$(OUT): $(OBJFILES)
	$(CC) -o $(OUT) $(LIBFLAGS) $(OBJFILES)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OUT)
	./$(OUT)

clean:
	rm -f $(OBJDIR)/*

.phony:
	all clean
