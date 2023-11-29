CC = gcc
LIBS = -lX11
INSTALL_DIR = /usr/local/bin

SOURCES = $(wildcard *.c)
TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

%: %.c
	$(CC) -o $@ $< $(LIBS)

install: all
	@echo "Moving files to $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR)
	@for file in $(TARGETS); do \
		mv $$file $(INSTALL_DIR)/; \
	done

clean:
	rm -f $(TARGETS)

