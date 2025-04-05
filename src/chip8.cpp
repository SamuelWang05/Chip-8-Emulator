#include "chip8.h"
#include <cstdio>
#include <ctime>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

inline unsigned char chip8_fontset[80] =
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

void chip8::initialize() {
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
    std::copy(chip8_fontset, chip8_fontset + std::size(chip8_fontset), memory);
}

bool chip8::load_rom(const char *filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);    // Read in binary mode, move file pointer to the end

    if (file) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer;

        if (file.read(buffer.data(), size)) { // Read contents into buffer
            for (int i = 0; i < size; ++i) {
                memory[i + 512] = buffer[i]; // Start filling memory at 0x200
            }

            return true;
        }
    }
    return false;
}

void chip8::emulateCycle() {
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1]; // opcodes are 2 bytes

    // Decode opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00F) {
                case 0x0000: // Clears the screen
                    memset(gfx, 0, sizeof(gfx));
                break;

                case 0x000E: // Returns from a subroutine
                    pc = stack[--sp];
                    pc+=2;
                break;

                default: printf("Unknown opcode: 0x%X\n", opcode);
            }
        break;

        case 0x1000: // Jumps to address NNN
                pc = opcode & 0x0FFF;
        break;

        case 0x2000: // Calls subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
        break;

        case 0x3000: // Skips the next instruction if VX equals NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else pc += 2;
        break;

        case 0x4000: // Skips next opcode if vX != N
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else pc += 2;
        break;

        case 0x5000: // Skips the next instruction if VX equals VY
            if (V[(opcode & 0x0F00) >> 8 ] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else pc += 2;
        break;

        case 0x6000: // Sets VX to NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) >> 4;
            pc += 2;
        break;

        case 0x7000: // Add NN to vX
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF) >> 4;
            pc += 2;
        break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // Set vX to the value of vY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0001: // Set vX to the result of bitwise vX OR vY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0002: // Set vX to the result of bitwise vX AND vY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0003: // Set vX to the result of bitwise vX XOR vY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0004: // Add vY to vX, vF is set to 1 if an overflow happened, to 0 if not, even if X=F
                    V[]
                break;

                default: printf("Unknown opcode: 0x%X\n", opcode);
            }

        break;

        case 0xA000: // ANNN: Sets I to the address NNN
            // Execute opcode
                I = opcode & 0x0FFF;
        pc += 2;
        break;

        default: printf("Unknown opcode: 0x%X\n", opcode);
    }

    // Update timers
    if(delay_timer > 0)
        --delay_timer;

    if(sound_timer > 0)
    {
        if(sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}
