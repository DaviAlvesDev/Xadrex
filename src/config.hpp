#ifndef CONFIG_H

#define CONFIG_H

#include <cmath>

#define uint8 unsigned char
#define uint32 unsigned int

#define NOME "Xadrex"

#define ALTURA 700
#define LARGURA 600
#define FPS 60

uint8 CASA_DIMEN = LARGURA < ALTURA ? LARGURA / 8 : ALTURA / 8;

uint32 offset_x = LARGURA > ALTURA ? (LARGURA - ALTURA) / 2 : 0;
uint32 offset_y = ALTURA > LARGURA ? (ALTURA - LARGURA) / 2 : 0;

Color casaBranca = { 225, 206, 154, 255 };
Color casaPreta = GREEN;

const uint32 PECA_DIMEN = std::round(CASA_DIMEN * 0.8);
const uint32 PECA_OFFSET = (CASA_DIMEN - PECA_DIMEN) / 2;

#define PEAO 'P'
#define TORRE 'T'
#define CAVALO 'C'
#define BISPO 'B'
#define DAMA 'D'
#define REI 'R'

Color corBrancas = WHITE;
Color corPretas = DARKGRAY;

#endif