#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <SDL/SDL.h>

#define HEIGHT 512
#define BPP 4
#define DEPTH 32

unsigned int *buf; // input buffer


void Draw_pixel(SDL_Surface *screen, Uint32 x, Uint32 y, Uint32 color)
{
  Uint32 bpp, ofs;

  bpp = screen->format->BytesPerPixel;
  ofs = screen->pitch*y + x*bpp;

  SDL_LockSurface(screen);
  memcpy(screen->pixels + ofs, &color, bpp);
  SDL_UnlockSurface(screen);
}

#define SGN(x) ((x)>0 ? 1 : ((x)==0 ? 0:(-1)))
#define ABS(x) ((x)>0 ? (x) : (-x))

/* Basic unantialiased Bresenham line algorithm */
static void bresenham_line(SDL_Surface *screen, Uint32 x1, Uint32 y1, Uint32 x2, Uint32 y2,
			   Uint32 color)
{
  int lg_delta, sh_delta, cycle, lg_step, sh_step;

  lg_delta = x2 - x1;
  sh_delta = y2 - y1;
  lg_step = SGN(lg_delta);
  lg_delta = ABS(lg_delta);
  sh_step = SGN(sh_delta);
  sh_delta = ABS(sh_delta);
  if (sh_delta < lg_delta) {
    cycle = lg_delta >> 1;
    while (x1 != x2) {
      Draw_pixel(screen,x1, y1, color);
      cycle += sh_delta;
      if (cycle > lg_delta) {
	cycle -= lg_delta;
	y1 += sh_step;
      }
      x1 += lg_step;
    }
    Draw_pixel(screen, x1, y1, color);
  }
  cycle = sh_delta >> 1;
  while (y1 != y2) {
    Draw_pixel(screen,x1, y1, color);
    cycle += lg_delta;
    if (cycle > sh_delta) {
      cycle -= sh_delta;
      x1 += lg_step;
    }
    y1 += sh_step;
  }
  Draw_pixel(screen, x1, y1, color);
}


void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;  
 
    colour = SDL_MapRGB( screen->format, r, g, b );
  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}

#define TRIGGER 200
void DrawScreen(SDL_Surface* screen, int bufsize)
{ 
    int x, y,oldx,oldy;
    oldy=128;
    static int max=-9999;
    SDL_Rect srcrect,dstrect;

// Read data
int len=fread(buf,1,bufsize*sizeof(int),stdin);
if (len!=bufsize*sizeof(int))
	exit(0); // EOF
// Write data (to use as in pipe)
len = write (1,buf,bufsize*sizeof(int));

#define REDUCTION 800.0

// Draw data slow-oscilloscope
float foldx =0 ,fx =0;

srcrect.x = (bufsize/REDUCTION);
srcrect.y = 0;
srcrect.w = (bufsize-(bufsize/REDUCTION));
srcrect.h = HEIGHT;

dstrect.x = 0;
dstrect.y = 0;
dstrect.w = (bufsize-(bufsize/REDUCTION));
dstrect.h = HEIGHT;

SDL_BlitSurface(screen,&srcrect,screen,&dstrect);
SDL_Rect rect2 = {bufsize-(bufsize/REDUCTION),0,bufsize,HEIGHT};
SDL_FillRect( SDL_GetVideoSurface(), &rect2, 0 );

for(x = 0; x < bufsize; x+=1 ) {
	    y=buf[x]/10000+10;
	    if (y>=HEIGHT-1) y=HEIGHT-1;
	    fx=x/REDUCTION;
	    bresenham_line(screen, bufsize-fx, oldy, bufsize-foldx,y,0xffFF00);
	    foldx=fx;oldy=y;
	}

    SDL_Flip(screen); 
}




#define WIDTH 1024

int main(int argc, char* argv[])
{

int buflen=WIDTH;
buf= (int *) calloc(buflen,sizeof(int));

    SDL_Surface *screen;
    SDL_Event event;
  
    int keypress = 0;
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_SWSURFACE|SDL_RESIZABLE)))
    {
        SDL_Quit();
        return 1;
    }

 // Lock screen
    if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return 1;
    }

 // main loop
    while(1) 
    {
         DrawScreen(screen,WIDTH);
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
                  case SDL_QUIT:
	              keypress = 1;
	              break;
                  case SDL_KEYDOWN:
                       keypress = 1;
                       break;
              }
         }
    }

// unlock screen
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);  

    SDL_Quit();
  
    return 0;
}




