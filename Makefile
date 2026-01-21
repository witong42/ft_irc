# Nom de l'exécutable
NAME = ircserv
CXX = c++
CXXFLAGS = -g -O0 -Wall -Wextra -Werror -std=c++98
LDFLAGS = 

# Répertoires
HEADER_DIR = header
CLASS_DIR = class
TEMPLATE_DIR = template
FUNCTION_DIR = .
BIN_DIR = bin
OBJ_DIR = bin/obj

# Fichiers sources
SRCS = $(FUNCTION_DIR)/main.cpp
CLASS_List = Channel Client Irc Server String 
SRCS2 = $(patsubst %,$(CLASS_DIR)/%.cpp,$(CLASS_List))

# Fichiers objets
OBJS = $(patsubst $(FUNCTION_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
OBJS2 = $(patsubst $(CLASS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS2))
ALL_OBJS = $(OBJS) $(OBJS2)

# Chemins d'inclusion
INCLUDES = -I./$(HEADER_DIR) -I./$(TEMPLATE_DIR) -I./$(CLASS_DIR)

# Règle par défaut
all: $(BIN_DIR)/$(NAME)

# Création de l'exécutable
$(BIN_DIR)/$(NAME): $(ALL_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(ALL_OBJS) -o $@ $(LDFLAGS)
	@echo "✓ Compilation terminée : $@"

# Compilation des fichiers .cpp en .o (fonction main.cpp)
$(OBJ_DIR)/%.o: $(FUNCTION_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	@echo "✓ Compilé : $<"

# Compilation des fichiers .cpp en .o (classes)
$(OBJ_DIR)/%.o: $(CLASS_DIR)/%.cpp | $(OBJ_DIR)
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
	./$(BIN_DIR)/$(NAME) 6667 pwd123

# Affichage des informations de debug
debug:
	make re
	@echo "Sources  : $(SRCS) $(SRCS2)"
	@echo "Objets   : $(ALL_OBJS)"
	@echo "Include  : $(INCLUDES)"
	valgrind --leak-check=full ./$(BIN_DIR)/$(NAME) 6667 pwd123
	make fclean

# Génération des dépendances automatiques
depend: $(SRCS) $(SRCS2)
	$(CXX) $(INCLUDES) -MM $(SRCS) $(SRCS2) | sed 's|^|$(OBJ_DIR)/|' > .depend

-include .depend

# Déclaration des cibles phony
.PHONY: all clean fclean re run debug depend
