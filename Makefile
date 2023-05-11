NAME = webserv

all: $(NAME)

$(NAME): 
		gcc main.cpp Server.cpp -o $(NAME) -lstdc++ && ./$(NAME)