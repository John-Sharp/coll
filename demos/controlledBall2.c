#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../coll.h"
#include "../../engine/engine.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define ISQRT2 0.7071067811865475

enum 
{
    BOX_W = 20,
    BOX_H = 20,
    CAGE_W = 10,
    CAGE_H = 10,
    CAGE_BL_X = 80,
    CAGE_BL_Y = 20,
    TOTAL_BOXES = 2 * CAGE_W + 2 * (CAGE_H - 1),
    BALL_R = 10,
};

enum 
{
    GS_LEFT = 1,
    GS_RIGHT = 2,
    GS_UP = 3,
    GS_DOWN = 4,
};

enum
{
    DECAL_BOX,
    DECAL_BALL,
    NUM_DECALS
};

enum
{
    COLL_GROUP_BALL,
    COLL_GROUP_CONTROLLED_BALL,
    COLL_GROUP_BOX,
};

typedef struct ballInitParams
{
    jvec v;
    jvec c;
    juint collGroup;
} ballInitParams;

ballInitParams INIT_PARAMS[] = {
	{v : {-1.000000, -1.000000}, c : {127.211990, 65.455395}, collGroup : COLL_GROUP_CONTROLLED_BALL},
        {v: {0.239549, 0.390755}, c: {110.001411, 50.267086}, collGroup : COLL_GROUP_BALL},
        {v: {1.0, -0.2}, c: {180.0, 180.065186}, collGroup : COLL_GROUP_BALL},
        {v: {0.0, 0.2}, c: {180, 90}, collGroup : COLL_GROUP_BALL}
};


typedef struct box
{
    actor a;
    sprite s;

    jvec v;
    jrect collBody;
    jfloat im;
} box;

typedef struct ball
{
    actor a;
    sprite s;

    jvec v;
    jcircle collBody;
    jfloat im; 
} ball;

typedef struct controlledBallEngine
{
    engine * engine;
    jcEng * collEng;


    decal decals[NUM_DECALS];

    ball balls[ARRAY_SIZE(INIT_PARAMS)];
    box boxes[TOTAL_BOXES];

} controlledBallEngine;

void boxUpdateSprite(box * b)
{
    b->s.rect.bl[0] = b->collBody.bl[0];
    b->s.rect.bl[1] = b->collBody.bl[1];
    b->s.rect.tr[0] = b->collBody.tr[0];
    b->s.rect.tr[1] = b->collBody.tr[1];
}

void boxRenderHandler(actor * a)
{
    box * b = a->owner;
    boxUpdateSprite(b);
    engineSpriteRender(b->a.eng, &b->s);
}

void initBox(box * b, jfloat x, jfloat y, controlledBallEngine * e)
{
    b->a.owner = b;
    b->a.renderHandler = boxRenderHandler;
    b->a.logicHandler = NULL;
    engineActorReg(e->engine, &b->a);

    b->s.d = &e->decals[DECAL_BOX];

    b->v[0] = 0;
    b->v[1] = 0;

    b->collBody.bl[0] = x;
    b->collBody.bl[1] = y;
    b->collBody.tr[0] = b->collBody.bl[0] + 20;
    b->collBody.tr[1] = b->collBody.bl[1] + 20;

    boxUpdateSprite(b);

    registerRect(e->collEng, &b->collBody, &b->v, COLL_GROUP_BOX, b);
}

void createBoxes(controlledBallEngine * e)
{
    juint i = 0;
    for (i = 0; i < CAGE_W; i++)
    {
        initBox(&e->boxes[i], i * BOX_W + CAGE_BL_X, CAGE_BL_Y, e);
    }

    for (i = 0; i < CAGE_H - 2; i++)
    {
        initBox(&e->boxes[CAGE_W + i], CAGE_BL_X, (i + 1)  * BOX_H + CAGE_BL_Y, e);
        initBox(&e->boxes[CAGE_W + CAGE_H - 2 + i], (CAGE_W - 1) * BOX_W + CAGE_BL_X, (i + 1) * BOX_H + CAGE_BL_Y, e);
    }

    for (i = 0; i < CAGE_W; i++)
    {
        initBox(&e->boxes[CAGE_W + 2*(CAGE_H - 2) + i], i * BOX_W + CAGE_BL_X, (CAGE_H - 1) * BOX_H + CAGE_BL_Y, e);
    }
}

