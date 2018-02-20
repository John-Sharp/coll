#ifndef COLL_H
#define COLL_H

#include "../jTypes/jTypes.h"

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

jfloat jvecMagSq(jvec v);

void printJcObject(FILE * fp, jcObject *object);
void jcObjectTranslate(jcObject *object, jvec v);

typedef void (*collHandler)(jcObject ** objects, jfloat t, JC_SIDE side, jvec * deltav);

typedef struct jcEng {} jcEng;
jcEng * createJcEng();
void jcEngDoStep(jcEng * eng);

bool registerCollHandler(jcEng * eng, juint groupNum1, juint groupNum2, collHandler handler);
bool registerCircle(jcEng * eng, jcircle * c, jvec * v, juint groupNum, void * owner);
bool registerRect(jcEng * eng, jrect * r, jvec * v, juint groupNum, void * owner);

#endif
