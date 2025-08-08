# ls -d src/* | sort | xargs -n1 basename

NAME	:=	webserv

SRC		:=	main.cpp \
			Utils/DJB2.cpp \
			Utils/setNonBlocking.cpp \
			Logger/Logger.cpp \
			Utils/DefaultResponses.cpp \
			Utils/IsDirectory.cpp \
			Utils/getAllFilesFromDirectory.cpp \
			Config/Config.cpp \
			Config/CreateConfig.cpp \
			Config/Route.cpp \
			Server/Server.cpp \
			Config/CGI.cpp \
			Server/Client.cpp \
			Server/CreateServerPool.cpp \
			Request/Request_GS.cpp \
			Request/chunked.cpp \
			Request/Request.cpp \
			Request/RequestParser.cpp \
			Request/RequestUtils.cpp \
			Request/Response/Response.cpp \
			Request/Response/Response_GS.cpp \
			Request/Response/Response_error.cpp \
			Request/Response/Response_utils.cpp


SRC_DIR	:=	src
BUILD	:=	.build

SRC		:=	$(addprefix $(SRC_DIR)/, $(SRC))
OBJ 	:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD)/%.o, $(SRC))
DEP		:=	$(OBJ:.o=.d)

CC		:=	g++
CFLAGS	:=	-Wall -Werror -Wextra -std=c++98 -g

all: create_dir $(NAME)

create_dir: | $(BUILD)

print:
	@echo $(OBJ)

$(BUILD):
	@mkdir -p $(BUILD)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(BUILD)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -Iinc -c $< -o $@
	@printf "\033[1;32%sm\tCompiled: $(<F)\033[0m\n";

clean:
	@if [ -d $(BUILD) ]; then $(RM) -rf $(BUILD) && printf "\033[1;31m\tDeleted: $(NAME) $(BUILD)\033[0m\n"; fi

fclean:
	@make --no-print-directory clean
	@if [ -f $(NAME) ]; then $(RM) -rf $(NAME) && printf "\033[1;31m\tDeleted: $(NAME)\033[0m\n"; fi

re:
	@make --no-print-directory fclean
	@make --no-print-directory all

-include $(DEP)

.PHONY: all clean fclean re create_dir
