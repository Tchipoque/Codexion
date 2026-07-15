CC = cc
CFLAGS = -Wall -Wextra -Werror

NAME = codexion

MANDATORY_SRCS = \
    codexion.c src/engine.c  src/helper.c  src/stoppers.c  src/parser.c src/monitor.c

OBJS = $(MANDATORY_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(MANDATORY_SRCS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
# ./codexion 3 800 200 200 200 3 100 fifo
