
CC = clang

# Replace -O1 with -g for a debug version during development
#
CFLAGS = -Wall -Werror -O1
PROG = PennOS

SRC_DIR = ./src
BIN_DIR = ./bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
#OBJS = $(BIN_DIR)/$(notdir $(SRCS:.c=.o))
OBJS = $(SRCS:.c=.o)

.PHONY : clean

$(PROG) : $(OBJS)
	# $(CC) -o $@ $^ parser.o

clean :
	$(RM) $(OBJS) $(PROG)
