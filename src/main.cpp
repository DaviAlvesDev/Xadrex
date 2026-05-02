#include <iostream>
#include <vector>
#include <cstdlib>
#include <raylib.h>
#include "config.hpp"

struct PosicaoCasa
{
    uint8 coluna = 'a';
    uint8 linha = '1';

    bool operator==(PosicaoCasa a)
    {
        return this->coluna == a.coluna && this->linha == a.linha;
    }
};

struct Peca
{
    bool is_branca = true;
    uint8 tipo = PEAO;
    Vector2  pos = {0, 0};
    PosicaoCasa pos_casa = {};
    Texture2D textura;
    bool is_selecionada = false;
    bool is_capturada = false;
    uint8 moves_quant = 0;
    std::vector<PosicaoCasa> legal_moves;

    void print_legal_moves()
    {
        std::cout << this->tipo << " em " << pos_casa.coluna << pos_casa.linha << ": ";
        for (int i = 0, n = this->legal_moves.size(); i < n; i++)
        {
            std::cout << this->legal_moves[i].coluna << this->legal_moves[i].linha << ", ";
        }
        std::cout << std::endl;
    }
};

struct Casa 
{
    PosicaoCasa pos_casa = {};
    Vector2 pos = {0, 0};
    Color cor = WHITE;
};

Peca* passant = NULL;
Peca pecas[32];
Casa tabuleiro[8][8];
uint8 tipo_pecas[] = {TORRE, CAVALO, BISPO, DAMA, REI, PEAO};
bool is_peca_selecionada = false;
bool is_brancas_turno = true;

void  desenhar_tabuleiro();
void desenhar_pecas();
void posicionar_pecas();
void criar_pecas();
void criar_tabuleiro();
Texture2D carregar_textura(uint8 tipo);
void descarregar();
PosicaoCasa posicao_mouse();
void lidar_com_click();
void selecionar_peca(PosicaoCasa casa);
Peca* procurar_peca(PosicaoCasa casa);
void capturar_peca(Peca &peca);
void mover_peca(Peca &peca, PosicaoCasa casa);
void calcular_legal_moves();
void calcular_lmoves_peao(Peca &peca);
void calcular_lmoves_torre(Peca &peca);
void calcular_lmoves_cavalo(Peca &peca);
void calcular_lmoves_bispo(Peca &peca);
bool en_passant(PosicaoCasa casa);
bool posicao_legal(PosicaoCasa casa);

int main()
{
    InitWindow(LARGURA, ALTURA, NOME);
    SetTargetFPS(FPS);

    criar_pecas();
    criar_tabuleiro();
    posicionar_pecas();
    calcular_legal_moves();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        desenhar_tabuleiro();
        desenhar_pecas();

        lidar_com_click();

        EndDrawing();
    }

    descarregar();

    CloseWindow();
}

PosicaoCasa posicao_mouse()
{
    Vector2 mouse = GetMousePosition();

    uint8 coluna = (mouse.x - offset_x) / CASA_DIMEN;
    uint8 linha = (mouse.y - offset_y) / CASA_DIMEN;

    PosicaoCasa pos_mouse;
    pos_mouse.coluna = coluna + 'a';
    pos_mouse.linha = '8' - linha;

    return pos_mouse;
}

void selecionar_peca(PosicaoCasa casa)
{
    for (int i = 0; i  < 32; i++)
    {
        if (pecas[i].pos_casa == casa) 
        {
            if (pecas[i].is_branca && !is_brancas_turno) return;
            else if (!pecas[i].is_branca && is_brancas_turno) return;

            pecas[i].is_selecionada = true;
            is_peca_selecionada = true;
            return;
        }
    }
}

void mover_peca(Peca &peca, PosicaoCasa casa)
{
    bool is_legal = false;
    for (int i = 0, n = peca.legal_moves.size(); i < n; i++)
    {
        if (peca.legal_moves[i] == casa) 
        {
            is_legal = true;
            break;
        }
    }

    if (!is_legal) 
    {
        is_peca_selecionada = false;
        peca.is_selecionada = false;

        selecionar_peca(casa);
        return;
    }

    Peca *capturada = procurar_peca(casa);
    if (capturada) capturar_peca(*capturada);

    if (peca.tipo == PEAO)
    {
        if (passant && casa.coluna != peca.pos_casa.coluna) capturar_peca(*passant);

        if (std::abs(casa.linha - peca.pos_casa.linha) == 2) passant = &peca;
    }

    else passant = NULL;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (tabuleiro[i][j].pos_casa == casa)
            {
                peca.pos.x = tabuleiro[i][j].pos.x + PECA_OFFSET;
                peca.pos.y = tabuleiro[i][j].pos.y + PECA_OFFSET;
                
                peca.pos_casa = casa; 
                
                peca.is_selecionada = false;
                is_peca_selecionada = false;

                is_brancas_turno = !is_brancas_turno;
                peca.moves_quant++;
                calcular_legal_moves();
                return;
            }
        }
    }
}