void ballUpdateSprite(ball * b)
{
    b->s.rect.bl[0] = b->collBody.c[0] - BALL_R;
    b->s.rect.bl[1] = b->collBody.c[1] - BALL_R;
    b->s.rect.tr[0] = b->collBody.c[0] + BALL_R;
    b->s.rect.tr[1] = b->collBody.c[1] + BALL_R;
}

void ballRenderHander(actor * a)
{
    ball * b = a->owner;
    ballUpdateSprite(b);
    engineSpriteRender(b->a.eng, &b->s);
}

void initBall(ball * b, const ballInitParams * params, controlledBallEngine * e)
{
    b->a.owner = b;
    b->a.renderHandler = ballRenderHander;
    b->a.logicHandler = NULL;
    engineActorReg(e->engine, &b->a);

    b->s.d = &e->decals[DECAL_BALL];

    b->im = 1;

    b->v[0] = params->v[0];
    b->v[1] = params->v[1];

    b->collBody.c[0] = params->c[0];
    b->collBody.c[1] = params->c[1];
    b->collBody.r = BALL_R;

    ballUpdateSprite(b);
    registerCircle(e->collEng, &b->collBody, &b->v, params->collGroup, b);
}

void createBalls(controlledBallEngine * e)
{
    juint i;
    for (i = 0; i < ARRAY_SIZE(INIT_PARAMS); i++)
    {
        initBall(&e->balls[i], &INIT_PARAMS[i], e);
    }
}

void momentumResolver(jvec va, jfloat ima, jvec vb, jfloat imb, jvec n, jfloat restitution, jvec deltava, jvec deltavb)
{
    jfloat vSep = (va[0] - vb[0]) * n[0] + (va[1] - vb[1]) * n[1];
    jfloat deltaV = -(restitution + 1) * vSep;
    
    deltava[0] = ima / (ima + imb) * deltaV * n[0];
    deltava[1] = ima / (ima + imb) * deltaV * n[1];

    deltavb[0] = -imb / (ima + imb) * deltaV * n[0];
    deltavb[1] = -imb / (ima + imb) * deltaV * n[1];
}

void ballBallCollHandler(jcObject ** objects, jfloat t, JC_SIDE side, jvec * deltav)
{
//	if (t==0)
    ball * ballA = objects[0]->owner;
    ball * ballB = objects[1]->owner;

    jvec n = {ballA->collBody.c[0], ballA->collBody.c[1]};
    jvecNorm(jvecSub(n, ballB->collBody.c));

    momentumResolver(ballA->v, ballA->im, ballB->v, ballB->im, n, 0, deltav[0], deltav[1]);
}

void controlledBallBallCollHandler(jcObject ** objects, jfloat t, JC_SIDE side, jvec * deltav)
{
    ball * cBall = objects[0]->owner;
    ball * ball = objects[1]->owner;

    jvec n = {cBall->collBody.c[0], cBall->collBody.c[1]};
    jvecSub(n, ball->collBody.c);
    jvecNorm(n);

    momentumResolver(cBall->v, 0, ball->v, ball->im, n, 0, deltav[0], deltav[1]);
    cBall->v[0] = 0;
    cBall->v[1] = 0;
}

void boxBallCollHandler(jcObject ** objects, jfloat t, JC_SIDE side, jvec * deltav)
{
    box * box = objects[0]->owner;
    jvec * deltavBox = &deltav[0];
    ball * ball = objects[1]->owner;
    jvec * deltavBall = &deltav[1];

    jvec n = {0, 0};

    if (side & JC_SIDE_L)
    {
        n[0] += -1;
    }
    if (side & JC_SIDE_R)
    {
        n[0] += 1;
    }
    if (side & JC_SIDE_B)
    {
        n[1] += -1;
    }
    if (side & JC_SIDE_T)
    {
        n[1] += 1;
    }

    if (n[0] != 0 && n[1] != 0)
    {
        n[0] *= ISQRT2;
        n[1] *= ISQRT2;
    }

    momentumResolver(ball->v, ball->im, box->v, 0, n, 1, *deltavBall, *deltavBox);
}

