NAME=main.hex
MCU=atmega328p
F_CPU=16000000
UART_BAUDRATE=115200
SRC=main.c
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJS=$(SRC:.c=.bin)
CFLAGS=-std=c99 -O1 -g -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(UART_BAUDRATE) -I.
PORT=/dev/ttyUSB0

FLASH := .flash

all: $(FLASH)
	
$(NAME): $(OBJS)
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.c
	$(CC) $(CFLAGS) -o $(OBJS) $(SRC)

hex: $(NAME)

$(FLASH): $(NAME)
	avrdude -p $(MCU) -c arduino -U flash:w:$(OBJS) -P $(PORT)
	@touch $@

screen:
	gnome-terminal -- screen $(PORT) $(UART_BAUDRATE)

squit:
	screen -X quit &>/dev/null || true

clean:
	rm -f $(NAME) $(FLASH) *.bin

.PHONY: hex flash clean
