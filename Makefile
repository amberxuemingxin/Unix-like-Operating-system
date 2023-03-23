
CC = clang

# Replace -O1 with -g for a debug version during development
#
CFLAGS = -Wall -Werror -O1
PENNOS = PennOS
PENNFAT = PennFAT

SRC_DIR_K = ./src/Kernel
SRC_DIR_F = ./src/pennFat
BIN_DIR_K = ./bin/Kernel
BIN_DIR_F = ./bin/pennFat
LOG_DIR = ./log

SRCS_K = $(wildcard $(SRC_DIR_K)/*.c)
SRCS_F = $(wildcard $(SRC_DIR_F)/*.c)
OBJS_K = $(patsubst $(SRC_DIR_K)/%.c, $(BIN_DIR_K)/%.o, $(SRCS_K))
OBJS_F = $(patsubst $(SRC_DIR_F)/%.c, $(BIN_DIR_F)/%.o, $(SRCS_F))
FINAL_OBJS_K = $(BIN_DIR_K)/PennOS.o  $(BIN_DIR_K)/scheduler.o $(BIN_DIR_K)/kernel.o $(BIN_DIR_K)/queue.o $(BIN_DIR_K)/shell.o $(BIN_DIR_K)/prompts.o $(BIN_DIR_K)/logger.o parser.o

.PHONY: all $(PENNOS) $(PENNFAT) clean

all: $(PENNOS) $(PENNFAT)

 $(PENNOS): $(FINAL_OBJS_K)
	$(CC) $(CFLAGS) $(FINAL_OBJS_K) -o $(PENNOS)

$(BIN_DIR_K)/%.o: $(SRC_DIR_K)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR_K)/*.o $(LOG_DIR)/*.txt
