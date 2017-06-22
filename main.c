#include <math.h>
#include "chipmunk.h"
#include "SOIL.h"

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
void moveRobo(cpBody* body, void* data);
void moveBola(cpBody* body, void* data);

void moveGoleiroA(cpBody* body, void* data);
void moveGoleiroB(cpBody* body, void* data);

void moveDefensorDireitaA(cpBody* body, void* data);
void moveDefensorDireitaB(cpBody* body, void* data);

void moveDefensorEsquerdaA(cpBody* body, void* data);
void moveDefensorEsquerdaB(cpBody* body, void* data);

void moveAtacanteDireitaA(cpBody* body, void* data);
void moveAtacanteDireitaB(cpBody* body, void* data);

void moveAtacanteEsquerdaA(cpBody* body, void* data);
void moveAtacanteEsquerdaB(cpBody* body, void* data);


// Prototipos
void initCM();
void freeCM();
void restartCM();
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast);

// Score do jogo
int score1 = 0;
int score2 = 0;

// Flag de controle: 1 se o jogo tiver acabado
int gameOver = 0;

// cpVect e' um vetor 2D e cpv() e' uma forma rapida de inicializar ele.
cpVect gravity;

// O ambiente
cpSpace* space;

// Paredes "invisíveis" do ambiente
cpShape* leftWall, *rightWall, *topWall, *bottomWall;

// A bola
cpBody* ballBody;

// Um robô
cpBody* robotBody;

//Robos do time A
cpBody* goleiroA;
cpBody* defensorDireitaA;
cpBody* defensorEsquerdaA;
cpBody* atacanteDireitaA;
cpBody* atacanteEsquerdaA;

//Robos do time B
cpBody* goleiroB;
cpBody* defensorDireitaB;
cpBody* defensorEsquerdaB;
cpBody* atacanteDireitaB;
cpBody* atacanteEsquerdaB;

// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

// Inicializa o ambiente: é chamada por init() em opengl.c, pois necessita do contexto
// OpenGL para a leitura das imagens
void initCM()
{
    gravity = cpv(0, 100);

    // Cria o universo
    space = cpSpaceNew();

    // Seta o fator de damping, isto é, de atrito do ar
    cpSpaceSetDamping(space, 0.8);

    // Descomente a linha abaixo se quiser ver o efeito da gravidade!
    //cpSpaceSetGravity(space, gravity);

    // Adiciona 4 linhas estáticas para formarem as "paredes" do ambiente
    leftWall   = newLine(cpv(0,0), cpv(0,ALTURA_JAN), 0, 1.0);
    rightWall  = newLine(cpv(LARGURA_JAN,0), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);
    bottomWall = newLine(cpv(0,0), cpv(LARGURA_JAN,0), 0, 1.0);
    topWall    = newLine(cpv(0,ALTURA_JAN), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);

    // Agora criamos a bola...
    // Os parâmetros são:
    //   - posição: cpVect (vetor: x e y)
    //   - raio
    //   - massa
    //   - imagem a ser carregada
    //   - ponteiro para a função de movimentação (chamada a cada passo, pode ser NULL)
    //   - coeficiente de fricção
    //   - coeficiente de elasticidade
    ballBody = newCircle(cpv(512,350), 8, 1, "small_football.png", moveBola, 0.2, 1);

    goleiroA = newCircle(cpv(50,350), 20, 50, "ship1.png", moveGoleiroA, 0.2, 0.5);
    defensorEsquerdaA = newCircle(cpv(280,200), 20, 50, "ship1.png", NULL, 0.2, 0.5);
    defensorDireitaA = newCircle(cpv(280,500), 20, 50, "ship1.png", NULL, 0.2, 0.5);
    atacanteDireitaA = newCircle(cpv(490,335), 20, 50, "ship1.png", NULL, 0.2, 0.5);
    atacanteEsquerdaA = newCircle(cpv(490, 385), 20, 50, "ship1.png", NULL, 0.2, 0.5);

    goleiroB = newCircle(cpv(970,350), 20, 50, "ship2.png", moveGoleiroB, 0.2, 0.5);
    defensorEsquerdaB = newCircle(cpv(770,200), 20, 50, "ship2.png", NULL, 0.2, 0.5);
    defensorDireitaB = newCircle(cpv(770,500), 20, 50, "ship2.png", NULL, 0.2, 0.5);
    atacanteDireitaB = newCircle(cpv(530,335), 20, 50, "ship2.png", NULL, 0.2, 0.5);
    atacanteEsquerdaB = newCircle(cpv(530,385), 20, 50, "ship2.png", NULL, 0.2, 0.5);
}


