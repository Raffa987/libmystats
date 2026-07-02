#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "stats.h"



int main() {

    struct mystats *stats = malloc(sizeof(struct mystats));
    statsInit(stats);

    statsUpdate(stats);
        printf(
        "CPU: %d°C | "
        "GPU: %u°C | "
        "GPU: %d GB | "
        "RAM: %.2f/%.2f GB",
        stats->CPU_temp,
        stats->GPU_temp,
        stats->GPU_mem_used,
        stats->RAM_used,
        stats->RAM_tot
    );

    statsCleanup();
}


