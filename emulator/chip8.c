#define PROGRAM_START 0x200;
unsigned char memory[4096];
unsigned char registers[16];
unsigned short register_i;
unsigned short program_counter;
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
unsigned char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
unsigned short stack[24];
unsigned char stack_pointer;
unsigned char keypad;


void emulate_cycle()
{
    unsigned short opcode = memory[program_counter] | memory[program_counter + 1];
    
}

void initialize()
{
    program_counter = PROGRAM_START;
    register_i = 0;
    stack_pointer = 0;
}

void emulate()
{
    initialize();
    while(1)
    {
        emulate_cycle();
    }
}

int main(int argc, char **argv)
{
    emulate();    
    return 0;
}
