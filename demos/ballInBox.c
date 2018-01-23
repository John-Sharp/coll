#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../coll.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define ISQRT2 0.7071067811865475

typedef struct context context;

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

typedef struct ballInitParams
{
    jvec v;
    jvec c;
} ballInitParams;

ballInitParams INIT_PARAMS[] = {
//    {v: {0.031, -0.021}, c: {150, 150}},
    {v: {-0.31, -0.021}, c: {180.017555, 160.065186}},
    {v: {0.031, -0.3}, c: {180, 150}}
};


typedef enum BOX_TYPE
{
    BOX_TYPE_H,
    BOX_TYPE_V
} BOX_TYPE;
typedef struct box
{
    context * ctx;
    jvec v;
    jrect collBody;

    BOX_TYPE boxType;

    SDL_Rect dest;
} box;

typedef struct ball
{
    context * ctx;
    jvec v;
    jcircle collBody;
    jfloat im; 

    SDL_Rect dest;
    SDL_Texture *tex;
} ball;

/**
 * Context structure that will be passed to the loop handler
 */
struct context
{
    SDL_Renderer *renderer;

    ball balls[ARRAY_SIZE(INIT_PARAMS)];
    box boxes[TOTAL_BOXES];

    SDL_Texture *boxTexture;
    SDL_Texture *ballTexture;
    jcEng * collEng;
};

int getTextures(context * ctx)
{
  SDL_Surface *image = IMG_Load("assets/box.png");
  if (!image)
  {
     printf("IMG_Load: %s\n", IMG_GetError());
     return 0;
  }
  ctx->boxTexture = SDL_CreateTextureFromSurface(ctx->renderer, image);

  SDL_FreeSurface (image);

  image = IMG_Load("assets/ball.png");
  if (!image)
  {
     printf("IMG_Load: %s\n", IMG_GetError());
     return 0;
  }
  ctx->ballTexture = SDL_CreateTextureFromSurface(ctx->renderer, image);

  SDL_FreeSurface (image);

  return 1;
}

void initBox(box * b, jfloat x, jfloat y, BOX_TYPE type, context * ctx)
{
    b->ctx = ctx;
    b->v[0] = 0;
    b->v[1] = 0;

    b->boxType = type;

    b->collBody.bl[0] = x;
    b->collBody.bl[1] = y;
    b->collBody.tr[0] = b->collBody.bl[0] + 20;
    b->collBody.tr[1] = b->collBody.bl[1] + 20;

    registerRect(ctx->collEng, &b->collBody, &b->v, 1, b);
}

void createBoxes(context * ctx)
{
    juint i = 0;
    for (i = 0; i < CAGE_W; i++)
    {
        initBox(&ctx->boxes[i], i * BOX_W + CAGE_BL_X, CAGE_BL_Y, BOX_TYPE_H, ctx);
    }

    for (i = 0; i < CAGE_H - 2; i++)
    {
        initBox(&ctx->boxes[CAGE_W + i], CAGE_BL_X, (i + 1)  * BOX_H + CAGE_BL_Y, BOX_TYPE_V, ctx);
        initBox(&ctx->boxes[CAGE_W + CAGE_H - 2 + i], (CAGE_W - 1) * BOX_W + CAGE_BL_X, (i + 1) * BOX_H + CAGE_BL_Y, BOX_TYPE_V, ctx);
    }

    for (i = 0; i < CAGE_W; i++)
    {
        initBox(&ctx->boxes[CAGE_W + 2*(CAGE_H - 2) + i], i * BOX_W + CAGE_BL_X, (CAGE_H - 1) * BOX_H + CAGE_BL_Y, BOX_TYPE_H, ctx);
    }
}

void boxUpdateDest(box * b)
{
    b->dest.w = b->collBody.tr[0] - b->collBody.bl[0];
    b->dest.h = b->collBody.tr[1] - b->collBody.bl[1];

    b->dest.x = b->collBody.bl[0];
    b->dest.y = -1 * b->collBody.bl[1] + (400 - b->dest.h);
}

int ballGetTexture(ball * b)
{
  SDL_Surface *image = IMG_Load("assets/ball.png");
  if (!image)
  {
     printf("IMG_Load: %s\n", IMG_GetError());
     return 0;
  }
  b->tex = SDL_CreateTextureFromSurface(b->ctx->renderer, image);

  SDL_FreeSurface (image);

  return 1;
}