void lidar_com_click()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        PosicaoCasa mouse = posicao_mouse();

        if (mouse.coluna >= 'a' && mouse.coluna <= 'h' && mouse.linha >= '1' && mouse.linha <= '8')
        {
            TraceLog(LOG_INFO, "Casa: %c%c", mouse.coluna, mouse.linha);

            if (!is_peca_selecionada) selecionar_peca(mouse);
            else 
            {
                for (int i = 0; i < 32; i++)
                {
                    if (pecas[i].is_selecionada) 
                    {
                        mover_peca(pecas[i], mouse);
                        break;
                    }
                }
            } 
        }
    }
}

void descarregar()
{
    for (int i = 0; i < 32; i++)
    {
        UnloadTexture(pecas[i].textura);
    }
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
    float x = offset_x;
    float y = offset_y;

    for (int i = 0; i < 8; i++) // Linhas (0 a 7)
    {
        for (int j = 0; j < 8; j++) // Colunas (0 a 7)
        {
            // Define a cor alternada
            Color cor = ((i + j) % 2 == 0) ? casaBranca : casaPreta;

            // Coordenadas lógicas:
            // Coluna: 'a' + j (0='a', 1='b'...)
            // Linha:  '8' - i (0='8', 1='7'...) -> Para o topo ser a linha 8
            uint8 col_char = 'a' + j;
            uint8 lin_char = '8' - i;

            tabuleiro[j][i].pos_casa = {col_char, lin_char};
            tabuleiro[j][i].pos = { x, y };
            tabuleiro[j][i].cor = cor;

            x += CASA_DIMEN;
        }
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
                pecas[j+8] = {true, PEAO, {}, {}, carregar_textura(PEAO)};
                pecas[j+8+16] = {false, PEAO, {}, {}, carregar_textura(PEAO)};
            }
            continue;
        }

        if (i > 2)
        {
            pecas[i] = {true, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};
            pecas[i+16] = {false, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};
            continue;
        }

        pecas[i] = {true, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};
        pecas[7-i] = {true, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};

        pecas[i+16] = {false, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};
        pecas[7-i+16] = {false, tipo_pecas[i], {}, {}, carregar_textura(tipo_pecas[i])};
    }
}

void posicionar_pecas()
{
    // Brancas nas linhas 0 e 1 do array (Linhas 8 e 7 do xadrez)
    // Pretas nas linhas 7 e 6 do array (Linhas 1 e 2 do xadrez)
    
    for (int i = 0; i < 32; i++)
    {
        int col_idx = i % 8;
        int lin_idx;

        // Define a linha do array tabuleiro[col][lin] baseada no índice da peça
        if (i < 8) lin_idx = 7;         // Brancas (Fundo)
        else if (i < 16) lin_idx = 6;    // Peões Brancos
        else if (i < 24) lin_idx = 0;    // Pretas (fundo)
        else lin_idx = 1;               // Peões Pretos

        // Copia do tabuleiro para a peça
        pecas[i].pos.x = tabuleiro[col_idx][lin_idx].pos.x + PECA_OFFSET;
        pecas[i].pos.y = tabuleiro[col_idx][lin_idx].pos.y + PECA_OFFSET;
        pecas[i].pos_casa = tabuleiro[col_idx][lin_idx].pos_casa;
    }
}

void desenhar_pecas()
{
    for (int i = 0; i < 32; i++)
    {
        if (pecas[i].is_capturada) continue;
        Color cor;
        if (pecas[i].is_branca) cor = corBrancas;
        else cor = corPretas;

        if (pecas[i].is_selecionada) {
            DrawCircleV({pecas[i].pos.x + PECA_DIMEN/2, pecas[i].pos.y + PECA_DIMEN/2}, 30, Fade(YELLOW, 0.6f));
        }

        DrawTextureV(pecas[i].textura, pecas[i].pos, cor);
    }
}

Peca* procurar_peca(PosicaoCasa casa)
{
    for (int i = 0; i < 32; i++)
    {
        if (pecas[i].pos_casa == casa) return &pecas[i];
    }

    return NULL;
}

void calcular_legal_moves()
{
    for (int i = 0; i < 32; i++)
    {
        if (pecas[i].is_capturada) continue;

        pecas[i].legal_moves.clear();

        switch (pecas[i].tipo)
        {
        case PEAO:
            calcular_lmoves_peao(pecas[i]);
            break;
            
        case TORRE:
            calcular_lmoves_torre(pecas[i]);
            break;
            
        case CAVALO:
            calcular_lmoves_cavalo(pecas[i]);
            break;

        case BISPO:
            calcular_lmoves_bispo(pecas[i]);
            pecas[i].print_legal_moves();
            break;    

        default:
            break;
        }
    }
}

