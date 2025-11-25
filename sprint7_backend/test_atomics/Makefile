
NAME = executable

#SRCDIR = ./srcs/
#HEADDIR = ./includes/

SOURCES_FILES = $(wildcard *.cpp)

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O2 -lm -fno-stack-limit -std=c++2a -Wall -Wextra -DMYBUILD -g
#g++ -O2 -lm -fno-stack-limit -std=c++20 -x c++ source.cpp -o executable -Wall -Wextra -Werror
SOURCES = ${addprefix ${SRCDIR}, ${SOURCES_FILES}}

OBJECTS = ${SOURCES:.cpp=.o}

RM = rm -f

#full: ${NAME} test_target fclean
full: ${NAME} test_target fclean

all: ${NAME}

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< && touch _testok;\
	if [ -f "_testok" ]; then \
		rm -f _testok ;\
	else \
		rm -f _testok ;\
		echo "Compilation Failure :-(" ;\
		exit 1 ;\
	fi

${NAME}: ${OBJECTS}
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS) && touch _testok;\
	if [ -f "_testok" ]; then \
		rm -f _testok ;\
	else \
		rm -f _testok ;\
		echo "Linking Failure :-(" ;\
		exit 1 ;\
	fi

clean:
	@rm -fr ${OBJECTS}

fclean: clean
	@rm -fr ${NAME} 

re: fclean all

test_target:
	@./../test.sh



.PHONY:	all clean fclean re all
# Rule to link object files into the target executable

