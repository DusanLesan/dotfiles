LIBS = -lX11 -lcurl -lImlib2 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -ldbus-1
INSTALL_DIR = /usr/local/bin

SOURCES = $(wildcard *.c)
TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

%: %.c
	${CC} -o $@ $< $(LIBS)

install: all
	@mkdir -p $(INSTALL_DIR)
	@for file in $(TARGETS); do \
		mv $$file $(INSTALL_DIR)/; \
	done

clean:
	rm -f $(TARGETS)
