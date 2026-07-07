#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>
#include <dirent.h>
#include <string.h>
#include <sys/sysinfo.h>


struct mystats {
    nvmlDevice_t device;
    int GPU_mem_used;
    unsigned int GPU_temp;

    int CPU_temp;
    char CPU_path[512];

    float RAM_tot;
    float RAM_used;
};


/*
---UNUSED---
#define BAT_CAP_PATH "/sys/class/power_supply/BAT1/capacity"
#define BAT_STATUS_PATH "/sys/class/power_supply/BAT1/status"
*/

//do{}while(0) to make the compiler treat it like a function


//---READS THE CPU TEMPERATURE AND THE BATTERY CAPACITY---
int readValue(const char *PATH) {
    //file descriptor and buffer for read()
    int fd;
    char buffer[16];

    //safety check for open()
    if((fd = open(PATH, O_RDONLY)) == -1) {
        perror("open");
        return -1;
    }

    //safety check for read()
    ssize_t read_bytes;
    if((read_bytes = read(fd, buffer, sizeof(buffer) - 1)) == -1) {
        perror("read");
        close(fd);
        return -1;
    } 

    close(fd);

    //avoids atoi() to read trash
    buffer[read_bytes] = '\0';
    return atoi(buffer);   
}

/*
//---READ THE BATTERY STATUS---
void readBatStatus(char *batStat, size_t max_len){
    int fd;
    
    //safety check for open()
    if((fd = open(BAT_STATUS_PATH, O_RDONLY)) == -1) {
        exit_with_sys_err(("open"));
    }


    //safety check for read()
    ssize_t read_bytes;
    if((read_bytes = read(fd, batStat, max_len - 1)) == -1) {
        exit_with_sys_err("read");
    } 

    close(fd);

    //null-terminator
    batStat[read_bytes] = '\0';
}
*/

//---Finds the correct path for the CPU---
int CPUpathGetter(char *path_buffer, size_t max_len) {
    const char *base_path = "/sys/class/hwmon";


    DIR *input_dir = opendir(base_path);
    if(input_dir == NULL) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    while((entry = readdir(input_dir)) != NULL) {
        if(entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            char name_path[512];
            snprintf(name_path, sizeof(name_path), "%s/%s/name", base_path, entry->d_name);

            int fd;
            if((fd = open(name_path, O_RDONLY)) == -1) {
                continue;
            }
            
            char name_buf[32];
            ssize_t bytes;
            if((bytes = read(fd, name_buf, sizeof(name_buf) - 1)) == -1) {
                close(fd);
                continue;
            }
            close(fd);

            if (bytes > 0) {
                name_buf[bytes] = '\0';
                if (name_buf[bytes-1] == '\n') name_buf[bytes-1] = '\0';
                    if (strcmp(name_buf, "k10temp") == 0 || strcmp(name_buf, "coretemp") == 0) {                
                    snprintf(path_buffer, max_len, "%s/%s/temp1_input", base_path, entry->d_name);
                    closedir(input_dir);
                    return 0;
                }
            }
        }
    }

    closedir(input_dir);
    return -1;
}

void memUpdate(struct mystats *stats) {
    int fd = open("/proc/meminfo", O_RDONLY);
    char buf[512];


    ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (bytes_read > 0) {
        buf[bytes_read] = '\0';
        
        long long mem_total_kb = 0;
        long long mem_available_kb = 0;

        char *total_ptr = strstr(buf, "MemTotal:");
        char *avail_ptr = strstr(buf, "MemAvailable:");

        if (total_ptr) {
            sscanf(total_ptr, "MemTotal: %lld kB", &mem_total_kb);
        }
        if (avail_ptr) {
            sscanf(avail_ptr, "MemAvailable: %lld kB", &mem_available_kb);
        }

        if (mem_total_kb > 0 && mem_available_kb > 0) {
            stats->RAM_tot = mem_total_kb / (1024.0 * 1024.0);
            stats->RAM_used = (mem_total_kb - mem_available_kb) / (1024.0 * 1024.0);
        }
    }    
}


void statsUpdate(struct mystats *stats) {
    nvmlMemory_t GPU_memory;

    nvmlDeviceGetMemoryInfo(stats->device, &GPU_memory);
    stats->GPU_mem_used = (int)(((double)GPU_memory.used / (double)GPU_memory.total) * 100 + 0.5);

    nvmlDeviceGetTemperature(stats->device, NVML_TEMPERATURE_GPU, &stats->GPU_temp);

    int temp_raw = readValue(stats->CPU_path);
    stats->CPU_temp = (temp_raw >= 0) ? temp_raw / 1000 : -1;

    struct sysinfo info;
    sysinfo(&info);

    //Doesn't work(I mean it "work" but it doesn't like work work)
    /*stats->RAM_tot = (info.totalram * info.mem_unit) / (1024.0 * 1024.0 * 1024.0);
    stats->RAM_used = (info.totalram * info.mem_unit) / (1024.0 * 1024.0 * 1024.0) - (info.freeram * info.mem_unit) / (1024.0 * 1024.0 * 1024.0);*/
    
    memUpdate(stats);
}

int statsInit(struct mystats *stats) {
    if(CPUpathGetter(stats->CPU_path, 512)) {
        perror("Unable to find CPU");
        return 1;
    }

    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Errore inizializzazione NVML: %s\n", nvmlErrorString(result));
        return -1;
    }

    nvmlDeviceGetHandleByIndex(0, &stats->device);
    return 0;
}

int statsCleanup(void) {
    nvmlShutdown();

    return 0;
}


/*char *total_ptr = strstr(buf, "MemTotal:");
        char *avail_ptr = strstr(buf, "MemAvailable:");

        if (total_ptr) {
            sscanf(total_ptr, "MemTotal: %lld kB", &mem_total_kb);
        }
        if (avail_ptr) {
            sscanf(avail_ptr, "MemAvailable: %lld kB", &mem_available_kb);
        }*/