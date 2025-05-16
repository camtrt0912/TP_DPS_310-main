# Nom de l'exécutable
TARGET = thermalprobe

# Dossiers
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I$(INCLUDE_DIR)
LIBS = -lpaho-mqtt3c -lpthread $(LIB_DIR)/libdps310.a

# Fichiers source
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/fakedps310.c $(SRC_DIR)/mqtt_client.c

# Règles principales
all: $(LIB_DIR)/libdps310.a $(LIB_DIR)/libdps310.so $(BIN_DIR)/$(TARGET)

# Compilation de l'exécutable principal
$(BIN_DIR)/$(TARGET): $(SRC) $(LIB_DIR)/libdps310.a
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compilation de la bibliothèque statique
$(LIB_DIR)/libdps310.a: $(LIB_DIR)/libdps310.c $(INCLUDE_DIR)/libdps310.h
	$(CC) -c -fPIC -o $(LIB_DIR)/libdps310.o $(LIB_DIR)/libdps310.c
	ar rcs $@ $(LIB_DIR)/libdps310.o

# Compilation de la bibliothèque dynamique
$(LIB_DIR)/libdps310.so: $(LIB_DIR)/libdps310.c $(INCLUDE_DIR)/libdps310.h
	$(CC) -shared -o $@ $(LIB_DIR)/libdps310.c

# Nettoyage des fichiers générés
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR)/*.o $(LIB_DIR)/*.a $(LIB_DIR)/*.so

# Exécution du programme avec des arguments par défaut
run: $(BIN_DIR)/$(TARGET)
	./$(BIN_DIR)/$(TARGET) sensor1 5