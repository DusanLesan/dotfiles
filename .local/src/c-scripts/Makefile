LIBS_CURL = -lcurl
LIBS_X11 = -lX11
LIBS_IMLIB2 = -lImlib2
LIBS_DBUS = -ldbus-1 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include
LIBS_SQLITE = -lsqlite3

INSTALL_DIR = /usr/local/bin

SOURCES = $(wildcard *.c)
TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

solid-bg: solid-bg.c
	$(CC) -o $@ $< $(LIBS_IMLIB2) $(LIBS_X11)

file-handler: file-handler.c
	$(CC) -o $@ $< $(LIBS_DBUS)

roku-remote: roku-remote.c
	$(CC) -o $@ $< $(LIBS_CURL) $(LIBS_X11)

image-tag-handler: image-tag-handler.c
	$(CC) -o $@ $< $(LIBS_SQLITE)

%: %.c
	$(CC) -o $@ $<

install: all
	@mkdir -p $(INSTALL_DIR)
	@for file in $(TARGETS); do \
		mv $$file $(INSTALL_DIR)/; \
	done

clean:
	rm -f $(TARGETS)
