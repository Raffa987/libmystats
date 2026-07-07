CC = gcc
BUILD_DIR = build

# Variabili generiche (Iinclude è già qui)
CFLAGS = -Wall -Wextra -fPIC -O2 -Iinclude
LDFLAGS = -shared
LIBS = -lnvidia-ml

# Dichiara i target che non sono file reali
.PHONY: all clean

# Rimosso $(BUILD_DIR)/example
all: $(BUILD_DIR)/libmystats.so $(BUILD_DIR)/getStats

# Regola per creare la cartella build se non esiste
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- Target della libreria e test ---

$(BUILD_DIR)/libmystats.so: $(BUILD_DIR)/stats.o | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/stats.o: src/stats.c include/stats.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Usa $$ORIGIN per permettere all'eseguibile di trovare la .so ovunque tu lo avvii
# Sostituito src/windowInit.c con src/statsGet.c e aggiunto lo spazio prima di $(BUILD_DIR)
$(BUILD_DIR)/getStats: src/getStats.c $(BUILD_DIR)/libmystats.so | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -lmystats -Wl,-rpath,'$$ORIGIN' $(LIBS)

# --- Pulizia ---
clean:
	rm -rf $(BUILD_DIR)