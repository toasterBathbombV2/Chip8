#define MEMORY_START_ADDRESS 0x200
#define MEMORY_SIZE 0x1000
#define FONTS_START_ADDRESS 0xF00

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

// #define USABLE_MEMORY MEMORY_SIZE - MEMORY_START_ADDRESS

#include <cstdint>
#include <fstream>
#include <iostream>

class chip8 {
   public:
    void init();
    void loadROM(char* fileAddress);

    void fetchOpcode();
    void execute();

    void c00E0();  // cls
    void c00EE();  // ret

    void c1NNN();  // jmp
    void c2NNN();  // call
    void c3XNN();  // skip
    void c4XNN();
    void c5XY0();
    void c6XNN();
    void c7XNN();

    void c8XY0();
    void c8XY1();
    void c8XY2();
    void c8XY3();
    void c8XY4();
    void c8XY5();
    void c8XY6();
    void c8XY7();
    void c8XYE();

    void c9XY0();
    void cANNN();
    void cBNNN();
    void cCXNN();

    void cDXYN();

    void cEX9E();
    void cEXA1();

    void cFX07();
    void cFX0A();
    void cFX15();
    void cFX18();
    void cFX1E();
    void cFX29();
    void cFX33();
    void cFX55();
    void cFX65();

    uint8_t keyStroke[16]{};

    uint32_t screen[VIDEO_WIDTH * VIDEO_HEIGHT];

    uint8_t V[16]{};            // GENERAL PURPOSE - VF should not be used by prog
    uint8_t mem[0x1000]{};      // 4096 byte memory
    uint16_t stack[16]{};       // stack frame depth - 16 levels

    uint16_t I;                 // ADDRESS REG - use only lowest 12 bits
    uint16_t pc;                // PROGRAM COUNTER
    uint16_t sp = 0;            // points to top of stack
    uint16_t opcode;            

    uint8_t delayTimer;
    uint8_t soundTimer;         // while >0, decrement at 60Hz

    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };
};
