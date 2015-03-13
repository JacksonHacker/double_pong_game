#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mytypes.h"


#define GAME_SPEED  20           //  Increase it to slow down frame rate

#define KEYDOWN(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0 )
#define KEYUP(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1 )


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define BALL_WIDTH  32
#define BALL_HEIGHT 32

#define BAT_HEIGHT  100
#define BAT_WIDTH  32
#define BAT_SPEED  5.0f


/**   Declare Variables      */

HWND hCurrentWindow;
HINSTANCE hThisInstance;
HDC game_dc;



Bat global_curBat;
Ball global_curBall;

DEVMODE game_screen;

int done=0;


HPEN color_pen=CreatePen(PS_SOLID,1,RGB(255,0,0));
HBRUSH color_brush=CreateSolidBrush(RGB(255,0,0));
HPEN black_pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
HBRUSH black_brush=CreateSolidBrush(RGB(0,0,0));


/*  Make the class name into a global variable  */
char szClassName[ ] = "MainWindow";


/**    Declare Functions       */

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/* Declare more Procedures    */
ATOM RegisterMyClass();
void GameInit();
void GameMain();
void GameQuit();



int WINAPI WinMain (HINSTANCE hTheInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    hThisInstance = hTheInstance;

   if(!RegisterMyClass())
       return GE_CLASS_NOT_REGISTERED;           /** ERROR : Class could not be registered */

    /* The class is registered, let's create the program*/
    HWND hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Double Pong...  :)",       /* Title Text */
           WS_POPUP | WS_VISIBLE, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WINDOW_WIDTH,                 /* The programs width */
           WINDOW_HEIGHT,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    if(!hwnd)
       return  GE_WINDOW_NOT_CREATED;        /** ERROR : Window could not be Created */

    hCurrentWindow=hwnd;

    GameInit();


    MSG messages;            /* Here messages to the application are saved */
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (!done)
    {
        DWORD start_tick = GetTickCount();
        if(PeekMessage(&messages,NULL,0,0,PM_REMOVE))
        {
                /* Translate virtual-key messages into character messages */
                 TranslateMessage(&messages);
               /* Send message to WindowProcedure */
                 DispatchMessage(&messages);
        }

        GameMain();


        if(KEYDOWN(VK_ESCAPE))
          SendMessage(hwnd, WM_CLOSE,0,0);

        //take care of frame rate
        while((GetTickCount()-start_tick)<GAME_SPEED)
            ;


    }

    GameQuit();

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

          case  WM_CLOSE:
        {
                    int res;
                    res= MessageBox(hCurrentWindow,"Are U sure to quit!!!!","Really quit???",MB_YESNO | MB_ICONQUESTION);
                    if(res==IDNO)
                      return 0;
                    done = 1;
                    PostQuitMessage(WM_QUIT);
       }
       break;
       case WM_DESTROY:
           PostQuitMessage(0);
           break;
       default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

ATOM RegisterMyClass()
{
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName =0 ;                      /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (HBRUSH) GetStockObject(BLACK_BRUSH);

    /* Register the window class, and if it fails quit the program */
    return RegisterClassEx (&wincl);
}


void GameInit()
{
    game_screen.dmSize = sizeof (game_screen);
    game_screen.dmPelsWidth=WINDOW_WIDTH;
    game_screen.dmPelsHeight=WINDOW_HEIGHT;
    game_screen.dmBitsPerPel=16;
    game_screen.dmFields= DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL ;
    ChangeDisplaySettings(&game_screen,CDS_FULLSCREEN);
   /* {
        MessageBox(hCurrentWindow,"Requested Video Mode NOT AVAILABLE!!!   :) ","OOPS!!!",MB_OK | MB_ICONERROR);
        exit(GE_FULLSCREEN_NOT_AVAILABLE);
    }*/


    global_curBat.posy = (WINDOW_HEIGHT/2)  - (BAT_HEIGHT/2);
    global_curBat.speedy = 0.0f;
    global_curBat.cury = (float) global_curBat.posy;

    global_curBall.posx = 50 ;
    global_curBall.posy = 100 ;
    global_curBall.curx = (float) global_curBall.posx;
    global_curBall.cury = (float) global_curBall.posy;
    global_curBall.speedx = 5.0f;
    global_curBall.speedy = 1.5f;

    game_dc = GetDC(hCurrentWindow);
}

