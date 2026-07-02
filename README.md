# ags-sysstats

A small C library that reads live CPU, GPU and RAM statistics directly from the Linux system interfaces, built to feed real-time data into an [AGS](https://github.com/Aylur/ags) status bar (Hyprland/Wayland desktop).

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Nix](https://img.shields.io/badge/Nix-5277C3?style=for-the-badge&logo=nixos&logoColor=white)

## What it does

No `/proc` parsing shortcuts, no shelling out to other tools - this library talks to the kernel interfaces directly:

- **CPU temperature** - reads straight from `/sys/class/hwmon/*/temp1_input`, auto-detecting the right hwmon device (`k10temp` for AMD, `coretemp` for Intel) via `opendir`/`readdir`.
- **GPU stats** (memory usage %, temperature) - via **NVML** (NVIDIA Management Library).
- **RAM** (total/used, in GB) - via `sysinfo()`.

The result is a simple `struct mystats` that gets updated on demand, cheap enough to poll continuously for a live bar widget.

## Project structure

```
.
├── include/
│   └── stats.h        # public API + struct mystats
├── src/
│   ├── stats.c         # implementation
│   └── windowInit.c    # minimal usage example
├── Makefile
└── shell.nix           # reproducible dev environment (Nix)
```

## API

```c
int  statsInit(struct mystats *stats);     // resolves CPU hwmon path, initializes NVML
void statsUpdate(struct mystats *stats);   // refreshes all fields
int  statsCleanup(void);                   // shuts down NVML, call before exit
```

## Usage

```c
#include "stats.h"
#include <stdlib.h>

int main() {
    struct mystats *stats = malloc(sizeof(struct mystats));
    statsInit(stats);

    statsUpdate(stats);
    printf(
        "CPU: %d°C | GPU: %u°C | GPU: %d%% | RAM: %.2f/%.2f GB",
        stats->CPU_temp,
        stats->GPU_temp,
        stats->GPU_mem_used,
        stats->RAM_used,
        stats->RAM_tot
    );

    statsCleanup();
    return 0;
}
```

## Building

```sh
make
```

If you use Nix, `shell.nix` sets up a reproducible environment with the required dependencies (NVML headers/libs included):

```sh
nix-shell
make
```

## Requirements

- Linux with `/sys/class/hwmon` exposing CPU temperature (AMD `k10temp` or Intel `coretemp`)
- NVIDIA GPU + driver with NVML available (`libnvidia-ml`)

## Why

Built as the data layer for a custom [AGS](https://github.com/Aylur/ags) + Hyprland status bar - instead of polling stats through shell commands on every tick, this library exposes them as a small, fast, native module.

## License

Add your preferred license here (e.g. MIT, GPL-3.0).
