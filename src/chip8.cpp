#include "chip8.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <random>

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
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    // Seed rng
    srand (time(NULL));
}

bool chip8::load_rom(const char *filepath) {
    initialize();

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (file) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> buffer(size); // Allocate space for the file content
        if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::copy(buffer.begin(), buffer.end(), memory + 0x200);
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
            switch (opcode & 0x000F) {
                case 0x0000:
                    memset(gfx, 0, sizeof(gfx));
                    drawFlag = true;
                    pc += 2;
                break;

                case 0x000E: // Returns from a subroutine
                    pc = stack[--sp];
                    pc += 2;
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
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
        break;

        case 0x7000: // Add NN to vX
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
        break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // Set vX to the value of vY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0001: // Set vX to the result of bitwise vX OR vY
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0002: // Set vX to the result of bitwise vX AND vY
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0003: // Set vX to the result of bitwise vX XOR vY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0004: // Add vY to vX, vF is set to 1 if an overflow happened, to 0 if not, even if X=F
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    pc += 2;
                break;

                case 0x0005:
                    // Subtract vY from vX, vF is set to 0 if an underflow happened, to 1 if not, even if X=F
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0006: // Shifts VX to the right by 1, then stores the LSB of VX prior to the shift into VF
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x01);
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                break;

                case 0x0007: {
                    // Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    uint8_t vX7 = V[(opcode & 0x0F00) >> 8];
                    uint8_t vY7 = V[(opcode & 0x00F0) >> 4];

                    V[0xF] = vX7 > vY7 ? 0 : 1;

                    V[(opcode & 0x0F00) >> 8] = vY7 - vX7;

                    pc += 2;
                    break;
                }

                case 0x000E: // Shifts VX to the left by 1, then sets VF to 1 if the MSB of VX prior to that shift was set, or to 0 if it was unset
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80) != 0 ? 1 : 0; // Get MSB
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                break;
                default: printf("Unknown opcode: 0x%X\n", opcode);
            }

        break;

        case 0x9000: // Skips the next instruction if VX does not equal VY
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
        break;

        case 0xA000: // Sets I to the address NNN
            // Execute opcode
            I = opcode & 0x0FFF;
            pc += 2;
        break;

        case 0xB000: // Jumps to the address NNN plus V0
            pc = V[0x0] + (opcode & 0x0FFF);
        break;

        case 0xC000: // Sets VX to the result of a bitwise and operation on a random number and NN
            V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
            pc += 2;
        break;

        case 0xD000: { // Draws a sprite at coordinate (VX, VY)
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            // VF set to 1 if any screen pixels are flipped
            // from set to unset when sprite is drawn, 0 if not
            V[0xF] = 0;
            for (int yline = 0; yline < height; ++yline) {
                pixel = memory[I + yline];

                for (int xline = 0; xline < 8; ++xline) { // Width is always 8 pixels
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (gfx[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: // Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed
                    if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                break;

                case 0x00A1: // Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                break;

                default: printf("Unknown opcode: 0x%X\n", opcode);
            }
        break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // Sets VX to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                break;

                case 0x000A: { // A key press is awaited, and then stored in VX
                    bool key_press = false;

                    for (uint8_t i = 0; i < 16; ++i) {
                        if (key[i]) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            key_press = true;
                            break;
                        }
                    }

                    if (!key_press) break;

                    pc += 2;
                } break;

                case 0x0015: // Sets the delay timer to VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x0018: // Sets the sound timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x001E: // Adds VX to I. VF is not affected
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x0029: // Sets I to the location of the sprite for the character in VX(only consider the lowest nibble)
                    I = V[(opcode & 0x0F00) >> 8] * 0x5; // Each sprite 5 bytes wide
                    pc += 2;
                break;

                case 0x0033: // Stores the binary-coded decimal representation of VX
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                break;

                case 0x0055: // Stores from V0 to VX (including VX) in memory, starting at address I
                    for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i) {
                        memory[I + i] = V[i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                break;

                case 0x0065: // Fills from V0 to VX (including VX) with values from memory, starting at address I
                    for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i) {
                        V[i] = memory[I + i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                break;

                default: printf("Unknown opcode: 0x%X\n", opcode);
            }
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