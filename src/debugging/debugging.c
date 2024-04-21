#include "debugging.h"

/*****************************
 * Debugging functions below
 ******************************/

void print_regs(Chip8 *chip8)
{
  // print registers
  printf("Current OP: 0x%X\n", chip8->opcodeCourant);
  for (int i = 0; i < NOMBRE_REGISTRE; i++)
  {
    printf("registre Reg %X: 0x%X\n", i, chip8->registre[i]);
  }
  printf("Index Reg: 0x%X\n", chip8->pIndex);
  printf("PC Reg: 0x%X\n", chip8->pc);
  printf("SP Reg: 0x%X\n", chip8->pPile);
  printf("Delay Timer Reg: 0x%X\n", chip8->compteurJeu);
  printf("Sound Timer Reg: 0x%X\n", chip8->compteurSon);
  printf("\n");
}

void print_pile(Chip8 *chip8)
{
  // print pile
  for (int i = 0; i < TAILLE_PILE; i++)
  {
    printf("Stack Element %i: 0x%X\n", i, chip8->pile[i]);
  }
}

void print_clavier(Chip8 *chip8)
{
  // print clavier
  for (int i = 0; i < NOMBRE_TOUCHE; i++)
  {
    printf("Keyboard Key %X: %i\n", i, chip8->clavier[i]);
  }
}