
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define VOLUME 0.5f
#define quantOpcoes 4
#define gravidade 500.0f
#define pulo -450.0f
#define velocidadePlayer 400.0f
#define velocidadeBala 500.0f
#define velocidadeInimigo 160.0f
#define velocidadeBoss 100.0f
#define quantRecordes 10
#define maxBalas 15
#define maxBalasInimigo 100
#define animacaoFPS 12.0f
#define VelocidadeQuadros (1.0f / animacaoFPS)

typedef enum {
    ANIM_IDLE,
    ANIM_WALKING,
    ANIM_JUMPING
} PlayerState;
    
typedef struct {
    Texture2D texture;    
    float larguraFrame;     
    int maxFrames;        
    int frameAtual;     
    float tempoFrame;     
    bool virado;         
} Animation;

typedef struct {
    Texture2D texture;    
    float larguraFrame;     
    int maxFrames;        
    int frameAtual;     
    float tempoFrame;     
    bool virado;
    Vector2 position;     
    Rectangle hitbox;     
    PlayerState state; 
    float escala;        
} EnemyAnimation;

typedef enum {
    MENU = 0,   
    RANKING,
    JOGO,
    SAIR
} tela;

typedef struct pontuacao{
    char name[30];
    int score;
} pontuacao;

typedef struct Bala {
    Rectangle rect;
    Vector2 speed;
    bool ativa;
} Bala;

typedef struct Sala {
    int id;
    struct Sala* esquerda;
    struct Sala* direita;
    Texture2D background;
    Rectangle chao;
    Rectangle plataforma[2];
    Rectangle teto;
    Rectangle enemy;
    bool inimigoVivo;
    int vidaInimigo;
    Bala balasInimigo[maxBalasInimigo];
    EnemyAnimation* enemyAnim;
} Sala;

const int largura = 1920;
const int altura = 1080;
char nomeJogador[30] = "";
int index = 0;
pontuacao ranking[quantRecordes];
Animation * currentAnim = NULL;
PlayerState currentState = ANIM_IDLE;

int jogo(void);
tela Menu(void);
void salvarPontos(const char *name, int score);
int contScore(pontuacao *ranking);
EnemyAnimation* initEnemyAnimation(void);
Sala* criarSala(int id);
void freeSala(Sala* sala);
void vitoria();
void desenharInimigo(Sala* salaAtual);


int main(void) {
    InitWindow(largura, altura, "Nier: 2D");
    SetTargetFPS(60);
    tela telaAtual = MENU;
    while (telaAtual == MENU) {
        telaAtual = Menu();
    }

    if (telaAtual == SAIR) {
        CloseWindow();
        return 0;
    }
    
    int score = jogo();
    
    salvarPontos(nomeJogador, score);

    CloseWindow();
    return 0;
}

