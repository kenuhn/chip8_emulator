#include "instructions.h"

/*
 * Opcode 00E0: Clear the display
 * Display (memory) is cleared
 */
void cls(Chip8 *chip8)
{
    for (int i = 0; i < ECRAN_HAUTEUR; i++)
    {
        for (int j = 0; j < ECRAN_LARGEUR; j++)
        {
            chip8->screen[i][j] = 0;
        }
    }
    chip8->screenFlag = TRUE;
    chip8->pc += 2;
}

/*
 * Opcode 00EE: Return from subroutine
 * pc popped from top of pile, pPile decremented
 */
void return_from_subroutine(Chip8 *chip8)
{
    chip8->pPile--;
    chip8->pc = chip8->pile[chip8->pPile];
    chip8->pc += 2;
}

/*
 * Opcode 1NNN: Jump to address NNN
 * pc set to nnn
 */
void jump(Chip8 *chip8)
{
    uint16_t nnn = chip8->opcodeCourant & 0x0FFF;

    chip8->pc = nnn;
}

/*
 * Opcode 2NNN: Call Subroutine at NNN
 * pPile incremented, pc pushed to pile, pc set to NNN
 */
void call_subroutine(Chip8 *chip8)
{
    uint16_t nnn = chip8->opcodeCourant & 0x0FFF;

    chip8->pile[chip8->pPile] = chip8->pc;
    chip8->pPile++;
    chip8->pc = nnn;
}

/*
 * Opcode 3XKK: Skip next instruction
 * Increments the pc by 4 (2 instructions) if  registre[x] == KK
 */
void se_Vx_kk(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t kk = chip8->opcodeCourant & 0x00FF;

    if (chip8->registre[x] == kk)
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode 4XKK: Skip next instruction
 * Increments the pc by 4 (2 instructions) if  registre[x] != KK
 */
void sne_Vx_kk(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t kk = chip8->opcodeCourant & 0x00FF;

    if (chip8->registre[x] != kk)
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode 5XY0: Skip next instruction
 * Increments the pc by 4 (2 instructions) if  registre[x] == registre[Y]
 */
void se_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    if (chip8->registre[x] == chip8->registre[y])
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode 6XKK: Load Register Vx immediate
 * Sets the registre[X] register to byte KK
 */
void ld_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t kk = chip8->opcodeCourant & 0x00FF;

    chip8->registre[x] = kk;
    chip8->pc += 2;
}

/*
 * Opcode 7XKK: ADD Register Vx immediate
 * Sets the registre[X] register to registre[X] + byte KK
 */
void add_Vx_imm(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t kk = chip8->opcodeCourant & 0x00FF;

    // TODO: What if there is overflow?
    chip8->registre[x] += kk;
    ;
    chip8->pc += 2;
}

/*
 * Opcode 8XY0: Load Vx, Vy
 * Sets register registre[X] to register registre[Y]
 */
void move_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    chip8->registre[x] = chip8->registre[y];
    chip8->pc += 2;
}

/*
 * Opcode 8XY1: OR Vx, Vy
 * registre[X] | registre[Y] result stored in registre[X]
 */
void or_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    chip8->registre[x] = (chip8->registre[x] | chip8->registre[y]);
    chip8->pc += 2;
}

/*
 * Opcode 8XY2: AND Vx, Vy
 * registre[X] & registre[Y] result stored in registre[X]
 */
void and_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    chip8->registre[x] = (chip8->registre[x] & chip8->registre[y]);
    chip8->pc += 2;
}

/*
 * Opcode 8XY3: XOR Vx, Vy
 * registre[X] ^ registre[Y] result stored in registre[X]
 */
void xor_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    chip8->registre[x] = (chip8->registre[x] ^ chip8->registre[y]);
    chip8->pc += 2;
}

/*
 * Opcode 8XY4: ADD Vx, Vy
 * registre[X] + registre[Y] stored in registre[X]
 * If the sum is over 255, registre[F] register (carry) is set to 1, else 0
 * Only the bottom 8 bits of the sum are stored in the registre[X] register
 */
void add_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;
    uint16_t sum = (chip8->registre[x] + chip8->registre[y]);

    if (sum > 255)
    {
        chip8->registre[0xF] = 1;
    }
    else
    {
        chip8->registre[0xF] = 0;
    }

    chip8->registre[x] = (sum & 0xFF);
    chip8->pc += 2;
}