//Reseta posicao dos robos
void resetPositions(){
    cpVect pos = cpBodyGetPosition(ballBody);
    pos.x = 512;
    pos.y = 350;
    cpBodySetPosition(ballBody, pos);
    
    pos = cpBodyGetPosition(goleiroA);
    pos.x = 50;
    pos.y = 350;
    cpBodySetPosition(goleiroA, pos);
    
    pos = cpBodyGetPosition(atacanteDireitaA);
    pos.x = 450;
    pos.y = 350;
    cpBodySetPosition(atacanteDireitaA, pos);
    
    pos = cpBodyGetPosition(defensorDireitaA);
    pos.x = 250;
    pos.y = 350;
    cpBodySetPosition(defensorDireitaA, pos);
    
    pos = cpBodyGetPosition(goleiroB);
    pos.x = 970;
    pos.y = 350;
    cpBodySetPosition(goleiroB, pos);
    
    pos = cpBodyGetPosition(defensorDireitaB);
    pos.x = 770;
    pos.y = 350;
    cpBodySetPosition(defensorDireitaB, pos);
    
    pos = cpBodyGetPosition(atacanteDireitaB);
    pos.x = 570;
    pos.y = 350;
    cpBodySetPosition(atacanteDireitaB, pos);
}

//Atualiza scores
void updateScore(int score){
    if(score == 1){
        score1++;
    }else{
        score2++;
    }
}


void moveGoleiroA(cpBody* body, void* data)
{
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    //Volta para o gol
    if(robotPos.x >= 130 || robotPos.y >= 460 || robotPos.y <= 260){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect golPos = cpv(50,350);
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    //vai atras da bola
    }else if(ballPos.x < 512){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
}

void moveGoleiroB(cpBody* body, void* data)
{
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    //volta para o gol
    if(robotPos.x <= 900 || robotPos.y >= 450 || robotPos.y <= 270){
        cpVect pos = robotPos;
        pos.x = robotPos.x;
        pos.y = -robotPos.y;
        cpVect golPos = cpv(970,350);
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),10);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    //vai atras da bola
    }else if(ballPos.x > 520){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }

}

void moveDefensorDireitaA(cpBody* body, void* data)
{
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    //volta para a posicao de defensor
    if(ballPos.x <  500){
        printf("%f", ballPos.x);
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    } else  if(robotPos.x <= 200 || robotPos.y >= 45 || robotPos.y <= 665){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect golPos = cpv(250,350);
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),10);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    //vai atras da bola
    }

}

void moveDefensorDireitaB(cpBody* body, void* data)
{
     cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    //volta para a posicao de defensor
    if(ballPos.x >  515){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    } else  if(robotPos.x >= 830 || robotPos.y >= 45|| robotPos.y <= 665 ){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect golPos = cpv(830,355);
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),10);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    //vai atras da bola
    }
}


// Exemplo: move a bola aleatoriamente
void moveBola(cpBody* body, void* data)
{
    // Obtém a posição da bola...
    cpVect ballPos  = cpBodyGetPosition(ballBody);


    // Sorteia um impulso entre -10 e 10, para x e y
    cpVect impulso = cpv(rand()%20-10,rand()%20-10);
    //cpVect impulso = cpv(50,150);
    //impulso = cpvneg(impulso);
    impulso = cpvmult(cpvnormalize(impulso),20);
    // E aplica na bola
    cpBodyApplyImpulseAtWorldPoint(body, impulso, cpBodyGetPosition(body));

    //Update scores
    if(ballPos.x <= 50 && ballPos.y >= 350 && ballPos.y <= 385){
        updateScore(2);
        resetPositions();//resetar posicoes dos robos para nao atualizar o score varias vezes
    }else if(ballPos.x >= 975 && ballPos.y >= 325 && ballPos.y <= 385){
        updateScore(1);
        resetPositions();
    }
}
// Libera memória ocupada por cada corpo, forma e ambiente
// Acrescente mais linhas caso necessário
void freeCM()
{
    printf("Cleaning up!\n");
    UserData* ud = cpBodyGetUserData(ballBody);
    cpShapeFree(ud->shape);
    cpBodyFree(ballBody);

    ud = cpBodyGetUserData(robotBody);
    cpShapeFree(ud->shape);
    cpBodyFree(robotBody);

    ud = cpBodyGetUserData(goleiroA);
    cpShapeFree(ud->shape);
    cpBodyFree(goleiroA);

    ud = cpBodyGetUserData(defensorDireitaA);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorDireitaA);

    ud = cpBodyGetUserData(defensorEsquerdaA);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorEsquerdaA);
    
    ud = cpBodyGetUserData(atacanteDireitaA);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteDireitaA);

    ud = cpBodyGetUserData(atacanteEsquerdaA);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteEsquerdaA);
    
    ud = cpBodyGetUserData(goleiroB);
    cpShapeFree(ud->shape);
    cpBodyFree(goleiroB);

    ud = cpBodyGetUserData(defensorDireitaB);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorDireitaB);

    ud = cpBodyGetUserData(defensorEsquerdaB);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorEsquerdaB);
    
    ud = cpBodyGetUserData(atacanteDireitaB);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteDireitaB);

    ud = cpBodyGetUserData(atacanteEsquerdaB);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteEsquerdaB);
    
    cpShapeFree(leftWall);
    cpShapeFree(rightWall);
    cpShapeFree(bottomWall);
    cpShapeFree(topWall);

    cpSpaceFree(space);
}