void controlledBallBoxCollHandler(jcObject ** objects, jfloat t, JC_SIDE side, jvec * deltav)
{
    box * box = objects[0]->owner;
    jvec * deltavBox = &deltav[0];
    ball * ball = objects[1]->owner;
    jvec * deltavBall = &deltav[1];

    jvec n = {0, 0};

    if (side & JC_SIDE_L)
    {
        n[0] += -1;
        ball->v[0] = 0;
    }
    if (side & JC_SIDE_R)
    {
        n[0] += 1;
        ball->v[0] = 0;
    }
    if (side & JC_SIDE_B)
    {
        n[1] += -1;
        ball->v[1] = 0;
    }
    if (side & JC_SIDE_T)
    {
        n[1] += 1;
        ball->v[1] = 0;
    }

    if (n[0] != 0 && n[1] != 0)
    {
        n[0] *= ISQRT2;
        n[1] *= ISQRT2;
    }

    (*deltavBall)[0] = 0;
    (*deltavBall)[1] = 0;

    (*deltavBox)[0] = 0;
    (*deltavBox)[1] = 0;
}


void ballInBoxPreLogic(engine * e)
{
    controlledBallEngine * eng = e->owner;
 
    if (isStateActive(GS_LEFT))
    {
        eng->balls[0].v[0] = -1;
    }
    else if (isStateActive(GS_RIGHT))
    {
        eng->balls[0].v[0] = 1;
    }
    else 
    {
        eng->balls[0].v[0] = 0;
    }
    if (isStateActive(GS_UP))
    {
        eng->balls[0].v[1] = 1;
    }
    else if (isStateActive(GS_DOWN))
    {
        eng->balls[0].v[1] = -1;
    }
    else{
        eng->balls[0].v[1] = 0;
    }

    jvec r = {eng->balls[0].collBody.c[0] - eng->balls[1].collBody.c[0], eng->balls[0].collBody.c[1] - eng->balls[1].collBody.c[1]};
    if (jvecMagSq(r) < 400)
    {
        printf("%f %f, %f %f\n", eng->balls[0].collBody.c[0], eng->balls[0].collBody.c[1], eng->balls[1].collBody.c[0], eng->balls[1].collBody.c[1]);
        exit(1);
    }

    jcEngDoStep(eng->collEng);
}

int main()
{
    struct controlledBallEngine controlledBallEngine;
    controlledBallEngine.engine = createEngine(600, 400, &controlledBallEngine);

    juint boxTexture = engineLoadTexture(controlledBallEngine.engine, "assets/box.png");
    decalInit(&controlledBallEngine.decals[DECAL_BOX], controlledBallEngine.engine,
            boxTexture, engineGetTextureRect(controlledBallEngine.engine, boxTexture));


    juint ballTexture = engineLoadTexture(controlledBallEngine.engine, "assets/ball.png");
    decalInit(&controlledBallEngine.decals[DECAL_BALL], controlledBallEngine.engine,
            ballTexture, engineGetTextureRect(controlledBallEngine.engine, ballTexture));

    controlledBallEngine.collEng = createJcEng();

    createBalls(&controlledBallEngine);
    createBoxes(&controlledBallEngine);

    registerCollHandler(controlledBallEngine.collEng, COLL_GROUP_BOX, COLL_GROUP_BALL, boxBallCollHandler);
    registerCollHandler(controlledBallEngine.collEng, COLL_GROUP_BALL, COLL_GROUP_BALL, ballBallCollHandler);
    registerCollHandler(controlledBallEngine.collEng, COLL_GROUP_CONTROLLED_BALL, COLL_GROUP_BALL, controlledBallBallCollHandler);
    registerCollHandler(controlledBallEngine.collEng, COLL_GROUP_BOX, COLL_GROUP_CONTROLLED_BALL, controlledBallBoxCollHandler);

    enginePreLogicCallBackReg(controlledBallEngine.engine, ballInBoxPreLogic);

    keyStateBinding binding;
    binding.k = SDLK_LEFT;
    binding.s = GS_LEFT;
    binding.t = BINDING_CONTINUOUS;
    addBinding(&binding);

    binding.k = SDLK_RIGHT;
    binding.s = GS_RIGHT;
    binding.t = BINDING_CONTINUOUS;
    addBinding(&binding);

    binding.k = SDLK_UP;
    binding.s = GS_UP;
    binding.t = BINDING_CONTINUOUS;
    addBinding(&binding);

    binding.k = SDLK_DOWN;
    binding.s = GS_DOWN;
    binding.t = BINDING_CONTINUOUS;
    addBinding(&binding);

    engineStart(controlledBallEngine.engine);
    return 0;
}
