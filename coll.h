#ifndef COLL_H
#define COLL_H

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

typedef enum SHAPE_TYPE
{
  SHAPE_TYPE_CIRCLE,
  SHAPE_TYPE_RECT
} SHAPE_TYPE;

typedef enum JC_SIDE
{
    JC_SIDE_NONE = 0,
    JC_SIDE_L = 1,
    JC_SIDE_R = 1<<1,
    JC_SIDE_B = 1<<2,
    JC_SIDE_T = 1<<3
} JC_SIDE;

typedef struct jcObject
{
  SHAPE_TYPE shapeType;
  union shape 
  {
    jcircle * circle;
    jrect * rect;
  } shape;

  jvec * v;

  juint groupNum;
  void * owner;

} jcObject;

void jcObjectTranslate(jcObject *object, jvec v);

typedef void (*collHandler)(jcObject ** objects, jfloat t, JC_SIDE side);

typedef struct jcPairing
{
  jcObject * objects[2];
  collHandler handler;
} jcPairing;

typedef struct jcRegisteredCollHandler
{
  juint groupNums[2];
  collHandler handler;
} jcRegisteredCollHandler;

#include "listHeaders/jcObjectList.h"
#include "listHeaders/jcPairingList.h"
#include "listHeaders/jcRegisteredCollHandlerList.h"
typedef struct jcEng
{
  jcObjectList * objectList;
  jcPairingList * pairingList;
  jcRegisteredCollHandlerList * registeredCollHandlerList;
} jcEng;
jcEng * initJcEng(jcEng * eng);

bool registerCollHandler(jcEng * eng, juint groupNum1, juint groupNum2, collHandler handler);
bool registerCircle(jcEng * eng, jcircle * c, jvec * v, juint groupNum, void * owner);
bool registerRect(jcEng * eng, jrect * r, jvec * v, juint groupNum, void * owner);

void processCollisions(jcEng * eng);

typedef enum AXIS
{
    AXIS_X = 0,
    AXIS_Y = 1,
    NUM_AXIS = 2
} AXIS;

bool circleWithCircleCollDetect(jcircle c1, jvec v, jcircle c2, jfloat * t);
bool circleWithRectCollDetect(jcircle c, jvec v, jrect r, jfloat * t, JC_SIDE * side);
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

#endif
