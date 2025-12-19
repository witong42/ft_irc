NAME		=	test_unit

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -I../header

SRCS		=	channel_client.cpp \
				../class/channel.cpp \
				../class/client.cpp

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