// Função chamada para reiniciar a simulação
void restartCM()
{
    // Escreva o código para reposicionar os jogadores, ressetar o score, etc.

    // Não esqueça de ressetar a variável gameOver!
    gameOver = 0;
}

// ************************************************************
//
// A PARTIR DESTE PONTO, O PROGRAMA NÃO DEVE SER ALTERADO
//
// A NÃO SER QUE VOCÊ SAIBA ***EXATAMENTE*** O QUE ESTÁ FAZENDO
//
// ************************************************************

int main(int argc, char** argv)
{
    // Inicialização da janela gráfica
    init(argc,argv);

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

// Cria e adiciona uma nova linha estática (segmento) ao ambiente
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast)
{
   cpShape* aux = cpSegmentShapeNew(cpSpaceGetStaticBody(space), inicio, fim, 0);
   cpShapeSetFriction(aux, fric);
   cpShapeSetElasticity(aux, elast);
   cpSpaceAddShape(space, aux);
   return aux;
}

// Cria e adiciona um novo corpo dinâmico, com formato circular
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast)
{
    // Primeiro criamos um cpBody para armazenar as propriedades fisicas do objeto
    // Estas incluem: massa, posicao, velocidade, angulo, etc do objeto
    // A seguir, adicionamos formas de colisao ao cpBody para informar o seu formato e tamanho

    // O momento de inercia e' como a massa, mas para rotacao
    // Use as funcoes cpMomentFor*() para calcular a aproximacao dele
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // As funcoes cpSpaceAdd*() retornam o que voce esta' adicionando
    // E' conveniente criar e adicionar um objeto na mesma linha
    cpBody* newBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    // Por fim, ajustamos a posicao inicial do objeto
    cpBodySetPosition(newBody, pos);

    // Agora criamos a forma de colisao do objeto
    // Voce pode criar multiplas formas de colisao, que apontam ao mesmo objeto (mas nao e' necessario para o trabalho)
    // Todas serao conectadas a ele, e se moverao juntamente com ele
    cpShape* newShape = cpSpaceAddShape(space, cpCircleShapeNew(newBody, radius, cpvzero));
    cpShapeSetFriction(newShape, fric);
    cpShapeSetElasticity(newShape, elast);

    UserData* newUserData = malloc(sizeof(UserData));
    newUserData->tex = loadImage(img);
    newUserData->radius = radius;
    newUserData->shape= newShape;
    newUserData->func = func;
    cpBodySetUserData(newBody, newUserData);
    printf("newCircle: loaded img %s\n", img);
    return newBody;
}

void changeImg(cpBody* body, char* img){
    UserData* ud = cpBodyGetUserData(body);
    ud->tex = loadImage(img);
    ud->radius = 30;
    cpBodySetUserData(body, ud);
}

void easterEgg(){
    changeImg(goleiroA,"bhead_left.png");
    changeImg(defensorDireitaA,"bhead_left.png");
    changeImg(atacanteDireitaA,"bhead_left.png");
    changeImg(goleiroB,"bhead_right.png");
    changeImg(defensorDireitaB,"bhead_right.png");
    changeImg(atacanteDireitaB,"bhead_right.png");
}
