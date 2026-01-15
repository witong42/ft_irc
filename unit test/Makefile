NAME		=	test_unit

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -I../header

SRCS		=	main.cpp \
				client_test.cpp \
				channel_test.cpp \
				mode_test.cpp \
				../class/Channel.cpp \
				../class/Client.cpp

OBJS		=	$(SRCS:.cpp=.o)

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
