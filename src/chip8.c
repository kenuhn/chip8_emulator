#include "chip8.h"

// Load the rom into memory starting at location 0x200
void load_rom(Chip8 *chip8, const char *rom_filename)
{
    long rom_length;
    uint8_t *rom_buffer;

    FILE *rom = fopen(rom_filename, "rb");
    if (rom != NULL)
    {
        // Get the size of the rom to allocate memory for a buffer
        fseek(rom, 0, SEEK_END);
        rom_length = ftell(rom);
        rewind(rom);

        rom_buffer = (uint8_t *)malloc(sizeof(uint8_t) * rom_length);
        if (rom_buffer == NULL)
        {
            printf("ERROR: Out of memory\n");
            exit(EXIT_FAILURE);
        }

        // TODO: handle error if not read into memory?
        fread(rom_buffer, sizeof(uint8_t), rom_length, rom);

        // Check that the rom is not too large for the region in memory it is placed in
        if ((PROGRAM_END_ADDR - PROGRAM_START_ADDR) >= rom_length)
        {
            for (int i = 0; i < rom_length; i++)
            {
                chip8->ram[i + 0x200] = rom_buffer[i];
            }
        }
        else
        {
            printf("ERROR: ROM file too large\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("ERROR: ROM file does not exist\n");
        exit(EXIT_FAILURE);
    }

    fclose(rom);
    free(rom_buffer);
}

/*
 * Initilize the system to its startup state, all of the
 * ram elements are set to 0, the pile is cleared, and the registers
 * are cleared. The PC register starts execution at location 0x200
 * in memory
 */
void init_system(Chip8 *chip8)
{

    chip8->executionFlag = TRUE;
    chip8->screenFlag = FALSE;
    chip8->pauseFlag = FALSE;

    chip8->pc = PC_START;
    chip8->opcodeCourant = 0;
    chip8->pPile = 0;
    chip8->pIndex = 0;

    // Clear display (memory)
    for (int i = 0; i < ECRAN_HAUTEUR; i++)
    {
        for (int j = 0; j < ECRAN_LARGEUR; j++)
        {
            chip8->screen[i][j] = 0;
        }
    }

    // Clear pile
    for (int i = 0; i < TAILLE_PILE; i++)
    {
        chip8->pile[i] = 0;
    }

    // Clear ram
    for (int i = 0; i < TOTAL_RAM; i++)
    {
        chip8->ram[i] = 0;
    }

    // Clear registers
    for (int i = 0; i < NOMBRE_REGISTRE; i++)
    {
        chip8->registre[i] = 0;
    }

    // Load fontset into memory
    for (int i = 0; i < TAILLE_SPRITES; i++)
    {
        chip8->ram[i] = SPRITES[i];
    }

    // Set timers to 0
    chip8->compteurJeu = 0;
    chip8->compteurSon = 0;

    // Keyboard setup
    for (int i = 0; i < NOMBRE_TOUCHE; i++)
    {
        chip8->clavier[i] = FALSE;
    }
    chip8->clavierFlag = FALSE;
}

// Largely similar to the init function, however all of the ram is not cleared
// (so the rom does not have to be re-loaded into memory)
void reset_system(Chip8 *chip8)
{
    chip8->executionFlag = TRUE;
    chip8->screenFlag = FALSE;
    chip8->pauseFlag = FALSE;

    chip8->pc = PC_START;
    chip8->opcodeCourant = 0;
    chip8->pPile = 0;
    chip8->pIndex = 0;

    // Clear display (memory)
    for (int i = 0; i < ECRAN_HAUTEUR; i++)
    {
        for (int j = 0; j < ECRAN_LARGEUR; j++)
        {
            chip8->screen[i][j] = 0;
        }
    }

    // Clear ram from the fontset end (80) to the Program ram
    for (int i = 80; i < PROGRAM_START_ADDR; i++)
    {
        chip8->ram[i] = 0;
    }

    // Clear registers, clavier and pile (all 16 each)
    for (int i = 0; i < 16; i++)
    {
        chip8->registre[i] = 0;
        chip8->clavier[i] = FALSE;
        chip8->pile[i] = 0;
    }
    // Reset timers to 0
    chip8->compteurJeu = 0;
    chip8->compteurSon = 0;
}

/*
 * Fetches the next opcode to execute from memory which is
 * located at the pc and the pc + 1 (opcode is 2 bytes)
 */
uint16_t fetch_opcode(Chip8 *chip8)
{
    uint16_t opcode;

    uint8_t msB = chip8->ram[chip8->pc];
    uint8_t lsB = chip8->ram[chip8->pc + 1];

    opcode = msB << 8 | lsB;
    printf("opcode %u\n", opcode);
    return opcode;
}

/*
 * Calls the instruction to execute based on the fetched opcode.
 *
 * If logging is enabled, the program will print the opcode and what
 * instruction was ran.
 */
void execute_instruction(Chip8 *chip8, int logging)
{
    uint16_t opcode = fetch_opcode(chip8);
    chip8->opcodeCourant = opcode;

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            if (logging)
            {
                printf("Instruction Clear screen (00E0)\n");
            }
            cls(chip8);
            break;

        case 0x00EE:
            if (logging)
            {
                printf("Instruction Return from Subroutine (00EE)\n");
            }
            return_from_subroutine(chip8);
            break;

        default:
            printf("ERROR: Unrecognized opcode 0x%X\n", opcode);
            exit(EXIT_FAILURE);
        }
        break;

    case 0x1000:
        if (logging)
        {
            printf("Instruction Jump (1NNN)\n");
        }
        jump(chip8);
        break;

    case 0x2000:
        if (logging)
        {
            printf("Instruction Call Subroutine (2NNN)\n");
        }
        call_subroutine(chip8);
        break;

    case 0x3000:
        if (logging)
        {
            printf("Skip next instr Vx == kk (3XKK)\n");
        }
        se_Vx_kk(chip8);
        break;

    case 0x4000:
        if (logging)
        {
            printf("Skip next instr Vx != kk (4XKK)\n");
        }
        sne_Vx_kk(chip8);
        break;

    case 0x5000:
        if (logging)
        {
            printf("Skip next instr Vx == Vy (5XY0)\n");
        }
        se_Vx_Vy(chip8);
        break;

    case 0x6000:
        if (logging)
        {
            printf("Instruction Load Vx reg (6XKK)\n");
        }
        ld_Vx(chip8);
        break;

    case 0x7000:
        if (logging)
        {
            printf("Instruction ADD Vx reg immediate (7XKK)\n");
        }
        add_Vx_imm(chip8);
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            if (logging)
            {
                printf("Instruction Move Vy reg into Vx reg (8XY0)\n");
            }
            move_Vx_Vy(chip8);
            break;

        case 0x0001:
            if (logging)
            {
                printf("Instruction OR (8XY1)\n");
            }
            or_Vx_Vy(chip8);
            break;

        case 0x0002:
            if (logging)
            {
                printf("Instruction AND (8XY2)\n");
            }
            and_Vx_Vy(chip8);
            break;

        case 0x0003:
            if (logging)
            {
                printf("Instruction XOR (8XY3)\n");
            }
            xor_Vx_Vy(chip8);
            break;

        case 0x0004:
            if (logging)
            {
                printf("Instruction ADD VX VY (8XY4)\n");
            }
            add_Vx_Vy(chip8);
            break;

        case 0x0005:
            if (logging)
            {
                printf("Instruction SUB VX VY (8XY5)\n");
            }
            sub_Vx_Vy(chip8);
            break;

        case 0x0006:
            if (logging)
            {
                printf("Instruction SHR VX (8XY6)\n");
            }
            shr(chip8);
            break;

        case 0x0007:
            if (logging)
            {
                printf("Instruction SUBN VX VY (8XY7)\n");
            }
            subn_Vx_Vy(chip8);
            break;

        case 0x000E:
            if (logging)
            {
                printf("Instruction SHL VX (8XYE)\n");
            }
            shl(chip8);
            break;

        default:
            printf("ERROR: Unrecognized opcode 0x%X\n", opcode);
            exit(EXIT_FAILURE);
        }
        break;

    case 0x9000:
        if (logging)
        {
            printf("Skip next instr Vx != Vy (9XY0)\n");
        }
        sne_Vx_Vy(chip8);
        break;

    case 0xA000:
        if (logging)
        {
            printf("Instruction LDI (ANNN)\n");
        }
        ldi(chip8);
        break;

    case 0xB000:
        if (logging)
        {
            printf("Instruction JUMP + V0 (BNNN)\n");
        }
        jump_V0(chip8);
        break;

    case 0xC000:
        if (logging)
        {
            printf("Instruction RNG Vx (CXKK)\n");
        }
        rnd(chip8);
        break;

    case 0xD000:
        if (logging)
        {
            printf("Draw Sprite (DXYN)\n");
        }
        drw(chip8);
        break;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            if (logging)
            {
                printf("Instruction Skip next instr if key pressed (009E)\n");
            }
            skp(chip8);
            break;

        case 0x00A1:
            if (logging)
            {
                printf("Instruction Skip next instr if key not pressed (00A1)\n");
            }
            sknp(chip8);
            break;

        default:
            printf("ERROR: Unrecognized opcode 0x%X\n", opcode);
            exit(EXIT_FAILURE);
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            if (logging)
            {
                printf("Instruction Load VX with Delay Timer (0007)\n");
            }
            ld_Vx_dt(chip8);
            break;

        case 0x000A:
            if (logging)
            {
                printf("Instruction Wait for key press (000A)\n");
            }
            ld_Vx_k(chip8);
            break;

        case 0x0015:
            if (logging)
            {
                printf("Instruction Load Delay Timer with VX (0015)\n");
            }
            ld_dt_Vx(chip8);
            break;

        case 0x0018:
            if (logging)
            {
                printf("Instruction Load SOUND Timer with VX (0018)\n");
            }
            ld_st_Vx(chip8);
            break;

        case 0x001E:
            if (logging)
            {
                printf("Instruction ADD Index and Vx (001E)\n");
            }
            add_i_Vx(chip8);
            break;

        case 0x0029:
            if (logging)
            {
                printf("Instruction LOAD Font from VX value (0029)\n");
            }
            ld_F_Vx(chip8);
            break;

        case 0x0033:
            if (logging)
            {
                printf("Instruction STORE BCD of VX value (0033)\n");
            }
            st_bcd_Vx(chip8);
            break;

        case 0x0055:
            if (logging)
            {
                printf("Instruction STORE Regs V[0] - V[X] starting at I register (0055)\n");
            }
            st_V_regs(chip8);
            break;

        case 0x0065:
            if (logging)
            {
                printf("Instruction LOAD Regs V[0] - V[X] starting at I register (0065)\n");
            }
            ld_V_regs(chip8);
            break;

        default:
            printf("ERROR: Unrecognized opcode 0x%X\n", opcode);
            exit(EXIT_FAILURE);
        }
        break;

    default:
        printf("ERROR: Unrecognized opcode 0x%X\n", opcode);
        exit(EXIT_FAILURE);
    }
}