bool posicao_legal(PosicaoCasa casa)
{
    return (casa.coluna >= 'a' && casa.coluna <= 'h') && (casa.linha >= '1' && casa.linha <= '8');
}

void calcular_lmoves_peao(Peca &peca)
{
    if (peca.tipo != PEAO) return;

    uint8 direcao = peca.is_branca ? 1 : -1;
    
    {
        PosicaoCasa legal_move = peca.pos_casa;
        
        legal_move.linha += direcao;
        Peca* alvo = procurar_peca(legal_move);
        
        if (!alvo) 
        {
            peca.legal_moves.push_back(legal_move);
            
            legal_move.linha += direcao;
            alvo = procurar_peca(legal_move);
            if (!alvo) peca.legal_moves.push_back(legal_move);
        }
    }
    
    {
        int diagonais[] = {1, -1};
        
        for (int i = 0; i < 2; i++)
        {
            PosicaoCasa legal_move = peca.pos_casa;
            legal_move.linha += direcao;
            legal_move.coluna += diagonais[i];

            Peca* alvo = procurar_peca(legal_move);
            if (alvo && (alvo->is_branca ^ peca.is_branca)) peca.legal_moves.push_back(legal_move);

            if (passant && (passant->is_branca ^ peca.is_branca) && en_passant(legal_move)) peca.legal_moves.push_back(legal_move);
        }
    }
    
}

bool en_passant(PosicaoCasa casa)
{
    if (!passant) return false;

    PosicaoCasa tmp = passant->pos_casa;

    if (passant->is_branca) tmp.linha--;
    else tmp.linha++;

    return tmp == casa;
}

void calcular_lmoves_torre(Peca &peca)
{
    if (peca.tipo != TORRE) return;
    int direcoes[] = {1, -1};

    for (int i = 0; i < 2; i++) 
    {
        PosicaoCasa legal_move = peca.pos_casa;
        legal_move.linha += direcoes[i];
        for (; procurar_peca(legal_move) == NULL && posicao_legal(legal_move); legal_move.linha += direcoes[i])
        {
            peca.legal_moves.push_back(legal_move);
        }

        Peca* alvo = procurar_peca(legal_move);
        if (alvo && (alvo->is_branca ^ peca.is_branca)) peca.legal_moves.push_back(legal_move);
    }

    for (int i = 0; i < 2; i++) 
    {
        PosicaoCasa legal_move = peca.pos_casa;
        legal_move.coluna += direcoes[i];
        for (; procurar_peca(legal_move) == NULL && posicao_legal(legal_move); legal_move.coluna += direcoes[i])
        {
            peca.legal_moves.push_back(legal_move);
        }

        Peca* alvo = procurar_peca(legal_move);
        if (alvo && (alvo->is_branca ^ peca.is_branca)) peca.legal_moves.push_back(legal_move);
    }

}

void calcular_lmoves_cavalo(Peca &peca)
{
    if (peca.tipo != CAVALO) return;
    int direcoes[2][2] = {{2, -2}, {1, -1}};

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            PosicaoCasa legal_move_1 = peca.pos_casa;
            PosicaoCasa legal_move_2 = peca.pos_casa;

            legal_move_1.coluna += direcoes[0][i];
            legal_move_1.linha += direcoes[1][j];
            
            legal_move_2.coluna += direcoes[1][i];
            legal_move_2.linha += direcoes[0][j];

            Peca* alvo_1 = procurar_peca(legal_move_1);
            Peca* alvo_2 = procurar_peca(legal_move_2);

            if (!alvo_1 || (alvo_1 && (alvo_1->is_branca ^ peca.is_branca))) 
            {
                if (posicao_legal(legal_move_1)) peca.legal_moves.push_back(legal_move_1);
            }

            if (!alvo_2 || (alvo_2 && (alvo_2->is_branca ^ peca.is_branca))) 
            {
                if (posicao_legal(legal_move_2)) peca.legal_moves.push_back(legal_move_2);
            }
        }
    }
}

void calcular_lmoves_bispo(Peca &peca)
{
    if (peca.tipo != BISPO) return;
    int direcoes[] = {1, -1};

    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 2; j++)
        {    
            PosicaoCasa legal_move = peca.pos_casa;
            legal_move.linha += direcoes[i];
            legal_move.coluna += direcoes[j];
            for (; procurar_peca(legal_move) == NULL && posicao_legal(legal_move); legal_move.linha += direcoes[i], legal_move.coluna += direcoes[j])
            {
                peca.legal_moves.push_back(legal_move);
            }

            Peca* alvo = procurar_peca(legal_move);
            if (alvo && (alvo->is_branca ^ peca.is_branca)) peca.legal_moves.push_back(legal_move);
        }
    }
}

void capturar_peca(Peca &peca)
{
    peca.is_capturada = true;
    peca.pos = {0, 0};
    peca.pos_casa = {0, 0};
}