/*
 * Opcode 8XY5: SUB Vx, Vy
 * registre[X] - registre[Y] stored in registre[X]
 * If registre[X] > registre[Y], registre[F] register (borrow) is set to 1, else 0 (NOT borrow essentially)
 */
void sub_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    if (chip8->registre[x] > chip8->registre[y])
    {
        chip8->registre[0xF] = 1;
    }
    else
    {
        chip8->registre[0xF] = 0;
    }

    chip8->registre[x] -= chip8->registre[y];
    chip8->pc += 2;
}

/*
 * Opcode 8XY6: SHR Vx
 * registre[X] = registre[X] >> 1
 * If LSb of registre[X] == 1, registre[F] register is set to 1, else 0
 */
void shr(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    // check if the LSb is 1 (odd num in registre[X] will have a LSB of 1)
    if (chip8->registre[x] % 2 == 1)
    {
        chip8->registre[0xF] = 1;
    }
    else
    {
        chip8->registre[0xF] = 0;
    }

    chip8->registre[x] = chip8->registre[x] >> 1;
    chip8->pc += 2;
}

/*
 * Opcode 8XY7: SUBN Vx, Vy
 * registre[Y] - registre[X] stored in registre[X]
 * If registre[Y] > registre[X], registre[F] register (borrow) is set to 1, else 0 (NOT borrow essentially)
 */
void subn_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;

    if (chip8->registre[y] > chip8->registre[x])
    {
        chip8->registre[0xF] = 1;
    }
    else
    {
        chip8->registre[0xF] = 0;
    }

    chip8->registre[x] = chip8->registre[y] - chip8->registre[x];
    chip8->pc += 2;
}

/*
 * Opcode 8XY6: SHL Vx
 * registre[X] = registre[X] << 1
 * If MSb of registre[X] == 1, registre[F] register is set to 1, else 0
 */
void shl(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    // check if the MSb is 1
    if ((chip8->registre[x] & 10000000) == 1)
    {
        chip8->registre[0xF] = 1;
    }
    else
    {
        chip8->registre[0xF] = 0;
    }

    chip8->registre[x] = chip8->registre[x] << 1;
    chip8->pc += 2;
}

/*
 * Opcode 9XY0: Skip next instruction
 * Increments the pc by 4 (2 instructions) if  registre[X] != registre[Y]
 */
void sne_Vx_Vy(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t target_y_reg = (chip8->opcodeCourant & 0x00F0) >> 4;

    if (chip8->registre[x] != chip8->registre[target_y_reg])
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode ANNN: Load I immediate
 * Sets the Index register to the value of NNN
 */
void ldi(Chip8 *chip8)
{
    uint16_t nnn = chip8->opcodeCourant & 0x0FFF;

    chip8->pIndex = nnn;
    chip8->pc += 2;
}

/*
 * Opcode BNNN: Jump + registre[0]
 * set pcister to NNN + registre[0]
 */
void jump_V0(Chip8 *chip8)
{
    uint16_t nnn = chip8->opcodeCourant & 0x0FFF;

    chip8->pc = (nnn + chip8->registre[0]);
}

/*
 * Opcode CXKK: Random
 * Generate Random Num between 0 - 255 then bitwise AND with value KK.
 * Store the result in registre[X]
 */
void rnd(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t kk = chip8->opcodeCourant & 0x00FF;
    uint8_t random_num = rand() % 256;

    chip8->registre[x] = random_num & kk;
    chip8->pc += 2;
}

/*
 * Opcode DXYN: Display N-byte sprite at ram[pIndex]
 * Draws sprite at location registre[X], registre[Y]. Set registre[F] if collision
 *
 * Initial source of implimentation used as template found below:
 * http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 */
void drw(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t y = (chip8->opcodeCourant & 0x00F0) >> 4;
    uint8_t sprite_height = chip8->opcodeCourant & 0x000F;
    uint8_t x_location = chip8->registre[x];
    uint8_t y_location = chip8->registre[y];
    uint8_t pixel;

    // Reset collision register to FALSE
    chip8->registre[0xF] = FALSE;
    for (int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++)
    {
        pixel = chip8->ram[chip8->pIndex + y_coordinate];
        for (int x_coordinate = 0; x_coordinate < 8; x_coordinate++)
        {
            if ((pixel & (0x80 >> x_coordinate)) != 0)
            {
                if (chip8->screen[y_location + y_coordinate][x_location + x_coordinate] == 1)
                {
                    chip8->registre[0xF] = TRUE;
                }
                chip8->screen[y_location + y_coordinate][x_location + x_coordinate] ^= 1;
            }
        }
    }

    chip8->screenFlag = TRUE;
    chip8->pc += 2;
}

/*
 * Opcode EX9E: Skip next instruction if key pressed
 * Skips the next instruction if the key with value registre[X] is pressed
 */
void skp(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t vX_value = chip8->registre[x];

    if (chip8->clavier[vX_value] != FALSE)
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode EXA1: Skip next instruction if key not pressed
 * Skips the next instruction if the key with value registre[X] is not pressed
 */
void sknp(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;
    uint8_t vX_value = chip8->registre[x];

    if (chip8->clavier[vX_value] == FALSE)
    {
        chip8->pc += 4;
    }
    else
    {
        chip8->pc += 2;
    }
}

/*
 * Opcode FX07: Load Vx, DT
 * registre[X] set to value in compteurJeu
 */
void ld_Vx_dt(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->registre[x] = chip8->compteurJeu;
    chip8->pc += 2;
}

/*
 * Opcode FX0A: Load Vx, K
 * Waits for a key press
 * registre[X] set to value of key (K) pressed
 */
void ld_Vx_k(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->clavierFlag = FALSE;

    for (int i = 0; i < NOMBRE_TOUCHE; i++)
    {
        if (chip8->clavier[i] != FALSE)
        {
            chip8->registre[x] = i;
            chip8->clavierFlag = TRUE;
        }
    }

    if (!chip8->clavierFlag)
    {
        return;
    }

    chip8->pc += 2;
}

/*
 * Opcode FX15: Load DT, Vx
 * compteurJeu set to value registre[X]
 */
void ld_dt_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->compteurJeu = chip8->registre[x];
    chip8->pc += 2;
}

/*
 * Opcode FX18: Load ST, Vx
 * compteurSon set to value registre[X]
 */
void ld_st_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->compteurSon = chip8->registre[x];
    chip8->pc += 2;
}

