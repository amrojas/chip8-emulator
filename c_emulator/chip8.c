#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* 4 KiB of memory in the system */
uint8_t memory[4096];

/* Registers are named V0, V1, ..., VF,
   and are 8-bit. */
uint8_t registers[16];

/* Address register is 16 bits long. */
unsigned short register_i;

/* 4 KiB would take 12 bits to address. */
unsigned short program_counter;

/* 1s and 0s for screen go here */
uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
 
/* Original 1802 version had 24 levels of nesting */
unsigned short stack[24];
uint8_t stack_pointer;

/* State of keys must be saved. */
uint8_t keypad[NUM_KEYS];

/* Two timers that count down at 60Hz
   Soudn timer causes beep when finished. */
uint8_t delay_timer;
uint8_t sound_timer;

const SDL_Keycode keymap[16] = {
    SDLK_1, SDLK_2, SDLK_3, SDLK_4,
    SDLK_q, SDLK_w, SDLK_e, SDLK_r,
    SDLK_a, SDLK_s, SDLK_d, SDLK_f,
    SDLK_z, SDLK_x, SDLK_c, SDLK_v,
};

static unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


int load_rom()
{
    program_counter = PROGRAM_START;
    register_i = 0;
    stack_pointer = 0;

    FILE *file = fopen("roms/Pong [Paul Vervalin, 1990].ch8", "rb");
    fread(memory, sizeof(memory), sizeof(char), file);
    fclose(file);
    return 0;
}


void emulate_cycle()
{
    // Byte count is from left to right
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
                uint8_t new_state = screen[row][col] ^ memory[register_i] & (1 << length);
                if (new_state == 0 && screen[row][col] == 1)
                    registers[0xF] |= 1;
                screen[row][col] = new_state;
            }
        }
        program_counter += 2;
        break;
    
    case 0xE:
        switch (byte3 << 4 | byte4)
        {
        case 0x9E:
            if (keypad[registers[byte2]])
                program_counter += 2;
            program_counter += 2;
            break;
        
        case 0xA1:
            if (!keypad[registers[byte2]])
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
        {
            bool key_is_pressed = false;
            for (int i = 0; i < NUM_KEYS; i++)
            {
                if (keypad[i])
                {
                    registers[byte2] = i;
                    key_is_pressed = true;
                }
            }
            // Early return and let key presses be recognized
            // Will keep rerunning this instruction
            if (!key_is_pressed)
                return;
            break;
        }
        
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

void set_key(int keynum, bool value)
{
    keypad[keynum] = value;
}