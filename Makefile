CC = clang

# Replace -O1 with -g for a debug version during development
#
CFLAGS = -Wall -Werror -g
PENNOS = PennOS
PENNFAT = PennFAT

SRC_DIR_K = ./src/Kernel
SRC_DIR_F = ./src/pennFat
BIN_DIR_K = ./bin/Kernel
BIN_DIR_F = ./bin/pennFat
SUB_DIR_K := $(wildcard $(SRC_DIR_K)/*/.)
LOG_DIR = ./log

SRCS_K = $(wildcard $(SRC_DIR_K)/*.c) $(wildcard $(SRCDIR)/built-ins/*.c)
SRCS_F = $(wildcard $(SRC_DIR_F)/*.c)
SRCS_F := $(filter-out $(SRC_DIR_F)/pennFAT.c, $(SRCS_F))
OBJS_K = $(patsubst $(SRC_DIR_K)/%.c,$(BIN_DIR_K)/%.o,$(SRCS_K))
OBJS_K += $(patsubst $(SRC_DIR_F)/%.c, $(BIN_DIR_F)/%.o, $(SRCS_F))
OBJS_F = $(patsubst $(SRC_DIR_F)/%.c, $(BIN_DIR_F)/%.o, $(SRCS_F))
OBJS_F := $(filter-out $(SRC_DIR_F)/pennFAT.o, $(OBJS_F))

.PHONY: all $(PENNOS) $(PENNFAT) clean

all: $(PENNOS) $(PENNFAT)

 $(PENNOS): $(OBJS_K)
	$(CC) $(CFLAGS) $(OBJS_K) parser.o -o $(PENNOS)

$(BIN_DIR_K)/%.o: $(SRC_DIR_K)/%.c | $(BIN_DIR_K)
	$(CC) $(CFLAGS) -c -o $@ $<

# $(PENNFAT) : $(FINAL_OBJS_F)
# 	$(CC) $(CFLAGS) $(FINAL_OBJS_F) -o $(PENNFAT)

$(BIN_DIR_F)/%.o: $(SRC_DIR_F)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN_DIR_K)/*.o $(LOG_DIR)/*.txt