/*
 * Opcode FX1E: Add I, VX
 * Adds current pIndex and registre[X], result stored in pIndex
 */
void add_i_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->pIndex = chip8->pIndex + chip8->registre[x];
    chip8->pc += 2;
}

/*
 * Opcode FX29: LD F, VX
 * pIndex set to value of location of hex sprite value in registre[X] * 5
 */
void ld_F_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->pIndex = (chip8->registre[x] * 0x5);
    chip8->pc += 2;
}

/*
 * Opcode FX33: LD B, VX
 * Store the Binary Coded Decimal representation of registre[X]
 * at memory location I (hundreds place) I+1 (tens place) and I+2 (ones place)
 *
 * Used this for page for a hint on this op implementation:
 * http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 */
void st_bcd_Vx(Chip8 *chip8)
{
    uint8_t x = (chip8->opcodeCourant & 0x0F00) >> 8;

    chip8->ram[chip8->pIndex] = chip8->registre[x] / 100; // MSb
    chip8->ram[chip8->pIndex + 1] = (chip8->registre[x] / 10) % 10;
    chip8->ram[chip8->pIndex + 2] = (chip8->registre[x] % 100) % 10; // LSb
    chip8->pc += 2;
}

/*
 * Opcode FX55: LD [I], Vx
 * Store registre[0] - registre[X] in memory starting at pIndex value
 */
void st_V_regs(Chip8 *chip8)
{
    uint8_t end_ld_v_reg = (chip8->opcodeCourant & 0x0F00) >> 8;

    for (int i = 0; i <= end_ld_v_reg; i++)
    {
        chip8->ram[chip8->pIndex + i] = chip8->registre[i];
    }

    // TODO: Does pIndex need to change?
    chip8->pIndex += (end_ld_v_reg + 1);

    chip8->pc += 2;
}

/*
 * Opcode FX65: LD Vx, I
 * Read values into registre[0] - registre[X] from memory starting at pIndex value
 */
void ld_V_regs(Chip8 *chip8)
{
    uint8_t end_ld_v_reg = (chip8->opcodeCourant & 0x0F00) >> 8;

    for (int i = 0; i <= end_ld_v_reg; i++)
    {
        chip8->registre[i] = chip8->ram[chip8->pIndex + i];
    }

    // TODO: Does pIndex need to change?
    chip8->pIndex += (end_ld_v_reg + 1);

    chip8->pc += 2;
}
