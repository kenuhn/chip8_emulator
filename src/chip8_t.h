#ifndef CHIP8_T_H
#define CHIP8_T_H
#include <SDL2/SDL.h>
/*
 *
 * Definition of the chip8 struct and some constants
 * to be used by the program.
 *
 */

#define NOMBRE_TOUCHE 16
#define NOMBRE_REGISTRE 16
#define TOTAL_RAM 4096
#define TAILLE_PILE 16
#define TAILLE_SPRITES 80
#define PC_START 0x200
#define TIMER_MAX 255

#define CHIP8_RAM_START_ADDR 0x000
#define CHIP8_RAM_END_ADDR 0x1FF
#define PROGRAM_START_ADDR 0x200
#define PROGRAM_END_ADDR 0xFFF

#define ECRAN_LARGEUR 64
#define ECRAN_HAUTEUR 32

#define TRUE 1
#define FALSE 0

typedef struct Chip8_t Chip8;

const static uint8_t SPRITES[TAILLE_SPRITES] = {
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

struct Chip8_t
{
    uint8_t ram[TOTAL_RAM];     // 4k of memory+*
    uint16_t pile[TAILLE_PILE]; // pile, stores up to 16 levels

    // registers
    uint8_t registre[NOMBRE_REGISTRE]; // general purpose registers (0 - 14) and the carry flag register (15)
    uint16_t pIndex;                   // index register
    uint16_t pc;                       // pc register
    uint16_t pPile;                    // pile pointer register

    // timers
    uint8_t compteurJeu;
    uint8_t compteurSon;

    uint16_t opcodeCourant; // current opcode being executed by the system

    // screen
    uint8_t screen[ECRAN_HAUTEUR][ECRAN_LARGEUR];

    // keys (16)
    uint8_t clavier[NOMBRE_TOUCHE];
    uint8_t clavierFlag;

    // Status flags for the emulator
    uint8_t executionFlag;
    uint8_t screenFlag;
    uint8_t pauseFlag;
};

#endif // CHIP8_T_H