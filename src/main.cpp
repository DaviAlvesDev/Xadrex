#include <raylib.h>
#include "config.hpp"

struct Peca
{
    bool is_branca = true;
    uint8 tipo = PEAO;
    Vector2  pos = {0, 0};
    uint8 coluna = 'a';
    uint8 linha = '1';
    Texture2D textura;
};

struct Casa 
{
    uint8 coluna = 'a';
    uint8 linha = '1';
    Vector2 pos = {0, 0};
    Color cor = WHITE;
};

Peca pecas[32];
Casa tabuleiro[8][8];
uint8 tipo_pecas[] = {TORRE, CAVALO, BISPO, DAMA, REI, PEAO};

void  desenhar_tabuleiro();
void desenhar_pecas();
void posicionar_pecas();
void criar_pecas();
void criar_tabuleiro();
Texture2D carregar_textura(uint8 tipo);

int main()
{
    InitWindow(LARGURA, ALTURA, NOME);
    SetTargetFPS(FPS);

    criar_pecas();
    criar_tabuleiro();
    posicionar_pecas();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        desenhar_tabuleiro();
        desenhar_pecas();

        EndDrawing();
    }

    CloseWindow();
}

Texture2D carregar_textura(uint8 tipo)
{
    Texture2D textura;
    Image img;

    switch (tipo)
    {
    case PEAO:
        img = LoadImage("./assets/peao.png");
        break;

    case TORRE:
        img = LoadImage("./assets/torre.png");
        break;

    case CAVALO:
        img = LoadImage("./assets/cavalo.png");
        break;

    case BISPO:
        img = LoadImage("./assets/bispo.png");
        break;

    case DAMA:
        img = LoadImage("./assets/dama.png");
        break;

    case REI:
        img = LoadImage("./assets/rei.png");
        break;
    
    default:
        break;
    }

    ImageResize(&img, PECA_DIMEN, PECA_DIMEN);
    textura = LoadTextureFromImage(img);
    UnloadImage(img);

    return textura;
}

void  desenhar_tabuleiro()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            DrawRectangleV(tabuleiro[i][j].pos, {(float) CASA_DIMEN, (float) CASA_DIMEN}, tabuleiro[i][j].cor);
        }
    }
}

void criar_tabuleiro()
{
    uint32 x = offset_x;
    uint32 y = offset_y;
    uint8 c = 'h';
    uint8 l = '1';
    bool branco = false;
    for (int i = 7; i >= 0; i--)
    {
        for (int j = 0; j < 8; j++)
        {
            branco = !branco;
            Color cor;

            if (branco) cor = casaBranca;
            else cor = casaPreta;

            uint8 coluna = c + 'a';
            uint8 linha = l + '1';

            tabuleiro[j][i] = {coluna, linha, {(float) x, (float) y}, cor};
            c--;
            x += CASA_DIMEN;
        }
        c = 'h';
        l++;

        branco = !branco;
        x = offset_x;
        y += CASA_DIMEN;
    }
}

void criar_pecas()
{
    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
        {
            for (int j = 0; j < 8; j++)
            {
                pecas[j+8] = {true, PEAO, {}, 0, 0, carregar_textura(PEAO)};
                pecas[j+8+16] = {false, PEAO, {}, 0, 0, carregar_textura(PEAO)};
            }
            continue;
        }

        if (i > 2)
        {
            pecas[i] = {true, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};
            pecas[i+16] = {false, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};
            continue;
        }

        pecas[i] = {true, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};
        pecas[7-i] = {true, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};

        pecas[i+16] = {false, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};
        pecas[7-i+16] = {false, tipo_pecas[i], {}, 0, 0, carregar_textura(tipo_pecas[i])};
    }
}

void posicionar_pecas()
{
    uint8 c = 0;
    for (int i = 0; i < 32; i++)
    {
        if (c > 7) c = 0;

        uint8 l = pecas[i].is_branca ? 0 : 7;
        if (i > 7 && i < 16) l++;
        else if (i > 23) l--;

        pecas[i].pos.x = tabuleiro[c][l].pos.x + PECA_OFFSET;
        pecas[i].pos.y = tabuleiro[c][l].pos.y + PECA_OFFSET;

        pecas[i].coluna = tabuleiro[c][l].coluna;
        pecas[i].linha = tabuleiro[c][l].linha;

        c++;
    }
}

void desenhar_pecas()
{
    for (int i = 0; i < 32; i++)
    {
        Color cor;
        if (pecas[i].is_branca) cor = corBrancas;
        else cor = corPretas;

        DrawTextureV(pecas[i].textura, pecas[i].pos, cor);
    }
}