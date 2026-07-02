CC = gcc
BUILD_DIR = build

# Variabili generiche (Iinclude è già qui)
CFLAGS = -Wall -Wextra -fPIC -O2 -Iinclude
LDFLAGS = -shared
LIBS = -lnvidia-ml

# Variabili specifiche per GTK4 e Layer Shell
GTK_CFLAGS = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0)
GTK_LIBS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0)

# Dichiara i target che non sono file reali
.PHONY: all clean

all: $(BUILD_DIR)/libmystats.so $(BUILD_DIR)/test_app $(BUILD_DIR)/example

# Regola per creare la cartella build se non esiste
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- Target della libreria e test ---

$(BUILD_DIR)/libmystats.so: $(BUILD_DIR)/stats.o | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/stats.o: src/stats.c include/stats.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Usa $$ORIGIN per permettere all'eseguibile di trovare la .so ovunque tu lo avvii
$(BUILD_DIR)/test_app: tests/test.c $(BUILD_DIR)/libmystats.so | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -lmystats -Wl,-rpath,'$$ORIGIN' $(LIBS)

# --- Target per l'app GTK ---

$(BUILD_DIR)/example: src/windowInit.c $(BUILD_DIR)/libmystats.so | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ $< -L$(BUILD_DIR) -lmystats -Wl,-rpath,'$$ORIGIN' $(GTK_LIBS) $(LIBS)

# --- Pulizia ---

clean:
	rm -rf $(BUILD_DIR)