# STM32 Tutorial

This is the repository for the demo presented at the STM32 programming course at NTUEESAAD

## How to Build
1. Clone the repository

```bash
git clone git@github.com:EricLin0123/STM32Tutorial.git
```

1. Change the directory to the build directory

```bash
mkdir build; cd build
```
3. Compile the code

```bash
cmake ..
cmake --build .
```
4. Flash the following .elf file to the board

```bash
build/F103Template.elf
```

## How to use
In /Core/Src/main.c, define the example you want to run to be `1`. For example if you want to run ADC example:
```C
#define isblink 0
#define DMAdemo 0
#define isDMA 0
#define isADC 1
#define isDMA_ADC
#define isPWM 0
#define isWS2812 0
#define isIMU 0
#define isUARTdemo 0
```

