#ifndef __MYTYPES__H_
#define __MYTYPES__H_

typedef enum tagGameErrors
{
    GE_NO_ERROR                      =  0,
    GE_CLASS_NOT_REGISTERED          =  22,
    GE_WINDOW_NOT_CREATED            =  23,
    GE_FULLSCREEN_NOT_AVAILABLE      =  24

}GameErrors;

typedef enum tagTrueFalse
{
    MY_FALSE =0,
    MY_TRUE  =1
}TrueFalse;

typedef struct tagBat
{
    int  posy;           /* y co-ord of the Bat   */
    float  cury;
    float  speedy;                 /* speed of the bat up/down (+/- speed)    */

}Bat;      /* Both Players and the Enemis will have the same struct */


typedef struct tagBall
{
    int posx,posy;      /* x and y co-ords of the Bat   */
    float curx, cury;
    float speedx, speedy;           /* speed of the ball leftright(+/- speed) and up/down (+/- speed)   */

}Ball;

#endif