int jogo(void) {
    InitAudioDevice();
    Music lvlMusic;
    Music musicBoss = LoadMusicStream("./music/bossFight.mp3");
    Music music = LoadMusicStream("./music/amoessamusica.mp3");
    Sound victory = LoadSound("./sounds/fimFase.mp3");
    Sound defeat = LoadSound("./sounds/lego-yoda-death-sound-effect.mp3");
    Sound enemyMage = LoadSound("./sounds/Retro Event 19.wav");
    Sound enemyHowl = LoadSound("./sounds/Robot2.mp3");
    Sound playerShoot = LoadSound("./sounds/tiro.mp3");
    SetSoundVolume(playerShoot, VOLUME);
    SetSoundVolume(enemyHowl,1.0f);
    InitWindow(largura, altura, "Nier: 2D");
    
    Sala* sala1 = criarSala(1);
    Sala* sala2 = criarSala(2);
    Sala* sala3 = criarSala(3);
    Sala* sala4 = criarSala(4);
    Sala* sala5 = criarSala(5);
    sala1->direita = sala2;
    sala2->esquerda = sala1;
    sala2->direita = sala3;
    sala3->esquerda = sala2;
    sala3->direita = sala4;
    sala4->esquerda = sala3;
    sala4->direita = sala5;
    sala5->esquerda = sala4;

    Sala* salaAtual = sala1;


    Rectangle player = {100, altura - 250, 60, 90};
    Vector2 playerSpeed = {0, 0};
    int enemyDiedCount = 0;
    int score;
    bool isGrounded = false;
    int playerLife = 5;

    Bala balas[maxBalas] = {0};
    for (int i = 0; i < maxBalas; i++) balas[i].ativa = false;

    srand(time(NULL));
    SetTargetFPS(60);
    bool movingRight = true;
    double countdownTime = 600.0;
    
    PlayMusicStream(music);
    
    lvlMusic = music;
    
    Animation idleAnim = {
        LoadTexture("./assets/Character/Idle.png"),
        0, 0, 0, 0.0f, false
    };
    idleAnim.larguraFrame = (float)(idleAnim.texture.width / 4);
    idleAnim.maxFrames = 4;
    
    currentAnim = &idleAnim;

    Animation walkAnim = {
        LoadTexture("./assets/Character/Walk.png"),
        0, 0, 0, 0.0f, false
    };
    walkAnim.larguraFrame = (float)(walkAnim.texture.width / 6);
    walkAnim.maxFrames = 6;

    Animation jumpAnim = {
        LoadTexture("./assets/Character/Jump.png"),
        0, 0, 0, 0.0f, false
    };
    jumpAnim.larguraFrame = (float)(jumpAnim.texture.width / 4);
    jumpAnim.maxFrames = 4;
    
    Animation enemyAnim = {
        LoadTexture("./assets/Walk.png"),
        0, 0, 0, 0.0f, false
    };
    enemyAnim.larguraFrame = (float)(enemyAnim.texture.width / 8);
    enemyAnim.maxFrames = 8;

    while (!WindowShouldClose()) {
        UpdateMusicStream(lvlMusic);
        
        if (playerLife <= 0) {
        StopMusicStream(lvlMusic);
        PlaySound(defeat);
        ClearBackground(BLACK);
        BeginDrawing();
        
        const char *msgGameOver = "GAME OVER";
        int textWidth = MeasureText(msgGameOver, 30);
        DrawText(msgGameOver, (1920 - textWidth) / 2, 1080 / 2, 30, RED);

        EndDrawing();
        sleep(3);
        break;
    }
        
         countdownTime -= GetFrameTime();

         if (countdownTime <= 0) {
            StopMusicStream(lvlMusic);
            PlaySound(defeat);
            ClearBackground(BLACK);
            BeginDrawing();

            const char *msgTempoEsgotado = "TEMPO ESGOTADO";
            int textWidth = MeasureText(msgTempoEsgotado, 30);
            DrawText(msgTempoEsgotado, (1920 - textWidth) / 2, (1080 / 2) - 50, 30, RED);

            EndDrawing();
            sleep(3);
            break;
        }
        
        if (salaAtual->inimigoVivo == false && salaAtual == sala5){
            StopMusicStream(lvlMusic);
            PlaySound(victory);
            ClearBackground(RAYWHITE);
            BeginDrawing();
            DrawText("VITÓRIA!", largura / 2 - 100, altura / 2, 30, GREEN);
            EndDrawing();
            sleep(3);
            break;
        }
        
        PlayerState newState = ANIM_IDLE;
        
        if (!isGrounded) {
            newState = ANIM_JUMPING;
            currentAnim = &jumpAnim;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A)) {
            newState = ANIM_WALKING;
            currentAnim = &walkAnim;
        } else {
            newState = ANIM_IDLE;
            currentAnim = &idleAnim;
        }

        if (IsKeyDown(KEY_D)){
            player.x += velocidadePlayer * GetFrameTime();
            currentAnim->virado = false;
        }
        if (IsKeyDown(KEY_A)){ 
            player.x -= velocidadePlayer * GetFrameTime();
            currentAnim->virado = true;
        }

        if (IsKeyPressed(KEY_W) && isGrounded) {
            playerSpeed.y = pulo;
            isGrounded = false;
        }
        
        if (newState != currentState) {
            currentState = newState;
            currentAnim->frameAtual = 0;
            currentAnim->tempoFrame = 0.0f;
        }
        
        currentAnim->tempoFrame += GetFrameTime();
        if (currentAnim->tempoFrame >= VelocidadeQuadros) {
            currentAnim->tempoFrame = 0.0f;
            currentAnim->frameAtual++;
            if (currentAnim->frameAtual >= currentAnim->maxFrames) {
                currentAnim->frameAtual = 0;
            }
        }
        
        enemyAnim.tempoFrame += GetFrameTime();
        if (enemyAnim.tempoFrame >= VelocidadeQuadros) {
            enemyAnim.tempoFrame = 0.0f;
            enemyAnim.frameAtual++;
            if (enemyAnim.frameAtual >= enemyAnim.maxFrames) {
                enemyAnim.frameAtual = 0;
            }
        }

        playerSpeed.y += gravidade * GetFrameTime();
        player.y += playerSpeed.y * GetFrameTime();

        if (CheckCollisionRecs(player, salaAtual->chao)) {
            player.y = salaAtual->chao.y - player.height;
            playerSpeed.y = 0;
            isGrounded = true;
        }
        if (CheckCollisionRecs(player, salaAtual->plataforma[0])) {
            player.y = salaAtual->plataforma[0].y - player.height;
            playerSpeed.y = 0;
            isGrounded = true;
        }
        if (CheckCollisionRecs(player, salaAtual->plataforma[1])) {
            player.y = salaAtual->plataforma[1].y - player.height;
            playerSpeed.y = 0;
            isGrounded = true;
        }
        
        if(salaAtual->inimigoVivo == false){
            if (player.x + player.width > largura) {
                if (salaAtual->direita) {
                    salaAtual = salaAtual->direita;
                    if((salaAtual->inimigoVivo == true) && (salaAtual != sala5)){
                        PlaySound(enemyHowl);
                    }else if((salaAtual->inimigoVivo == true) && (salaAtual == sala5)){
                        PlaySound(enemyMage);
                    }
                    player.x = 0;
                    
                } else {
                    player.x = largura - player.width;
                }
            } else if (player.x < 0) {
                if (salaAtual->esquerda) {
                    salaAtual = salaAtual->esquerda;
                    player.x = largura - player.width;
                } else {
                    player.x = 0;
                }
            }
        }

        if (salaAtual->inimigoVivo) {
            
            if(player.x < 0) player.x = 0;
            if(player.x > largura - player.width) player.x = largura - player.width;
            
            if(salaAtual->enemy.x < player.x){
                enemyAnim.virado = false;
            }else{
                enemyAnim.virado = true;
            }
            
            if (salaAtual->id == 5) {
                if(IsMusicStreamPlaying(music)){
                    StopMusicStream(lvlMusic);
                    lvlMusic = musicBoss;
                    PlayMusicStream(lvlMusic);
                }
                if (movingRight) {
                    salaAtual->enemy.x += velocidadeBoss * GetFrameTime();
                    if (salaAtual->enemy.x + salaAtual->enemy.width >= largura) movingRight = false;
                } else {
                    salaAtual->enemy.x -= velocidadeBoss * GetFrameTime();
                    if (salaAtual->enemy.x <= 0) movingRight = true;
                }

                if (rand() % 8 == 0) {
                    for (int i = 0; i < maxBalasInimigo; i++) {
                        if (!salaAtual->balasInimigo[i].ativa) {
                            salaAtual->balasInimigo[i].rect = (Rectangle){salaAtual->enemy.x + salaAtual->enemy.width / 2, salaAtual->enemy.y + salaAtual->enemy.height, 5, 10};
                            salaAtual->balasInimigo[i].speed = (Vector2){0, velocidadeBala};
                            salaAtual->balasInimigo[i].ativa = true;
                            break;
                        }
                    }
                }
            } else {
                Vector2 direction = {player.x - salaAtual->enemy.x, player.y - salaAtual->enemy.y};
                float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
                if (magnitude > 0) {
                    direction.x /= magnitude;
                    salaAtual->enemy.x += direction.x * velocidadeInimigo * GetFrameTime();
                }
            }

            if (CheckCollisionRecs(player, salaAtual->enemy)) {
                playerLife--;
                player.x -= 50;
            }
        }

        for (int i = 0; i < maxBalasInimigo; i++) {
            if (salaAtual->balasInimigo[i].ativa) {
                salaAtual->balasInimigo[i].rect.y += salaAtual->balasInimigo[i].speed.y * GetFrameTime();
                if (salaAtual->balasInimigo[i].rect.y > altura) salaAtual->balasInimigo[i].ativa = false;

                if (CheckCollisionRecs(salaAtual->balasInimigo[i].rect, player)) {
                    playerLife--;
                    salaAtual->balasInimigo[i].ativa = false;
                    if (playerLife <= 0) break;
                }
            }
        }

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP)) {
            PlaySound(playerShoot);
            for (int i = 0; i < maxBalas; i++) {
                if (!balas[i].ativa) {
                    balas[i].rect.x = player.x + player.width;
                    balas[i].rect.y = player.y + (player.height / 6) - 20;
                    balas[i].rect.width = 10;
                    balas[i].rect.height = 5;

                    if (IsKeyPressed(KEY_RIGHT)) {
                        balas[i].ativa = true;
                        balas[i].speed = (Vector2){velocidadeBala, 0};
                    } else if (IsKeyPressed(KEY_LEFT)) {
                        balas[i].ativa = true;
                        balas[i].speed = (Vector2){-velocidadeBala, 0};
                    } else if (IsKeyPressed(KEY_UP)) {
                        balas[i].ativa = true;
                        balas[i].speed = (Vector2){0, -velocidadeBala};
                        balas[i].rect.height = 10;
                    }
                    break;
                }
            }
        }

        for (int i = 0; i < maxBalas; i++) {
            if (balas[i].ativa) {
                balas[i].rect.x += balas[i].speed.x * GetFrameTime();
                balas[i].rect.y += balas[i].speed.y * GetFrameTime();
                if (balas[i].rect.x > largura || balas[i].rect.x < 0 || balas[i].rect.y < 0) balas[i].ativa = false;

                if (salaAtual->inimigoVivo && CheckCollisionRecs(balas[i].rect, salaAtual->enemy)) {
                    salaAtual->vidaInimigo -= 1;
                    balas[i].ativa = false;
                    if (salaAtual->vidaInimigo <= 0){
                      salaAtual->inimigoVivo = false;
                      enemyDiedCount += 1;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
       if(salaAtual == sala5) {
    float scaleX = (float)GetScreenWidth() / salaAtual->background.width;
    float scaleY = (float)GetScreenHeight() / salaAtual->background.height;
    float scale = fmaxf(scaleX, scaleY);  
    
    Vector2 pos = {
        (GetScreenWidth() - (salaAtual->background.width * scale)) * 0.5f,
        (GetScreenHeight() - (salaAtual->background.height * scale)) * 0.5f
    };
    
    DrawTextureEx(salaAtual->background, pos, 0.0f, scale, WHITE);
} else {
    Rectangle dest = {
        0, 
        0, 
        (float)GetScreenWidth(), 
        (float)GetScreenHeight()
    };
    
    DrawTexturePro(
        salaAtual->background,
        (Rectangle){0, 0, (float)salaAtual->background.width, (float)salaAtual->background.height},
        dest,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}
    DrawRectangleRec(salaAtual->plataforma[0], BROWN);
    DrawRectangleRec(salaAtual->plataforma[1], BROWN);
        Rectangle sourceRec = {
            currentAnim->larguraFrame * currentAnim->frameAtual,
            0,
            currentAnim->virado ? -currentAnim->larguraFrame : currentAnim->larguraFrame,
            (float)currentAnim->texture.height
        };
        
        float spriteHeight = (float)currentAnim->texture.height;
        float spriteWidth = currentAnim->larguraFrame;
        float desiredHeight = 180.0f;
        float escala = desiredHeight / spriteHeight;
        
        Rectangle destRec = {
            player.x - ((spriteWidth * escala - player.width) / 2),
            player.y - (desiredHeight - player.height),            
            spriteWidth * escala,                                   
            spriteHeight * escala                                   
        };      
        
        Vector2 origin = {0, 0};
        DrawTexturePro(currentAnim->texture, sourceRec, destRec, origin, 0.0f, WHITE);
        
        float desiredEnemyHeight = 200.0f;
        float escalaInimigo = ((desiredEnemyHeight) / (float)enemyAnim.texture.height);
        
        if (salaAtual->inimigoVivo) {
            Rectangle enemySourceRec = {
                enemyAnim.larguraFrame * enemyAnim.frameAtual,
                0,
                enemyAnim.virado ? -enemyAnim.larguraFrame : enemyAnim.larguraFrame,
                (float)enemyAnim.texture.height
            };
            
     
            
            Rectangle enemyDestRec = {
                salaAtual->enemy.x - 70,
                salaAtual->enemy.y - 70,
                enemyAnim.larguraFrame * escalaInimigo,
                enemyAnim.texture.height * escalaInimigo
            };
            DrawTexturePro(enemyAnim.texture, enemySourceRec, enemyDestRec, (Vector2){0, 0}, 0.0f, WHITE);
          
            

            DrawRectangleLines(
                (int)salaAtual->enemy.x,
                (int)salaAtual->enemy.y,
                (int)salaAtual->enemy.width,
                (int)salaAtual->enemy.height,
                RED
                );


            DrawRectangleLines(
                (int)salaAtual->enemyAnim->hitbox.x,
                (int)salaAtual->enemyAnim->hitbox.y,
                (int)salaAtual->enemyAnim->hitbox.width,
                (int)salaAtual->enemyAnim->hitbox.height,
            GREEN
                );
        }

        for (int i = 0; i < maxBalas; i++) if (balas[i].ativa) DrawRectangleRec(balas[i].rect, BLACK);
        for (int i = 0; i < maxBalasInimigo; i++) if (salaAtual->balasInimigo[i].ativa) DrawRectangleRec(salaAtual->balasInimigo[i].rect, WHITE);

        DrawText(TextFormat("Vidas: %d", playerLife), 10, 10, 20, WHITE);
        EndDrawing();
    }
    
    score = enemyDiedCount * (int)countdownTime + (playerLife * 100);
    vitoria();
    
    UnloadTexture(idleAnim.texture);
    UnloadTexture(walkAnim.texture);
    UnloadTexture(jumpAnim.texture);
    UnloadTexture(enemyAnim.texture);
    
    freeSala(sala1);
    UnloadSound(enemyMage);
    UnloadSound(enemyHowl);
    UnloadSound(playerShoot);
    UnloadSound(victory);
    UnloadSound(defeat);
    UnloadMusicStream(musicBoss);
    UnloadMusicStream(music);
    CloseAudioDevice();
    
    return score;
    
}
tela Menu(void) {
    InitAudioDevice();

    Texture2D menuBackground = LoadTexture("./images/backgroundMenu.png");
    SetTextureFilter(menuBackground, TEXTURE_FILTER_POINT);

    Music menuMusic = LoadMusicStream("./music/significance.mp3");
    SetMusicVolume(menuMusic, 0.5f);
    PlayMusicStream(menuMusic);

    int selectedOption = 0;
    int numRanking = contScore(ranking);
    const char *menuOptions[quantOpcoes] = { "Iniciar", "Instruções", "Ranking", "Sair" };

    int fontSize = 20;
    int spacing = 40;
    int totalHeight = quantOpcoes * spacing;
    int startY = (altura - totalHeight) / 2;
    int titleY = startY - 60;

    while (!WindowShouldClose()) {
        UpdateMusicStream(menuMusic);

        if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % quantOpcoes;
        if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + quantOpcoes) % quantOpcoes;

        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedOption == 0) {
                StopMusicStream(menuMusic);
                UnloadMusicStream(menuMusic);
                UnloadTexture(menuBackground);
                CloseAudioDevice();
                return JOGO;
            } else if (selectedOption == 3) {
                StopMusicStream(menuMusic);
                UnloadMusicStream(menuMusic);
                UnloadTexture(menuBackground);
                CloseAudioDevice();
                return SAIR;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(menuBackground, 0, 0, WHITE);

            DrawText("Menu Principal", (largura - MeasureText("Menu Principal", 30)) / 2, titleY, 30, DARKBLUE);

            for (int i = 0; i < quantOpcoes; i++) {
                Color color = (i == selectedOption) ? RED : WHITE;
                int textWidth = MeasureText(menuOptions[i], fontSize);
                int posX = (largura - textWidth) / 2;
                int posY = startY + i * spacing;
                DrawText(menuOptions[i], posX, posY, fontSize, color);
            }

            // Área de instruções ou ranking ao lado (à direita)
            if (selectedOption == 1) {
                int infoX = largura * 3 / 4;
                DrawText("Instruções", infoX - MeasureText("Instruções", 30) / 2, startY - 60, 30, DARKBLUE);
                DrawText("Use W-A-D para movimentar o personagem.", infoX - MeasureText("Use W-A-D para movimentar o personagem.", 17) / 2, startY, 17, WHITE);
                DrawText("Use as setas para disparar.", infoX - MeasureText("Use as setas para disparar.", 17) / 2, startY + 30, 17, WHITE);
                DrawText("Aperte ESC para parar de jogar.", infoX - MeasureText("Aperte ESC para parar de jogar.", 17) / 2, startY + 60, 17, WHITE);
            } else if (selectedOption == 2) {
                int infoX = largura * 3 / 4;
                DrawText("Ranking", infoX - MeasureText("Ranking", 30) / 2, startY - 60, 30, DARKBLUE);
                DrawText("Top 10 melhores pontuações!", infoX - MeasureText("Top 10 melhores pontuações!", 17) / 2, startY - 30, 17, WHITE);
                for (int i = 0; i < numRanking; i++) {
                    char text[100];
                    snprintf(text, sizeof(text), "%d. %s - %d", i + 1, ranking[i].name, ranking[i].score);
                    DrawText(text, infoX - MeasureText(text, 17) / 2, startY + i * 25, 17, WHITE);
                }
            }

        EndDrawing();
    }

    UnloadMusicStream(menuMusic);
    CloseAudioDevice();
    UnloadTexture(menuBackground);
    return SAIR;
}

void salvarPontos(const char *name, int score) {
    if (name == NULL) {
        return;
    }

    FILE *file = fopen("Ranking.txt", "r");
    if (!file) {
        printf("Oh no\n");
        return;
    }

    pontuacao temp;
    bool nomeExiste = false;
    int currentCount = 0;

    pontuacao pontos[quantRecordes];

    while (fscanf(file, "%50s %d", temp.name, &temp.score) == 2) {
        pontos[currentCount++] = temp;
        if (strcmp(temp.name, name) == 0) {
            nomeExiste = true;
            if (score > temp.score) {
                pontos[currentCount - 1].score = score;
            }
        }
    }
    fclose(file);

    if (!nomeExiste) {
        strcpy(pontos[currentCount].name, name);
        pontos[currentCount].score = score;
        currentCount++;
    }

    for (int i = 0; i < currentCount - 1; i++) {
        for (int j = 0; j < currentCount - 1 - i; j++) {
            if (pontos[j].score < pontos[j + 1].score) {
                pontuacao temp = pontos[j];
                pontos[j] = pontos[j + 1];
                pontos[j + 1] = temp;
            }
        }
    }

    file = fopen("Ranking.txt", "w");
    if (!file) {
        printf(":(\n");
        return;
    }

    for (int i = 0; i < currentCount && i < quantRecordes; i++) {
        if (strcmp(pontos[i].name, "") == 0 || strcmp(pontos[i].name, " ") == 0) {
            break;
        }
        fprintf(file, "%s %d\n", pontos[i].name, (int)pontos[i].score);
        printf("Saving %s %d to file\n", pontos[i].name, (int)pontos[i].score);
    }
    fclose(file);
}
int contScore(pontuacao *ranking) {
    FILE *file = fopen("Ranking.txt", "r");
    if (!file) {
        printf("Oh oh\n");
        return 0;
    }

    int cont = 0;
    while (cont < quantRecordes && fscanf(file, "%50s %d", ranking[cont].name, &ranking[cont].score) == 2) {
        cont++;
    }
    fclose(file);

    for (int i = 0; i < cont - 1; i++) {
        for (int j = 0; j < cont - 1 - i; j++) {
            if (ranking[j].score < ranking[j + 1].score) {
                pontuacao temp = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = temp;
            }
        }
    }

    return cont;
}
EnemyAnimation* initEnemyAnimation(void) {
    EnemyAnimation* enemyAnim = (EnemyAnimation*)malloc(sizeof(EnemyAnimation));
        
    enemyAnim->texture = LoadTexture("./assets/Walk.png");
    enemyAnim->larguraFrame = (float)(enemyAnim->texture.width /8);
    enemyAnim->maxFrames = 8;
    enemyAnim->frameAtual = 0;
    enemyAnim->tempoFrame = 0.0f;
    enemyAnim->virado = true;
    enemyAnim->escala = 2.0f;
    
    enemyAnim->hitbox = (Rectangle){
        0, 0,
        enemyAnim->larguraFrame * enemyAnim->escala,
        enemyAnim->texture.height * enemyAnim->escala
    };
    
    return enemyAnim;
}

Sala* criarSala(int id) {
    Sala* sala = (Sala*)malloc(sizeof(Sala));
    sala->id = id;
    sala->esquerda = NULL;
    sala->direita = NULL;
    sala->chao = (Rectangle){0, 1080 - 120, 1920, 50};
    sala->teto = (Rectangle){0, 0, 1920, 50};
    sala->inimigoVivo = false;
    sala->enemyAnim = NULL;
    for (int i = 0; i < maxBalasInimigo; i++) {
        sala->balasInimigo[i].ativa = false;
    }
    if (id == 1) {
        sala->background = LoadTexture("./images/background.png");
        sala->plataforma[0] = (Rectangle){300, 700, 300, 30};
        sala->plataforma[1] = (Rectangle){1300, 800, 200, 30};
    } else if (id == 2) {
        sala->background = LoadTexture("./images/background.png");
        sala->inimigoVivo = true;
        sala->enemy = (Rectangle){600, 1080 - 250, 50, 120};
        sala->vidaInimigo = 70;
        sala->enemyAnim = initEnemyAnimation();
        sala->plataforma[0] = (Rectangle){100, 800, 400, 30};
        sala->plataforma[1] = (Rectangle){1400, 750, 150, 30};
    } else if (id == 3) {
        sala->background = LoadTexture("./images/background.png");
        sala->inimigoVivo = true;
        sala->enemy = (Rectangle){600, 1080 - 250, 50, 120};
        sala->vidaInimigo = 70;
        sala->enemyAnim = initEnemyAnimation();
        sala->plataforma[0] = (Rectangle){500, 800, 350, 30};
        sala->plataforma[1] = (Rectangle){1100, 750, 250, 30};
    } else if (id == 4) {
        sala->background = LoadTexture("./images/background.png");
        sala->inimigoVivo = true;
        sala->enemy = (Rectangle){600, 1080 - 250, 50, 120};
        sala->vidaInimigo = 70;
        sala->enemyAnim = initEnemyAnimation();
        sala->plataforma[0] = (Rectangle){200, 800, 300, 30};
        sala->plataforma[1] = (Rectangle){500, 750, 200, 30};
    } else if (id == 5) {
        sala->background = LoadTexture("./images/finalboss.png");
        sala->inimigoVivo = true;
        sala->enemy = (Rectangle){0, sala->teto.y + sala->teto.height, 200, 50};
        sala->vidaInimigo = 100;
        sala->enemyAnim = initEnemyAnimation();
        sala->enemyAnim->maxFrames = 8;
        sala->plataforma[0] = (Rectangle){400, 850, 300, 30};
        sala->plataforma[1] = (Rectangle){1200, 750, 300, 30};
    }

    return sala;
}
void freeSala(Sala* sala) {
    while (sala) {
        if (sala->enemyAnim) {
            UnloadTexture(sala->enemyAnim->texture);
            free(sala->enemyAnim);
        }
        Sala* next = sala->direita;
        free(sala);
        sala = next;
    }
}

void vitoria(){
    const int largura = 1920;
    const int altura = 1080;
    
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Coloque o seu nome:", largura / 2 - MeasureText("Coloque o seu nome:", 20) / 2, altura / 2 - 50, 20, DARKBLUE);
        DrawText(nomeJogador, largura / 2 - MeasureText(nomeJogador, 20) / 2, altura / 2, 20, WHITE);
        EndDrawing();
        
        int entradaTeclado = GetKeyPressed();
        if(entradaTeclado >= 32 && entradaTeclado <= 125 && index < 50){
            nomeJogador[index++] = (char)entradaTeclado;
            nomeJogador[index] = '\0';
        }
        if(IsKeyPressed(KEY_BACKSPACE) && index > 0){
            nomeJogador[--index] = '\0';
        }
        if(IsKeyPressed(KEY_ENTER)){
            break;
        }
    }
}
void updateEnemyAnimation(Sala* salaAtual, Rectangle player) {
    if (!salaAtual->inimigoVivo || !salaAtual->enemyAnim) return;

    EnemyAnimation* enemyAnim = salaAtual->enemyAnim;
    
    enemyAnim->position.x = salaAtual->enemy.x;
    enemyAnim->position.y = salaAtual->enemy.y;
    
    salaAtual->enemy.x = enemyAnim->position.x;
    salaAtual->enemy.y = enemyAnim->position.y;
    salaAtual->enemy.width  = enemyAnim->hitbox.width;
    salaAtual->enemy.height = enemyAnim->hitbox.height;
    
    enemyAnim->virado = (salaAtual->enemy.x > player.x);
    
    enemyAnim->tempoFrame += GetFrameTime();
    if (enemyAnim->tempoFrame >= VelocidadeQuadros) {
        enemyAnim->tempoFrame = 0.0f;
        enemyAnim->frameAtual++;
        if (enemyAnim->frameAtual >= enemyAnim->maxFrames) {
            enemyAnim->frameAtual = 0;
        }
    }
    
    enemyAnim->hitbox.x = enemyAnim->position.x;
    enemyAnim->hitbox.y = enemyAnim->position.y;
}

void desenharInimigo(Sala* salaAtual) {
    if (!salaAtual->inimigoVivo || !salaAtual->enemyAnim) return;

    EnemyAnimation* enemyAnim = salaAtual->enemyAnim;
    
    Rectangle sourceRec = {
        enemyAnim->larguraFrame * enemyAnim->frameAtual,
        0,
        enemyAnim->virado ? -enemyAnim->larguraFrame : enemyAnim->larguraFrame,
        (float)enemyAnim->texture.height
    };
    
    Rectangle destRec = {
        enemyAnim->position.x ,
        enemyAnim->position.y,
        enemyAnim->larguraFrame * enemyAnim->escala,
        enemyAnim->texture.height * enemyAnim->escala
    };
    
    DrawTexturePro(
        enemyAnim->texture,
        sourceRec,
        destRec,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
    
    
}
