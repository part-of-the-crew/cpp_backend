
NAME = executable

#SRCDIR = ./srcs/
#HEADDIR = ./includes/

SOURCES_FILES = main.cpp

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O2 -lm -fno-stack-limit -std=c++2a -Wall -Wextra -g  -fsanitize=address,leak
#g++ -O2 -lm -fno-stack-limit -std=c++20 -x c++ source.cpp -o executable -Wall -Wextra -Werror
SOURCES = ${addprefix ${SRCDIR}, ${SOURCES_FILES}}

OBJECTS = ${SOURCES:.cpp=.o}

RM = rm -f

all: ${NAME} test_target fclean

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

${NAME}: ${OBJECTS}
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS)

clean:
	@rm -fr ${OBJECTS}

fclean: clean
	@rm -fr ${NAME} 

re: fclean all

test_target:
	@./../test.sh



.PHONY:	all clean fclean re
# Rule to link object files into the target executable

