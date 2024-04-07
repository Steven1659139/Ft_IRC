# Nom du programme
NAME = ircserv

# Compilateur et flags
CC = g++
CFLAGS = -Wall -Wextra -Werror -Iinclude -g

# Dossiers
SRCDIR = src
OBJDIR = obj
INCDIR = include

# Trouver les fichiers sources et objets
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Couleurs
GREEN = \033[0;32m
YELLOW = \033[1;33m
NC = \033[0m # No Color

# Règles de compilation
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)
	@echo "${GREEN}Compilation réussie !${NC}"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "${YELLOW}🐥${NC}" $<

# Nettoyage des fichiers objets
clean:
	@rm -rf $(OBJDIR)
	@echo "${YELLOW}Nettoyage des fichiers objets effectué.${NC}"

# Nettoyage complet (programme + fichiers objets)
fclean: clean
	@rm -f $(NAME)
	@echo "${GREEN}Nettoyage complet effectué.${NC}"


IP = 10.11.3.5

ddos-xav: #fork limit 881
	@for i in {1..10} ; do \
		echo "Connecting User$$i" ; \
		(printf "PASS 1234\r\nUSER User$$i\r\nNICK User$$i\r\nJOIN #chan\r\nPRIVMSG Xav :allo\r\n"; sleep 5) | nc $(IP) 4242 & \
    done; wait
	
# Règle pour recompiler
re: fclean all

.PHONY: all clean fclean re
