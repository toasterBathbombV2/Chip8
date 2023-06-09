#include "chip8.h"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include "SDL2/SDL.h"

const bool DEBUG = false;

using namespace std;

void chip8::init() {
    if (DEBUG) {
        cout << "chip8 :: INIT start" << endl;
    }

    pc = MEMORY_START_ADDRESS;
    sp = 0;
    I = 0;
    opcode = 0;

    for (int i = 0; i < 80; i++) {
        mem[0x50 + i] = font[i];
    }

    if (DEBUG) {
        cout << "chip8 :: INIT end" << endl;
    }
}

void chip8::loadROM(char* fileAddress) {
    if (DEBUG) {
        cout << "chip8 :: Load ROM start" << endl;
    }

    FILE* binFile = fopen(fileAddress, "r");

    if (binFile == NULL) {
        std::cerr << "File Error Detected!" << std::endl;
    }

    char buffer[MEMORY_SIZE - MEMORY_START_ADDRESS]{0};
    while (!feof(binFile)) {
        fread(buffer, sizeof(buffer), 1, binFile);
    }

    for (int i = 0; i < MEMORY_SIZE - MEMORY_START_ADDRESS; i++) {
        mem[MEMORY_START_ADDRESS + i] = buffer[i];
    }

    // delete[] buffer;

    if (DEBUG) {
        cout << "chip8 :: Load ROM end" << endl;
    }
}

void chip8::fetchOpcode() { opcode = mem[pc] << 8 | mem[pc + 1]; }

void chip8::execute() {
    if (DEBUG) {
        cout << "chip8 :: Execute Cycle start" << endl;
        printf("Opcode fetched: %d /n", opcode);
    }

    uint8_t nib;

    fetchOpcode();
    pc += 2;

    switch (opcode & 0xF000) {
        case 0x0000: {
            uint8_t i = opcode & 0x00FF;
            switch (i) {
                case 0x00E0: {
                    c00E0();
                } break;
                case 0x00EE: {
                    c00EE();
                    break;
                }
                default:
                    cerr << "CRITICAL: opcode 0 error!" << std::endl;
                    break;
            }
        } break;

        case 0x1000:
            c1NNN();
            break;

        case 0x2000:
            c2NNN();
            break;

        case 0x3000:
            c3XNN();
            break;

        case 0x4000:
            c4XNN();
            break;

        case 0x5000:
            c5XY0();
            break;

        case 0x6000:
            c6XNN();
            break;

        case 0x7000:
            c7XNN();
            break;

        case 0x8000: {
            nib = opcode & 0x000F;
            switch (nib) {
                case 0x0000:
                    c8XY0();
                    break;
                case 0x0001:
                    c8XY1();
                    break;
                case 0x0002:
                    c8XY2();
                    break;
                case 0x0003:
                    c8XY3();
                    break;
                case 0x0004:
                    c8XY4();
                    break;
                case 0x0005:
                    c8XY5();
                    break;
                case 0x0006:
                    c8XY6();
                    break;
                case 0x0007:
                    c8XY7();
                    break;
                case 0x000E:
                    c8XYE();
                    break;

                default:
                    cerr << "CRITICAL: opcode 8 error!" << endl;
                    break;
            }

            break;
        }

        case 0x9000:
            c9XY0();
            break;

        case 0xA000:
            cANNN();
            break;

        case 0xB000:
            cBNNN();
            break;

        case 0xC000:
            cCXNN();
            break;

        case 0xD000:
            cDXYN();
            break;

        case 0xE000: {
            nib = opcode & 0x000F;
            switch (nib) {
                case 0xE:
                    cEX9E();
                    break;
                case 0x1:
                    cEXA1();
                    break;

                default:
                    cerr << "CRITICAL: opcode E error!" << endl;
                    break;
            }
            break;
        }

        case 0xF000: {
            nib = (opcode & 0x00FF);
            switch (nib) {
                case 0x0007:
                    cFX07();
                    break;
                case 0x000A:
                    cFX0A();
                    break;
                case 0x0015:
                    cFX15();
                    break;
                case 0x0018:
                    cFX18();
                    break;
                case 0x001E:
                    cFX1E();
                    break;
                case 0x0029:
                    cFX29();
                    break;
                case 0x0033:
                    cFX33();
                    break;
                case 0x0055:
                    cFX55();
                    break;
                case 0x0065:
                    cFX65();
                    break;
                default:
                    cerr << "CRITICAL: opcode F error!" << endl;
                    break;
            }
            break;
        }

        default:
            std::cout << "CRITICAL: Unknown Opcode!" << std::endl;
            break;
    }

    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) {
        --soundTimer;
        std::cout << '\a';
        // beep();
    }

    if (DEBUG) {
        cout << "chip8 :: Execute Cycle end" << endl;
    }
}

