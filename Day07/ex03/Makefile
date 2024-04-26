CC = avr-gcc
OBJCOPY = avr-objcopy
SRC_PATH = 
OBJ_PATH = obj/
SRC_NAME = main.c uart.c
F_CPU = 16000000UL
UART_BAUDRATE = 115200
CFLAGS = -DF_CPU=$(F_CPU) -mmcu=atmega328p -Os -DUART_BAUDRATE=$(UART_BAUDRATE)
BIN_NAME = chips.bin
HEX_NAME = chips.hex
OBJ_NAME = $(SRC_NAME:.c=.o)
PORT = /dev/ttyUSB0
INCLUDE = -Iincludes
HEX = $(OBJ_PATH)$(HEX_NAME)
BIN = $(OBJ_PATH)$(BIN_NAME)
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))

all: hex flash

hex: $(HEX)

$(BIN): $(OBJ)
	$(CC) $(INCLUDE) $^ -o $(BIN) $(CFLAGS)

$(HEX): $(BIN)
	$(OBJCOPY) $(BIN) -O ihex $@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	mkdir -p $(OBJ_PATH)
	$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

flash: hex
	avrdude -c arduino -p M328P -P $(PORT) -b $(UART_BAUDRATE) -U flash:w:$(HEX)

clean:
	rm -rf $(OBJ_PATH)

re: clean all

.PHONY: all hex flash clean re
