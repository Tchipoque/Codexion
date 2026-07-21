CC = cc
CFLAGS = -Wall -Wextra -Werror

NAME = codexion

MANDATORY_SRCS = \
    codexion.c src/dongle_manager.c src/helper.c src/parser.c src/stoppers.c src/engine.c src/monitor.c src/schedulers.c


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
# ./codexion 9 1000 200 100 100 5 100 fifo
