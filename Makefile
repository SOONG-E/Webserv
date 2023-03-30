NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -g# --save-temps

SRCDIR = src
INCDIR = include
TMPDIR = tmp

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(TMPDIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

.DEFAULT_GOAL = all

-include $(DEPS)

all: $(NAME)
	sudo chown root:root $(NAME)
	sudo chmod +s $(NAME)

$(NAME): $(OBJS)
	@$(CXX) -o $@ $^

$(OBJS): | $(TMPDIR)

$(TMPDIR):
	mkdir -p $@

$(TMPDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -I$(INCDIR) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(TMPDIR)

fclean:
	$(MAKE) -s clean
	$(RM) $(NAME)

re:
	$(MAKE) -s fclean
	$(MAKE) -s all

.PHONY: all clean fclean re
