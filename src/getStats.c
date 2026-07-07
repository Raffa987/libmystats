#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "stats.h"



int main() {

    struct mystats *stats = malloc(sizeof(struct mystats));
    statsInit(stats);
    const char *tmp_file = "/dev/shm/stats_output.tmp";
    const char *final_file = "/dev/shm/stats_output.txt";

    while(1) {
        FILE *FP = fopen(tmp_file, "w");
        statsUpdate(stats);
        fprintf(FP,
            "CPU: %d°C | "
            "GPU: %u°C | "
            "GPU: %d%% | "
            "RAM: %.2f/%.2fGB",
            stats->CPU_temp,
            stats->GPU_temp,
            stats->GPU_mem_used,    
            stats->RAM_used,
            stats->RAM_tot
        );
        fclose(FP);

        rename(tmp_file, final_file);
        sleep(1);
        
    }

    statsCleanup();
    free(stats);
    return 0;
}


