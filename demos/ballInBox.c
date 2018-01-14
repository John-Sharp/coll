#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../coll.h"

typedef struct context context;

typedef struct box
{
    context * ctx;
    jvec v;
    jrect collBody;

    SDL_Rect dest;
    SDL_Texture *tex;
} box;

typedef struct ball
{
    context * ctx;
    jvec v;
    jcircle collBody;

    SDL_Rect dest;
    SDL_Texture *tex;
} ball;

/**
 * Context structure that will be passed to the loop handler
 */
struct context
{
    SDL_Renderer *renderer;

    ball ball;
    box box;

    jcEng * collEng;
};

int boxGetTexture(box * b)
{
  SDL_Surface *image = IMG_Load("assets/box.png");
  if (!image)
  {
     printf("IMG_Load: %s\n", IMG_GetError());
     return 0;
  }
  b->tex = SDL_CreateTextureFromSurface(b->ctx->renderer, image);

  SDL_FreeSurface (image);

  return 1;
}

void initBox(box * b, context * ctx)
{
    b->ctx = ctx;
    b->v[0] = 0;
    b->v[1] = 0;

    b->collBody.bl[0] = 80;
    b->collBody.bl[1] = 190;
    b->collBody.tr[0] = b->collBody.bl[0] + 20;
    b->collBody.tr[1] = b->collBody.bl[1] + 20;

    registerRect(ctx->collEng, &b->collBody, &b->v, 1, b);

    boxGetTexture(b);
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

void initBall(ball * b, context * ctx)
{
    b->ctx = ctx;
    b->v[0] = -0.5;
    b->v[1] = 0;

    b->collBody.c[0] = 300;
    b->collBody.c[1] = 200;
    b->collBody.r = 10;

    ballUpdateDest(b);

    registerCircle(ctx->collEng, &b->collBody, &b->v, 2, b);

    ballGetTexture(b);
}

void boxBallCollHandler(jcObject ** objects, jfloat t, JC_SIDE side)
{
    ball * ball;
    if (objects[0]->groupNum == 2)
    {
        ball = objects[0]->owner;
    }
    else
    {
        ball = objects[1]->owner;
    }

    ball->collBody.c[0] += ball->v[0] * t;
    ball->v[0] = 0;
    ball->v[1] = 0;
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
    ballUpdateDest(&ctx->ball);
    boxUpdateDest(&ctx->box);

    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->ball.tex, NULL, &ctx->ball.dest);
    SDL_RenderCopy(ctx->renderer, ctx->box.tex, NULL, &ctx->box.dest);
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
    initBall(&ctx.ball, &ctx);
    initBox(&ctx.box, &ctx);
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
