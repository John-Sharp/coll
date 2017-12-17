#ifndef COLL_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t juint;
typedef int32_t jint;
typedef float jfloat;

typedef jfloat jvec[2];

typedef struct jcircle
{
    jvec c; // centre
    jfloat r; // radius
} jcircle;

typedef struct jrect
{
  jvec bl; // bottom left
  jvec tr; // top right
} jrect;

typedef enum AXIS
{
    AXIS_X = 0,
    AXIS_Y = 1,
    NUM_AXIS = 2
} AXIS;

bool circleWithCircleCollDetect(jcircle c1, jvec v, jcircle c2, jfloat * t);
bool circleWithRectCollDetect(jcircle c, jvec v, jrect r, jfloat * t);
bool circleWithAxisParallelSegCollDetect(jcircle c, jvec v, jvec b, jfloat h, AXIS ax, jfloat * t);

typedef enum COLL_TYPE
{
    COLL_TYPE_NONE,
    COLL_TYPE_SMALL_SIDE,
    COLL_TYPE_LONG_SIDE,
    COLL_TYPE_INSIDE
} COLL_TYPE;

typedef struct oneDimCollObj
{
    COLL_TYPE coll_type;
    float t_start;
    float t_end;
} oneDimCollObj;

oneDimCollObj oneDCollDetect(uint32_t a, int32_t b1, int32_t b2, int32_t v);

#define COLL_H
#endif
