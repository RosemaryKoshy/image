CC      = gcc
C_STD   = -std=c17
C_W     = -Wall -Wextra -Wpedantic
C_DB    = -ggdb3 -Og -DDEBUG
C_NOSAN = $(C_STD) $(C_W) $(C_DB) -lm
C_FLAGS = $(C_NOSAN) -fsanitize=address,leak,undefined
TARGET  = image
SRC     = $(TARGET).c
RM      = rm -fv

all: $(TARGET)

$(TARGET):
	$(CC) $(C_FLAGS) -o $(TARGET) $(SRC)
	./$(TARGET) 9 5 > $(TARGET).ppm

clean:
	$(RM) $(TARGET) $(TARGET).ppm

grind:
	$(CC) $(C_NOSAN) -o $(TARGET) $(SRC)
	valgrind --track-origins=yes -s --leak-check=full --show-leak-kinds=all ./$(TARGET) 9 5 > $(TARGET).ppm

display:
	display -filter box -resize 25600% $(TARGET).ppm

warn:
	clang $(C_FLAGS) -Weverything -o $(TARGET) $(SRC)

tidy:
	clang-tidy -extra-arg=$(C_STD) $(SRC)

format:
	clang-format -style="{BasedOnStyle: google, IndentWidth: 4}" -i --verbose $(SRC)

lint:
	cpplint --filter=-legal/copyright $(SRC)