LIBTOOL ?= libtool

.PHONY: all clean

all: build/libmemtrack.a

clean:
	$(RM) -rf build

build/libmemtrack.a: build/memtrack.o
	$(LIBTOOL) -static -o $@ $^

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<
