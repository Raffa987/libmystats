#ifndef MYSTATS_H
#define MYSTATS_H

#include <nvml.h>

struct mystats {
    nvmlDevice_t device;
    int GPU_mem_used;
    unsigned  GPU_temp;

    int CPU_temp;
    char CPU_path[512];

    float RAM_tot;
    float RAM_used;    
};

/*
* populates paths and hooks NVML.
* @param stats Pointer to the struct to be inizialized
* @return -1 if something goes wrong, otherwise 0
*/
int statsInit(struct mystats *stats);

/*
* updates the value in the struct.
* @param pointer to the structs inizialize with statsInit(stats)
*/
void statsUpdate(struct mystats *stats);

/*
* frees resources and turns off NVML.
* to be called before the host program ends
* @return 0 if it succeeds
*/
int statsCleanup(void);

#endif