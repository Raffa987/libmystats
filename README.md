# TO DO
- rendere stats.c una libreria(DONE)
- finestra
- fare il comando alt + z su hyrpland.conf
- sistemare makefile


gcc
gcc $(pkg-config --cflags gtk4 gtk4-layer-shell-0) -Iinclude -o build/example src/windowInit.c $(pkg-config --libs gtk4 gtk4-layer-shell-0)