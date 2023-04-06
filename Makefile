NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP# -g --save-temps
INCFLAGS = $(addprefix -I,$(INCS))

SRCDIR = src
INCDIR = include
TMPDIR = tmp

SRCS = $(shell find $(SRCDIR) -type f -name '*.cpp')
INCS = $(shell find $(INCDIR) -type d)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(TMPDIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

.DEFAULT_GOAL = all

-include $(DEPS)

all: $(NAME)
ifeq ($(shell uname),Linux)
	sudo chown root:root $(NAME)
	sudo chmod +s $(NAME)
endif

$(NAME): $(OBJS)
	@$(CXX) -o $@ $^

$(TMPDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(INCFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(TMPDIR)

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) -s fclean
	$(MAKE) -s all

.PHONY: all clean fclean re
