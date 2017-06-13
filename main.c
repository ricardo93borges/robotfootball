#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
void moveRobo(cpBody* body, void* data);
void moveBola(cpBody* body, void* data);
void moveGoleiroA(cpBody* body, void* data);

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
cpBody* atacanteA;
cpBody* defensorA;

//Robos do time B
cpBody* goleiroB;
cpBody* atacanteB;
cpBody* defensorB;

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

    // ... e um robô de exemplo
    //robotBody = newCircle(cpv(812,350), 20, 5, "ship1.png", moveRobo, 0.2, 0.5);
    goleiroA = newCircle(cpv(50,350), 20, 5, "ship1.png", moveGoleiroA, 0.2, 0.5);
    defensorA = newCircle(cpv(250,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);
    atacanteA = newCircle(cpv(450,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);

    goleiroB = newCircle(cpv(970,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);
    defensorB = newCircle(cpv(770,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);
    atacanteB = newCircle(cpv(570,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);
}

void moveGoleiroA(cpBody* body, void* data)
{
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    if(robotPos.x>=100){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        ballPos.x = -ballPos.x;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);

    }else if(robotPos.y <= 180){
        cpVect pos = robotPos;
        pos.x = robotPos.x;
        pos.y = robotPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);

    }else if(robotPos.y >= 440){
        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        ballPos.y = -ballPos.y;
        cpVect delta = cpvadd(ballPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);

    }else{
        if(ballPos.x < 512){
            cpVect pos = robotPos;
            pos.x = -robotPos.x;
            pos.y = -robotPos.y;
            cpVect delta = cpvadd(ballPos,pos);
            delta = cpvmult(cpvnormalize(delta),20);
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    }
}
// Exemplo de função de movimentação: move o robô em direção à bola
void moveRobo(cpBody* body, void* data)
{
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
//    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 20 unidades
    delta = cpvmult(cpvnormalize(delta),20);
    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
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
    }else if(ballPos.x >= 975 && ballPos.y >= 350 && ballPos.y <= 385){
        updateScore(1);
        resetPositions();
    }
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

    pos = cpBodyGetPosition(atacanteA);
    pos.x = 450;
    pos.y = 350;
    cpBodySetPosition(atacanteA, pos);

    pos = cpBodyGetPosition(defensorA);
    pos.x = 250;
    pos.y = 350;
    cpBodySetPosition(defensorA, pos);

    pos = cpBodyGetPosition(goleiroB);
    pos.x = 970;
    pos.y = 350;
    cpBodySetPosition(goleiroB, pos);

    pos = cpBodyGetPosition(defensorB);
    pos.x = 770;
    pos.y = 350;
    cpBodySetPosition(defensorB, pos);

    pos = cpBodyGetPosition(atacanteB);
    pos.x = 570;
    pos.y = 350;
    cpBodySetPosition(atacanteB, pos);
}

//Atualiza scores
void updateScore(int score){
    if(score == 1){
        score1++;
    }else{
        score2++;
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

    ud = cpBodyGetUserData(defensorA);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorA);

    ud = cpBodyGetUserData(atacanteA);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteA);

    ud = cpBodyGetUserData(goleiroB);
    cpShapeFree(ud->shape);
    cpBodyFree(goleiroB);

    ud = cpBodyGetUserData(defensorB);
    cpShapeFree(ud->shape);
    cpBodyFree(defensorB);

    ud = cpBodyGetUserData(atacanteB);
    cpShapeFree(ud->shape);
    cpBodyFree(atacanteB);

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
