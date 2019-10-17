TARGET = energytrace
SRC = $(TARGET).c

MSP430FLASHER ?= /opt/MSP430Flasher_1.3.15

CC ?= gcc-8
CFLAGS ?= -O2 -I$(MSP430FLASHER)/Source/Inc -L$(MSP430FLASHER) -lmsp430

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run: all
	./energytrace 5

install: all
	install -t /usr/local/bin $(TARGET)


