#include "coll.h"

#include <stdbool.h>
#include <math.h>

#include <stdio.h>

bool solveQuadratic(jfloat a, jfloat b, jfloat c, jfloat *x)
{
    jfloat d = b * b - 4 * a * c;
    if (d < 0)
    {
        return false;
    }
    if (d == 0)
    {
        x[0] = -b / (2*a);
        x[1] = x[0];
        return true;
    }

    x[0] = (-b + sqrt(d))/(2*a);
    x[1] = (-b - sqrt(d))/(2*a);

    return true;
}

jfloat jvecDot(jvec a, jvec b)
{
    return a[0]*b[0] + a[1]*b[1];
}

bool circleLineCollDetect(jcircle ci, jvec p, jvec v, jfloat * t)
{
    jfloat a = jvecDot(v, v);
    jfloat b = 2 * (jvecDot(p, v) - jvecDot(ci.c, v));
    jfloat c = jvecDot(p, p) + jvecDot(ci.c, ci.c) - 2 * jvecDot(p, ci.c) - ci.r * ci.r;
    return solveQuadratic(a, b, c, t);
}

bool betweenPoints(jfloat a, jfloat b, jfloat c)
{
    if (a <= b && a >= c)
        return true;

    if (a >= b && a <= c)
        return true;

    return false;
}

bool getDesiredSolutionIfExtant(const jfloat * solns, jint num_solns, jfloat * t)
{
    // soln will contain the time of first collision (if
    // there was a collision)
    jfloat soln = -1;
    jint i;
    for (i = 0; i < num_solns; i++) // find the minimum value in solns
    {
        if (solns[i] >= 0 && (solns[i] < soln || soln < 0))
        {
            soln = solns[i];
        }
    }

    if (soln < 0 || soln > 1) // collision did not happen this timestep
    {
        return false;
    }

    *t = soln;
    return true;
}

bool circleWithAxisParallelSegCollDetect(jcircle c, jvec v, jvec b, jfloat h, AXIS ax, jfloat * tc)
{
    jfloat t[4]; // times when circle crosses extended line
    jfloat cp[2]; // two points where circle first touches extended line, and finishes touching it
    AXIS ax2 = (ax + 1) % NUM_AXIS;

    // calculate two times when circle first crosses extended line, and next crosses it
    t[0] = (b[ax2] - c.c[ax2] - c.r) / v[ax2];
    t[1] = (b[ax2] - c.c[ax2] + c.r) / v[ax2];

    // sort them so first element is earliest time
    if (t[0] > t[1])
    {
        jfloat temp = t[0];
        t[0] = t[1];
        t[0] = t[1];
    }

    // if the collision happened at a time either before or after this
    // frame there was no collision this frame
    if (t[0] > 1 || t[1] < 0)
    {
        return false;
    }

    // calculate point where extended line would first be intersected
    // by circle, and then where it would be last left by the circle
    cp[0] = c.c[ax] + t[0] * v[ax];
    cp[1] = c.c[ax] + t[1] * v[ax];

    // if the circle first intersects the extended line within the
    // boundaries of the line, and it happened in the time of the 
    // frame, then we know the time of first collision
    if (t[0] > 0 && betweenPoints(cp[0], b[ax], b[ax] + h))
    {
        *tc = t[0];
        return true;
    }

    // if reached this far then either the circle has either collided with
    // an endpoint of the line section, or hasn't hit the line segment
    // at all

    // place a test circle at the first endpoint (with radius same
    // as radius of circle) and find the times the vector of the
    // original circle's movement would intersect this test circle
    jcircle endPointCircle = {{b[0], b[1]}, c.r};
    juint num_solns = 0;
    if (circleLineCollDetect(endPointCircle, c.c, v, t))
    {
        num_solns += 2;
    }

    // do the same with the other endpoint of the line segment
    endPointCircle.c[ax] += h;
    if (circleLineCollDetect(endPointCircle, c.c, v, &t[num_solns]))
    {
        num_solns += 2;
    }

    return getDesiredSolutionIfExtant(t, num_solns, tc);
}

bool circleWithCircleCollDetect(jcircle c1, jvec v, jcircle c2, jfloat * t)
{
    jcircle c3 = {{c2.c[0], c2.c[1]}, c1.r + c2.r};
    jfloat tt[2];

    bool ret = circleLineCollDetect(c3, c1.c, v, tt);

    if (!ret)
        return ret;

    return getDesiredSolutionIfExtant(tt, 2, t);
}

bool circleWithRectCollDetect(jcircle c, jvec v, jrect r, jfloat * tc)
{
    jfloat h = r.tr[1] - r.bl[1];
    jfloat w = r.tr[0] - r.bl[0];
    jfloat t[4];
    int num_collisions = 0;

    if (circleWithAxisParallelSegCollDetect(c, v, r.bl, h, AXIS_Y, &t[num_collisions]))
    {
        num_collisions += 1;
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.tr, -h, AXIS_Y, &t[num_collisions]))
    {
        num_collisions += 1;
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.bl, w, AXIS_X, &t[num_collisions]))
    {
        num_collisions += 1;
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.tr, -w, AXIS_X, &t[num_collisions]))
    {
        num_collisions += 1;
    }

    if (num_collisions == 0)
    {
        return false;
    }

    return getDesiredSolutionIfExtant(t, num_collisions, tc);
}

/**
<<SMALL_SIDE>>                       <<LONG_SIDE>>
                         A
                        x-x
                 B    C     D    E
                x-x  x-x   x-x  x-x  
                         F
                    x---------x
                        ref 
                      x-----x
**/
typedef enum POS_TYPE
{
    A,
    B,
    C,
    D,
    E,
    F
} POS_TYPE;

POS_TYPE getPosType(uint32_t a, int32_t b1, int32_t b2)
{
    bool b1s = (b1 < 0);
    bool b2s = (b2 < 0);

    bool b1l = (b1 > a);
    bool b2l = (b2 > a);

    if (b1s)
    {
        if (b2s)
            return B;
        if (b2l)
            return F;
        return C;
    }

    if (b2l)
    {
        if (b1l)
            return E;
        return D;
    }

    return A;
}

oneDimCollObj oneDCollDetect(uint32_t a, int32_t b1, int32_t b2, int32_t v)
{
    POS_TYPE pt[] = {getPosType(a, b1, b2), getPosType(a, (int32_t)b1 + v, (int32_t)b2 + v)};

    if (pt[0] == B && pt[1] == B)
    {
        oneDimCollObj ret = {COLL_TYPE_NONE, -1, -1};
        return ret;
    }
    
    if (pt[0] == E && pt[1] == E)
    {
        oneDimCollObj ret = {COLL_TYPE_NONE, -1, -1};
        return ret;
    }

    if (pt[0] == B)
    {
        oneDimCollObj ret = {COLL_TYPE_SMALL_SIDE, -((float)b2) / (float)v, -1};
        return ret;
    }

    if (pt[0] == E)
    {
        oneDimCollObj ret = {COLL_TYPE_LONG_SIDE, (float)b1 / (float)v, -1};
        return ret;
    }

    oneDimCollObj ret = {COLL_TYPE_INSIDE, 999, 999};
    return ret;
}
