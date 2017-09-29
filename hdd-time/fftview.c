#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <SDL/SDL.h>
#include <fftw3.h>

#define HEIGHT 256
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
fftw_complex *in, *out;
fftw_plan p;
void DrawScreen(SDL_Surface* screen, int bufsize)
{ 
    int x, y,oldx;
    oldx=0;
    static int max=-9999;
    int triggered=0;

    int k;
    SDL_Rect srcrect,dstrect;

	


// Clean
//SDL_FillRect( SDL_GetVideoSurface(), NULL, 0 );

// Trigger
triggered=1; // always triggered
while(!triggered) {
	int len=fread(buf,1,sizeof(int),stdin);
	if (len!=sizeof(int))
		exit(0); // EOF
	y=buf[0]/256+128;
	if (y>TRIGGER) triggered=1;
	}


// Read data
int len=fread(buf,1,bufsize*sizeof(int),stdin);
if (len!=bufsize*sizeof(int))
	exit(0); // EOF
// Write data (to use as in pipe)
len = write (1,buf,bufsize*sizeof(int));

// Convert data



//    for(k=0;k<bufsize;k++) {in[k][0]=0;in[k][1]=0;}  
//    for(k=0;k<bufsize/8;k++) {in[k][0]=2000;in[k][1]=0;}

    for(k=0;k<bufsize;k++) {in[k][0]=buf[k] ;in[k][1]=0;}
    in[0][0]=0;
 //   for(k=0;k<bufsize;k++) {fprintf(stderr, "%f ",in[k][0]);}
    

    fftw_execute(p);
   // fprintf(stderr, "\n-------------------------\n");
    //for(k=0;k<bufsize;k++) {fprintf(stderr, "%f ",out[k][0]);}
    //exit(0);
 

    double maxout=-999,power;   
/*
    for(k=0;k<bufsize;k++) {
			power=sqrt(out[k][0]*out[k][0]+out[k][1]*out[k][1]);
			if (maxout<power) maxout=power/100;
			}
*/
    maxout=200;
    for(k=0;k<bufsize;k++) {
			buf[k]=(sqrt(out[k][0]*out[k][0]+out[k][1]*out[k][1])/maxout);
			}

 //   fprintf(stderr, "\n-aa------------------------\n");
   //for(k=0;k<bufsize;k++) {fprintf(stderr, "%d ",buf[k]);}

    bufsize/=2; // do not draw negative freqs.

// Draw data
    for(x = 0; x < bufsize; x++ ) {
	    y=buf[x];
	    if (abs(y)>max) max=abs(y);
	    bresenham_line(screen, oldx*2, 255, x*2,255,0x00+y);
	    oldx=x;
		}

    dstrect.x = 0;dstrect.y = 0;
    srcrect.x = 0;srcrect.y = 1;
    dstrect.w = srcrect.w=bufsize*2;dstrect.h=srcrect.h = 255;

    SDL_BlitSurface(screen,&srcrect,screen,&dstrect);

    SDL_Flip(screen); 

}

#define WIDTH 512

int main(int argc, char* argv[])
{

int buflen=WIDTH;
buf= (unsigned int *) calloc(buflen,sizeof(int));

    SDL_Surface *screen;
    SDL_Event event;
  
  
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

    // Initialize fftw
    in= (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*WIDTH);
    out=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*WIDTH);   
    p = fftw_plan_dft_1d(WIDTH, in,out,FFTW_FORWARD, FFTW_ESTIMATE);

 // main loop
    while(1) 
    {
         DrawScreen(screen,WIDTH);
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
                  case SDL_QUIT:
	              break;
                  case SDL_KEYDOWN:
                       break;
              }
         }
    }

    // Free ffttw
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

// unlock screen
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);  

    SDL_Quit();
  
    return 0;
}