/*
 * Gets user input and updates the clavier key status based on what keys
 * were or were not pressed.
 *
 * Also checks for key presses that have other functionality in the emulator
 *   ESC: Exit Emulator
 *   Spacebar: Pause Emulator
 *   F5: Reset Emulator
 */
void process_user_input(Chip8 *chip8)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {

        // Check for keys that were pressed
        if (e.type == SDL_KEYDOWN)
        {

            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                chip8->executionFlag = FALSE;
                break;

            case SDLK_SPACE:
                if (chip8->pauseFlag)
                {
                    chip8->pauseFlag = FALSE;
                }
                else
                {
                    chip8->pauseFlag = TRUE;
                }
                break;

            case SDLK_F5:
                reset_system(chip8);
                break;

            default:
                break;
            }

            // updates each key state in the clavier array based on their pressed status (TRUE if pressed)
            for (int i = 0; i < NOMBRE_TOUCHE; i++)
            {
                if (e.key.keysym.sym == CLAVIER_MAP[i])
                {
                    chip8->clavier[i] = TRUE;
                }
            }
        }

        // checks for keys that were not pressed, updates their state in the clavier to FALSE
        if (e.type == SDL_KEYUP)
        {
            for (int i = 0; i < NOMBRE_TOUCHE; i++)
            {
                if (e.key.keysym.sym == CLAVIER_MAP[i])
                {
                    chip8->clavier[i] = FALSE;
                }
            }
        }

        // Checks for the 'x' button on the window to be pressed
        if (e.type == SDL_QUIT)
        {
            chip8->executionFlag = FALSE;
        }
    }
}

/*
 * Updates the system timers for the emulator
 *
 * If the compteurJeu or the compteurSon are > 0
 * the corrisponding timer is decremented by 1
 */
void update_timers(Chip8 *chip8)
{
    if (chip8->compteurJeu > 0)
    {
        chip8->compteurJeu--;
    }
    if (chip8->compteurSon > 0)
    {
        chip8->compteurSon--;
    }
}
