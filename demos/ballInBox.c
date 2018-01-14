#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../coll.h"

typedef struct context context;

typedef struct ball
{
    context * ctx;
    jvec v;
    jcircle collBody;

    /**
     * Rectangle that the owl texture will be rendered into
     */
    SDL_Rect dest;
    SDL_Texture *tex;

} ball;

/**
 * Context structure that will be passed to the loop handler
 */
struct context
{
    SDL_Renderer *renderer;

    ball b;
};

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

void initBall(ball * b, context * ctx)
{
    b->ctx = ctx;
    b->v[0] = 0;
    b->v[1] = 0;

    b->collBody.c[0] = 300;
    b->collBody.c[1] = 200;
    b->collBody.r = 10;

    b->dest.w = 2*10;
    b->dest.h = 2*10;

    b->dest.x = 290;
    b->dest.y = 190;

    ballGetTexture(b);
}

/**
 * Loop handler that gets called each animation frame,
 * process the input, update the position of the owl and 
 * then render the texture
 */
void loop_handler(void *arg)
{
    context *ctx = arg;

    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->b.tex, NULL, &ctx->b.dest);
    SDL_RenderPresent(ctx->renderer);
}

int main()
{
    SDL_Window *window;
    context ctx;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(600, 400, 0, &window, &ctx.renderer);
    SDL_SetRenderDrawColor(ctx.renderer, 255, 255, 255, 255);

    initBall(&ctx.b, &ctx);
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