void chip8::c00E0() { memset(&screen, 0, sizeof(screen)); }

void chip8::c00EE() {
    --sp;
    pc = stack[sp];
}

void chip8::c1NNN() { pc = opcode & 0x0FFF; }

void chip8::c2NNN() {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
}

void chip8::c3XNN() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (V[x] == nn) {
        pc += 2;
    }
}

void chip8::c4XNN() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (V[x] != nn) {
        pc += 2;
    }
}

void chip8::c5XY0() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = opcode & 0x00F0 >> 4;

    if (V[x] == V[y]) {
        pc += 2;
    }
}

void chip8::c6XNN() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    V[x] = nn;
}

void chip8::c7XNN() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    V[x] += nn;
}

void chip8::c8XY0() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[y];
}

void chip8::c8XY1() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] |= V[y];
}

void chip8::c8XY2() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] &= V[y];
}

void chip8::c8XY3() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] ^= V[y];
}

void chip8::c8XY4() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint8_t sum = V[x] + V[y];

    V[0xF] = sum > 0xFF;
    V[x] = sum & 0xFF;

    // if (sum < V[x]) {
    //     V[0xF] = 1;
    // } else {
    //     V[0xF] = 0;
    // }
    // V[x] = sum;
}

void chip8::c8XY5() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    // ? should the condition be just '>' instead of '>='
    if (V[x] > V[y]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }

    V[x] -= V[y];
}

void chip8::c8XY6() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[0xF] = V[x] & 1;
    V[x] = V[x] >> 1;
}

void chip8::c8XY7() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    // Check with other sources
    if (V[y] > V[x]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }

    V[x] = V[y] - V[x];
}

void chip8::c8XYE() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[0xF] = (V[x] & 0x80) >> 7;
    V[x] <<= 1;
}

void chip8::c9XY0() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (V[x] != V[y]) {
        pc += 2;
    }
}

void chip8::cANNN() { I = opcode & 0x0FFF; }

void chip8::cBNNN() {
    uint8_t nnn = opcode & 0x0FFF;

    pc = V[0] + nnn;
}

void chip8::cCXNN() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    srand(time(NULL));

    uint8_t random = rand();

    V[x] = random & nn;
}

void chip8::cDXYN() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t hc8XY = opcode & 0x000F;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = V[Vx] % 64;
    uint8_t yPos = V[Vy] % 32;

    V[0xF] = 0;

    for (unsigned int row = 0; row < hc8XY; ++row) {
        uint8_t spriteByte = mem[I + row];

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &screen[(yPos + row) * 64 + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel) {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF) {
                    V[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void chip8::cEX9E() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keyStroke[V[x]]) {
        pc += 2;
    }
}

void chip8::cEXA1() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (!keyStroke[V[x]]) {
        pc += 2;
    }
}

void chip8::cFX07() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[x] = delayTimer;
}

void chip8::cFX0A() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    if (keyStroke[0]) {
        V[x] = 0;
    } else if (keyStroke[1]) {
        V[x] = 1;
    } else if (keyStroke[2]) {
        V[x] = 2;
    } else if (keyStroke[3]) {
        V[x] = 3;
    } else if (keyStroke[4]) {
        V[x] = 4;
    } else if (keyStroke[5]) {
        V[x] = 5;
    } else if (keyStroke[6]) {
        V[x] = 6;
    } else if (keyStroke[7]) {
        V[x] = 7;
    } else if (keyStroke[8]) {
        V[x] = 8;
    } else if (keyStroke[9]) {
        V[x] = 9;
    } else if (keyStroke[10]) {
        V[x] = 10;
    } else if (keyStroke[11]) {
        V[x] = 11;
    } else if (keyStroke[12]) {
        V[x] = 12;
    } else if (keyStroke[13]) {
        V[x] = 13;
    } else if (keyStroke[14]) {
        V[x] = 14;
    } else if (keyStroke[15]) {
        V[x] = 15;
    } else {
        pc -= 2;
    }
}

void chip8::cFX15() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    delayTimer = V[x];
}

void chip8::cFX18() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    soundTimer = V[x];
}

void chip8::cFX1E() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    I += V[x];
}

void chip8::cFX29() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t num = V[x];

    I = FONTS_START_ADDRESS + (5 * num);
}

void chip8::cFX33() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t num = V[x];

    mem[I] = num % 1000;
    mem[I + 1] = num % 100;
    mem[I + 2] = num % 10;
}

void chip8::cFX55() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        mem[I + i] = V[i];
    }
}

void chip8::cFX65() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        V[i] = mem[i + I];
    }
}

