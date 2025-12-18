# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/27 15:19:44 by jegirard          #+#    #+#              #
#    Updated: 2025/12/18 11:46:34 by jegirard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Nom de l'exécutable
NAME = ircserv
CXX = c++
CXXFLAGS = -g -o0 -Wall -Wextra -Werror -std=c++98
LDFLAGS = 

# Répertoires
HEADER_DIR = header
CLASS_DIR = class
TEMPLATE_DIR = template
FUNCTION_DIR = .
BIN_DIR = bin
OBJ_DIR = bin/obj


# Fichiers sources
# SRCS = $(wildcard $(FUNCTION_DIR)/*.cpp)
SRCS = $(FUNCTION_DIR)/main.cpp
OBJS = $(patsubst $(FUNCTION_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Chemins d'inclusion
INCLUDES = -I ./$(HEADER_DIR) -I ./$(TEMPLATE_DIR) -I ./$(CLASS_DIR)

# Règle par défaut
all: $(BIN_DIR)/$(NAME)

# Création de l'exécutable
$(BIN_DIR)/$(NAME): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✓ Compilation terminée : $@"

# Compilation des fichiers .cpp en .o
$(OBJ_DIR)/%.o: $(FUNCTION_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	@echo "✓ Compilé : $<"

# Création des répertoires nécessaires

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage des fichiers objets
clean:
	rm -rf $(OBJ_DIR)
	@echo "✓ Fichiers objets supprimés"

# Nettoyage complet
fclean: clean
	rm -rf $(BIN_DIR)
	@echo "✓ Nettoyage complet effectué"

# Recompilation complète
re: fclean all

# Exécution du programme
run: all
	./$(BIN_DIR)/$(NAME)

# Affichage des informations de debug
debug:
	make re
	@echo "Sources  : $(SRCS)"
	@echo "Objets   : $(OBJS)"
	@echo "Include  : $(INCLUDES)"
#	@echo $(OBJ_DIR)/%.o: $(FUNCTION_DIR)/%.cpp | $(OBJ_DIR)
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	valgrind  --leak-check=full ./$(BIN_DIR)/$(NAME)
	make fclean

# Génération des dépendances automatiques
depend: $(SRCS)
	$(CXX) $(INCLUDES) -MM $(SRCS) | sed 's|^|$(OBJ_DIR)/|' > .depend

-include .depend

# Déclaration des cibles phony
.PHONY: all clean fclean re run debug depend
