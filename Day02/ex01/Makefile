NAME=main.hex
MCU=atmega328p
F_CPU=16000000
BAUD=115200
SRC=main.c
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJS=$(SRC:.c=.bin)
CFLAGS=-std=c99 -O1 -g -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I.
PORT=/dev/ttyUSB0

all: hex flash
	
$(NAME): $(OBJS)
	$(OBJCOPY) -O ihex $(OBJS) $(NAME)

%.bin: %.c
	$(CC) $(CFLAGS) -o $(OBJS) $(SRC)

hex: $(NAME)

flash:
	avrdude -p $(MCU) -c arduino -U flash:w:$(OBJS) -P $(PORT)

clean:
	rm $(OBJS)
	rm $(NAME)

.PHONY: hex flash clean $(NAME)
