# COLORS
GREEN		= \033[0;32m
RED 		= \033[0;31m
ORANGE		= \033[0;33m
CYAN		= \033[0;36m
RESET		= \033[0m
UP 			= \033[A
CUT 		= \033[K

# PROGRAM
NAME		= webserv

# FOLDERS & FILES
SRC_DIR		= ./srcs/
SRC_FILES	=	main.cpp \
				HttpReqParsing.cpp \
				HttpResponse.cpp \
				ReqHandler.cpp \
				utils.cpp \
				Server.cpp \
				confparser.cpp


OBJ_DIR		= ./objs/
OBJ_FILES	= ${SRC_FILES:.cpp=.o}

SRC			= ${addprefix ${SRC_DIR}, ${SRC_FILES}}
OBJ			= ${addprefix ${OBJ_DIR}, ${OBJ_FILES}}

# FLAGS
CC			= c++
CFLAGS		= #-Wall -Wextra -Werror
CPP_FLAGS	= -std=c++98 -pedantic

# COMMANDS
MKDIR		= mkdir -p
RM			= rm -rf

all: ${NAME}

# Compile normal executable
${NAME}: ${OBJ_DIR} ${OBJ}
	@${CC} ${CFLAGS} ${CPP_FLAGS} -o ${NAME} ${OBJ}
	@echo "[$(ORANGE)$(NAME)$(RESET)] Creation *.o files & compilation : $(GREEN)OK$(RESET)"
	@echo "[\033[1;32msuccess$(RESET)]: $(NAME) is ready"

# Compile normals objects
${OBJ_DIR}%.o: ${SRC_DIR}%.cpp
	@echo "[$(CYAN)Compiling$(RESET)]: $<$(UP)$(CUT)"
	@${CC} ${CFLAGS} ${CPP_FLAGS} -o $@ -c $<

${OBJ_DIR}:
	@${MKDIR} ${OBJ_DIR}

run: all
	./${NAME}

clean:
	@${RM} ${OBJ}
	@${RM} ${OBJ_DIR}
	@echo "[$(ORANGE)$(NAME)$(RESET)] clean *.o files : $(GREEN)OK$(RESET)"

fclean:	clean
	@${RM} ${NAME}
	@echo "[$(ORANGE)$(NAME)$(RESET)] fclean: $(GREEN)OK$(RESET)"
	@echo "[\033[1;32msuccess$(RESET)]: $(NAME) has been cleaned up"

re: fclean all

.PHONY: all clean fclean re