void GameMain()
{
    Bat  curBat = global_curBat;

    curBat.cury += curBat.speedy;
    if(curBat.cury < 51.0f)
    {
        curBat.cury = 51.0f;
    }
    else if(curBat.cury > ((float)WINDOW_HEIGHT) - 50.0f)
    {
        curBat.cury = ((float)WINDOW_HEIGHT) - 50.0f;
    }
    curBat.posy = (int) curBat.cury;

    Ball curBall = global_curBall;

    curBall.cury += curBall.speedy;
    if(curBall.cury < 16.0f)
    {
        curBall.cury = 16.0f;
        curBall.speedy = - curBall.speedy;
    }
    else if(curBall.cury > ((float)WINDOW_HEIGHT - 16.0f))
    {
        curBall.cury = (float)WINDOW_HEIGHT - 16.0f;
        curBall.speedy = -curBall.speedy;
    }
    curBall.posy = (int) curBall.cury;

    curBall.curx += curBall.speedx;
    if((curBall.curx <= 42.0f) || (curBall.curx >= ((float)WINDOW_WIDTH - 42.0f)))
    {
        if( ((curBall.cury+16.0f) < (curBat.cury - 51.0f)) || ((curBall.cury - 16.0f) > (curBat.cury + 51.0f)))
        {
            done=1;
            MessageBox(hCurrentWindow,"You Lost... \n Try again Later.....   :) ","OOPS!!!",MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            curBall.speedx = -curBall.speedx;
            curBall.speedy = (curBall.cury - curBat.cury)/10.0f;
        }
    }
    curBall.posx = (int) curBall.curx;

    /**    ERASE BATS AND BALL */
    SelectObject(game_dc,black_pen);
    SelectObject(game_dc,black_brush);
    Ellipse(game_dc,0,(global_curBat.posy - 50),BAT_WIDTH,(global_curBat.posy + 50));
    Ellipse(game_dc,(WINDOW_WIDTH - BAT_WIDTH),(global_curBat.posy - 50),WINDOW_WIDTH,(global_curBat.posy + 50));
    Ellipse(game_dc,(global_curBall.posx - 16),(global_curBall.posy - 16),(global_curBall.posx + 16),(global_curBall.posy + 16));

    if(KEYDOWN(VK_UP))
       curBat.speedy = -  BAT_SPEED;
    else if(KEYDOWN(VK_DOWN))
       curBat.speedy =  BAT_SPEED;
    else
       curBat.speedy = 0.0f;

    global_curBall = curBall;
    global_curBat = curBat;

    /**    DRAW BATS AND BALL */
    SelectObject(game_dc,color_pen);
    SelectObject(game_dc,color_brush);
    Ellipse(game_dc,0,(global_curBat.posy - 50),BAT_WIDTH,(global_curBat.posy + 50));
    Ellipse(game_dc,(WINDOW_WIDTH - BAT_WIDTH),(global_curBat.posy - 50),WINDOW_WIDTH,(global_curBat.posy + 50));
    Ellipse(game_dc,(global_curBall.posx - 16),(global_curBall.posy - 16),(global_curBall.posx + 16),(global_curBall.posy + 16));

}


void GameQuit()
{
    DeleteObject(color_brush);
    DeleteObject(color_pen);
    DeleteObject(black_pen);
    DeleteObject(black_brush);
    ReleaseDC(hCurrentWindow,game_dc);

    ChangeDisplaySettings(NULL,NULL);    // return to Original display
}
