#include <stdio.h>
#include <stdlib.h>

/* Most programs begin here and do not
   access anything lower than this,
   this is where the interpreter itself
   lived. */
#define PROGRAM_START 0x200;

/* 4 KiB of memory in the system */
uint8_t memory[4096];

/* Registers are named V0, V1, ..., VF,
   and are 8-bit. */
uint8_t registers[16];

/* Address register is 16 bits long. */
unsigned short register_i;

/* 4 KiB would take 12 bits to address. */
unsigned short program_counter;

/* Pixels on the screen are off or on, so will just
   be storing 1's and 0's here. */
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
static uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
 
/* Original 1802 version had 24 levels of nesting */
unsigned short stack[24];
uint8_t stack_pointer;

/* State of all hex keys must be saved. */
uint8_t keypad[16];

/* Two timers that count down at 60Hz
   Soudn timer causes beep when finished. */
uint8_t delay_timer;
uint8_t sound_timer;


int load_rom()
{
    program_counter = PROGRAM_START;
    register_i = 0;
    stack_pointer = 0;

    FILE *file = fopen("pong", "rb");
    fread(memory, sizeof(memory), sizeof(char), file);
    return 0;
}


void emulate_cycle()
{
    char byte1 = memory[program_counter] >> 4 & 0xF;
    char byte2 = memory[program_counter] & 0xF;
    char byte3 = memory[program_counter + 1] >> 4 & 0xF;
    char byte4 = memory[program_counter + 1] & 0xF;
    switch (byte1)
    {
    case 0x0:
        switch (byte3 << 4 | byte4)
        {
        case 0xEE:
            program_counter = stack[stack_pointer--];
            break;
        
        case 0xE0:
            for (int i = 0; i <= SCREEN_HEIGHT * SCREEN_WIDTH - 1; i++)
                *((uint8_t*) screen + i) = 0;
        default:
            break;
        }
        break;

    case 0x1:
        program_counter = byte2 << 8 | byte3 << 4 | byte4;
        break;

    case 0x2:
        stack[stack_pointer++] = program_counter + 2;
        program_counter = byte2 << 8 | byte3 << 4 | byte4;
        break;

    case 0x3:
        if (registers[byte2] == (byte3 << 4 | byte4))
            program_counter += 2;
        program_counter += 2;
        break;
    
    case 0x4:
        if (registers[byte2] != (byte3 << 4 | byte4))
            program_counter += 2;
        program_counter += 2;
        break;
    
    case 0x5:
        if (registers[byte2] == registers[byte3])
            program_counter += 2;
        program_counter += 2;
        break;
    
    case 0x6:
        registers[byte2] = (byte3 << 4 | byte4);
        program_counter += 2;
        break;
    
    case 0x7:
        registers[byte2] += (byte3 << 4 | byte4);
        program_counter += 2;
        break;
    
    case 0x8:
        switch (byte4)
        {
        case 0x0:
            registers[byte2] = registers[byte3];
            break;
        
        case 0x1:
            registers[byte2] = registers[byte2] | registers[byte3];
            break;

        case 0x2:
            registers[byte2] = registers[byte2] & registers[byte3];
            break;
        
        case 0x3:
            registers[byte2] = registers[byte2] ^ registers[byte3];
            break;
        
        case 0x4:
            registers[0xF] = 
                ((int) registers[byte2] + (int) registers[byte3] > 0xFF)
                ? 1
                : 0;
            registers[byte2] += registers[byte3];
            break;
        
        case 0x5:
            registers[0xF] = registers[byte2] > registers[byte3] ? 1 : 0;
            registers[byte2] -= registers[byte3];
            break;
        
        case 0x6:
            registers[0xF] = registers[byte2] & 1;
            registers[byte2] = registers[byte2] >> 1;
            break;

        case 0x7:
            registers[0xF] = registers[byte3] > registers[byte2] ? 1 : 0;
            registers[byte2] = registers[byte3] - registers[byte3];
            break;
        
        case 0xE:
            registers[0xF] = registers[byte2] & 0x80;
            registers[byte2] = registers[byte2] << 1;
            break;

        default:
            break;
        }
        program_counter += 2;
        break;
    
    case 0x9:
        if (registers[byte2] != registers[byte3])
            program_counter += 2;
        program_counter += 2;
        break;

    case 0xA:
        register_i = byte2 << 8 | byte3 << 4 | byte4;
        program_counter += 2;
        break;
    
    case 0xB:
        program_counter = registers[0] + (byte2 << 8 | byte3 << 4 | byte4);
        break;

    case 0xC:
        registers[byte2] = rand() & (byte3 << 4 | byte4);
        program_counter += 2;
        break;
    
    case 0xD:
        registers[0xF] = 0;
        for (int height = 0; height <= byte4; height++)
        {
            for (int length = 0; length <= 8; length++)
            {
                uint8_t row = byte2 + height;
                uint8_t col = byte3 + length;
                screen[row][col] ^= memory[register_i] & (1 << length);
                registers[0xF] |= ((memory[register_i] >> length) & (1)) & screen[row][col];
            }
        }
        program_counter += 2;
        break;
    
    case 0xE:
        switch (byte3 << 4 | byte4)
        {
        case 0x9E:
            if (keypad[registers[byte2]]) //TODO: change to SDL
                program_counter += 2;
            program_counter += 2;
            break;
        
        case 0xA1:
            if (!keypad[registers[byte2]]) //TODO: change to SDL
                program_counter += 2;
            program_counter += 2;
            break;
        
        default:
            break;
        }
        break;
    
    case 0xF:
        switch (byte3 << 4 | byte4)
        {
        case 0x07:
            registers[byte2] = delay_timer;
            break;
        
        case 0x0A:
            //TODO: wait for key press
            break;
        
        case 0x15:
            delay_timer = registers[byte2];
            break;
        
        case 0x18:
            sound_timer = registers[byte2];
            break;

        case 0x1E:
            registers[0xF] = ((int) register_i + (int) registers[byte2] > 0xFFF) ? 1 : 0;
            register_i += registers[byte2];
            break;
        
        case 0x29:
            register_i = 0x50 + (registers[byte2]) * 5;
            break;
        
        case 0x33:
            memory[register_i] = registers[byte2] / 100;
            memory[register_i + 1] = (registers[byte2] % 100) / 10;
            memory[register_i + 2] = registers[byte2] % 10;
            break;
        
        case 0x55:
            for(int i = 0; i <= byte2; i++)
                memory[register_i++] = registers[i];
            break;
        
        case 0x65:
            for(int i = 0; i <= byte2; i++)
                registers[i] = memory[register_i++];
            break;
        
        default:
            break;
        }
        program_counter += 2;
        break;
    
    default:
        break;
    }
}