void ballUpdateDest(ball*b)
{
    b->dest.w = 2 * b->collBody.r;
    b->dest.h = 2 * b->collBody.r;

    b->dest.x = b->collBody.c[0] - b->collBody.r;
    b->dest.y = -1 * (b->collBody.c[1] - b->collBody.r) + (400 - b->dest.h);
}

void initBall(ball * b, const ballInitParams * params, context * ctx)
{
    b->ctx = ctx;

    b->im = 1;

    b->v[0] = params->v[0];
    b->v[1] = params->v[1];

    b->collBody.c[0] = params->c[0];
    b->collBody.c[1] = params->c[1];
    b->collBody.r = BALL_R;

    ballUpdateDest(b);

    registerCircle(ctx->collEng, &b->collBody, &b->v, 2, b);

    ballGetTexture(b);
}

void createBalls(context * ctx)
{
    juint i;
    for (i = 0; i < ARRAY_SIZE(INIT_PARAMS); i++)
    {
        initBall(&ctx->balls[i], &INIT_PARAMS[i], ctx);
    }
}

void momentumResolver(jvec va, jfloat ima, jvec vb, jfloat imb, jvec n, jfloat restitution)
{
    jfloat vSep = (va[0] - vb[0]) * n[0] + (va[1] - vb[1]) * n[1];
    jfloat deltaV = -(restitution + 1) * vSep;
    
    va[0] += ima / (ima + imb) * deltaV * n[0];
    va[1] += ima / (ima + imb) * deltaV * n[1];

    vb[0] -= imb / (ima + imb) * deltaV * n[0];
    vb[1] -= imb / (ima + imb) * deltaV * n[1];
}

void boxBallCollHandler(jcObject ** objects, jfloat t, JC_SIDE side)
{
    ball * ball;
    box * box;
    if (objects[0]->groupNum == 2)
    {
        ball = objects[0]->owner;
        box = objects[1]->owner;
    }
    else
    {
        ball = objects[1]->owner;
        box = objects[0]->owner;
    }

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

    ball->collBody.c[0] += ball->v[0] * (t);
    ball->collBody.c[1] += ball->v[1] * (t);

    switch (box->boxType)
    {
        case BOX_TYPE_V:
            if (n[0]==0)
            { 
                return;
            }
            n[1] = 0;
            break;
        case BOX_TYPE_H:
            if (n[1]==0)
            {
                return;
            }
            n[0] = 0;
            break;
    }


    momentumResolver(ball->v, ball->im, box->v, 0, n, 1);
}

/**
 * Loop handler that gets called each animation frame,
 * process the input, update the position of the owl and 
 * then render the texture
 */
void loop_handler(void *arg)
{
    context *ctx = arg;

    jcEngDoStep(ctx->collEng);

    SDL_RenderClear(ctx->renderer);
    juint i;

    for (i = 0; i < ARRAY_SIZE(INIT_PARAMS); i++)
    {
        ballUpdateDest(&ctx->balls[i]);
        SDL_RenderCopy(ctx->renderer, ctx->ballTexture, NULL, &ctx->balls[i].dest);
    }
    for (i = 0; i < TOTAL_BOXES; i++)
    {
        boxUpdateDest(&ctx->boxes[i]);
        SDL_RenderCopy(ctx->renderer, ctx->boxTexture, NULL, &ctx->boxes[i].dest);
    }
    SDL_RenderPresent(ctx->renderer);
}

int main()
{
    SDL_Window *window;
    context ctx;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(600, 400, 0, &window, &ctx.renderer);
    SDL_SetRenderDrawColor(ctx.renderer, 255, 255, 255, 255);

    ctx.collEng = createJcEng();
    getTextures(&ctx);
    createBalls(&ctx);
    createBoxes(&ctx);

    registerCollHandler(ctx.collEng, 1, 2, boxBallCollHandler);

    /**
     * Schedule the main loop handler to get 
     * called on each animation frame
     */
    bool carryOn = true;
    while (carryOn)
    {
        loop_handler(&ctx);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                carryOn = false;
            }
        }
    }
    // emscripten_set_main_loop_arg(loop_handler, &ctx, -1, 1);

    return 0;
}
