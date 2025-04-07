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

class chip8 {
public:
    chip8() = default;
    ~chip8() = default;

    void initialize();

    bool load_rom(const char* filepath);

    void emulateCycle();

    // Graphics
    unsigned char gfx[64 * 32];     // Graphics written on a 64 x 32 pixel screen. Only black/white colors supported

    // Keyboard (hex based)
    unsigned char key[16];

    // Chip 8 does not need to draw every cycle
    bool drawFlag;

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

    // Two timer registers, count @ 60 Hz. Set > 0 --> will count down to 0
    unsigned char delay_timer;
    unsigned char sound_timer;      // Will sound whenever timer reaches 0

    // Stack - remember current location before a jump is performed by PC
    unsigned short stack[16];
    unsigned short sp;              // Stack pointer
};

#endif //CHIP8_H