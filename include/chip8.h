/**
* CPU core implementation for Chip-8 interpreter
*
* Implementation of 35 different op codes (2 bytes long)
* More information about Chip-8 can be found on the wikipedia page
* https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
*
* This code is mostly based on the guidelines provided by Laurence Muller, but
* with my own implementation for the code
*
* His article can be found here:
* https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
*/

#ifndef CHIP8_H
#define CHIP8_H

#include <cstdio>
#include <string.h>
#include <string>

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

class chip8 {
public:
    chip8() = default;

    void initialize(const char* filepath) {
        pc = 0x200; // PC expects start at 0x200
        opcode = 0;
        I = 0;
        sp = 0;

        // Reset variables
        memset(stack, 0, sizeof(stack));
        memset(V, 0, sizeof(V));
        memset(key, 0, sizeof(key));
        memset(memory, 0, sizeof(memory));
        memset(gfx, 0, sizeof(gfx)); // Clear display

        // Reset timers
        delay_timer = 0;
        sound_timer = 0;

        // Loading fontset to memory
        for (int i = 0; i < 80; ++i) {
            memory[i] = chip8_fontset[i];
        }
    };

    // TODO -- finish this method, still need to find a way to store data from file and store it in memory
    bool load_rom(const char* filepath) {
        // Load file contents into a buffer
        FILE* file = fopen(filepath, "rb"); // Read file in binary mode

        if (file == nullptr) {
            return false;
        }

        fseek(file, 0, SEEK_END); // Find file size
        const long size = ftell(file);
        fclose(file);

        // Load program into memory
        for (int i = 0; i < size; ++i) {
            memory[i + 512] = file[i];
        }

    }

    void emulateCycle() {

    };

private:
    unsigned short opcode;          // Op codes are 2 bytes long
    unsigned char memory[4096];     // 4k memory total
    unsigned char V[16];            // 15 8-bit general purpose registers, 16th used for carry flag
    unsigned short I;               // Index register
    unsigned short pc;              // Program counter
    /**
     * Systems memory map:
     * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     * 0x050-0x0A0 - Used for the built-in 4x5 pixel font set (0-F)
     * 0x200-0xFFF - Program ROM and work RAM
     */

    // Graphics
    unsigned char gfx[64 * 32];     // Graphics written on a 64 x 32 pixel screen. Only black/white colors supported

    // Two timer registers, count @ 60 Hz. Set > 0 --> will count down to 0
    unsigned char delay_timer;
    unsigned char sound_timer;      // Will sound whenever timer reaches 0

    // Stack - remember current location before a jump is performed by PC
    unsigned short stack[16];
    unsigned short sp;              // Stack pointer

    // Keyboard (hex based)
    unsigned char key[16];
};

#endif //CHIP8_H