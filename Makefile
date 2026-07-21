CC = cc
CFLAGS = -Wall -Wextra -Werror

NAME = codexion

MANDATORY_SRCS = \
    codexion.c src/dongle_manager.c src/initializer.c src/scheduler.c \
	src/utils.c src/engine.c src/monitor.c src/stoppers.c src/validation.c

OBJS = $(MANDATORY_SRCS:.c=.o)

# Terminal colors
C_RESET = \033[0m
C_BLUE = \033[1;34m
C_GREEN = \033[1;32m
C_YELLOW = \033[1;33m
C_RED = \033[1;31m

define msg_info
	@printf "$(C_BLUE)[INFO]$(C_RESET) %s\n" "$(1)"
endef

define msg_ok
	@printf "$(C_GREEN)[OK]$(C_RESET) %s\n" "$(1)"
endef

define msg_warn
	@printf "$(C_YELLOW)[WARN]$(C_RESET) %s\n" "$(1)"
endef

define msg_error
	@printf "$(C_RED)[ERROR]$(C_RESET) %s\n" "$(1)"
endef

define msg_step
	@printf "$(C_BLUE)>>$(C_RESET) %s\n" "$(1)"
endef

all: $(NAME)

$(NAME): $(OBJS)
	$(call msg_step,Creating executable file...)
	@$(CC) $(CFLAGS) $(MANDATORY_SRCS) -o $(NAME)
	$(call msg_ok,Executable file created successfully.)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(call msg_warn,Removing object files...)
	@rm -f $(OBJS)
	$(call msg_ok,Cleanup completed.)

fclean: clean
	$(call msg_warn,Removing executable file...)
	@rm -f $(NAME)
	$(call msg_ok,Executable file removed.)


re: fclean all

.PHONY: all clean fclean re
# ./codexion 9 1000 200 100 100 5 100 fifo
