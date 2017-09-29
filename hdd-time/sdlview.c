#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <SDL/SDL.h>

#define HEIGHT 300
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

void DrawPulse(SDL_Surface* screen, unsigned int *buf,int bufsize)
{
int i,c,color,r,g,b;

// Darken screen
  Uint32 ofs,bpp;
  bpp = screen->format->BytesPerPixel;
  SDL_LockSurface(screen);
  for(i=0;i<100;i++)
    for(c=0;c<300;c++) {
        ofs = screen->pitch*c + i*bpp;
        color=*((int *)(screen->pixels+ofs));
        r=(color>>16)&0xff;g=(color>>8)&0xff;b=color&0xff;
        if (r>10) r-=10;if (g>10) g-=10;if (b>10) b-=10;
        color=(r<<16)+(g<<8)+b;
        *((int *)(screen->pixels+ofs))=color;
        }
  SDL_UnlockSurface(screen);

for(i=5;i<bufsize-5;i++) {
    if (buf[i]>30000000) {
        for (c=i-3;c<i+5;c++) {
            SDL_Rect rect2 = {(c-(i-3))*10-3,300-log2(buf[c])*10-3,6,6};
            SDL_FillRect( SDL_GetVideoSurface(), &rect2, 0xFF0000);
            //fprintf(stderr,"%d %f %d %f\n",(c-(i-3))*10,log(buf[c]),(c-(i-3))*10+10,log(buf[c+1]));
	        bresenham_line(screen, (c-(i-3))*10, 300-log2(buf[c])*10,(c-(i-3))*10+10, 300-log2(buf[c+1])*10,0xFFFFFF);
            }
        }
    }
}

int divisor=1;
void DrawScreen(SDL_Surface* screen, int bufsize)
{ 
    unsigned int x, y,oldy;
    oldy=128;
    SDL_Rect srcrect,dstrect;

// Read data
int len=fread(buf,1,bufsize*sizeof(int),stdin);
if (len!=bufsize*sizeof(int))
	exit(0); // EOF
// Write data (to use as in pipe)
len = write (1,buf,bufsize*sizeof(int));

#define REDUCTION 256.0
#define REDUCTIONY 524288
#define WIDTH 1024
// Draw data slow-oscilloscope
float foldx =0 ,fx =0;

//srcrect.x = 100+(bufsize/REDUCTION);
srcrect.x = 100+1;
srcrect.y = 0;
//srcrect.w = (bufsize-(bufsize/REDUCTION))+100;
srcrect.w = (bufsize-1);
srcrect.h = HEIGHT;

dstrect.x = 100;
dstrect.y = 0;
//dstrect.w = (bufsize-(bufsize/REDUCTION))+100;
dstrect.w = (bufsize-1);
dstrect.h = HEIGHT;

SDL_BlitSurface(screen,&srcrect,screen,&dstrect);
//SDL_Rect rect2 = {bufsize-(bufsize/REDUCTION),0,bufsize,HEIGHT};
SDL_Rect rect2 = {bufsize-2,0,2,HEIGHT};
SDL_FillRect( SDL_GetVideoSurface(), &rect2, 0 );

// Detect and draw pulse
DrawPulse(screen,buf,bufsize);

// Draw rolling scope
for(x = 0; x < bufsize; x+=1 ) {
	    y=buf[x]/REDUCTIONY;
	    if (y>=HEIGHT-1) y=HEIGHT-1;
	    fx=x/REDUCTION;
	    int width = abs(y-oldy);
	    if (width>=HEIGHT) width=HEIGHT-1;
	    bresenham_line(screen, -1+bufsize-fx, (HEIGHT/2)-(width/2),-1+bufsize-foldx, (HEIGHT/2)+(width/2),0xFFFF00);
        if ((-1+bufsize-foldx)+4<bufsize)
    	    bresenham_line(screen, (-1+bufsize-fx)-1, (HEIGHT/2)-(width/3),(-1+bufsize-foldx)-2, (HEIGHT/2)+(width/3),0xFFFF00);
	    foldx=fx;oldy=y;
	}
// Draw scope
SDL_Rect rect3 = {0,HEIGHT,bufsize-2,HEIGHT/2};
SDL_FillRect( SDL_GetVideoSurface(), &rect3, 0 );
oldy=HEIGHT*1.3;
for(x = 0; x < bufsize-1; x+=1 ) {
	    y=(HEIGHT*1.3)-log2(buf[x])*10;
	    if (y>=HEIGHT*1.3) y=HEIGHT*1.3;
	    if (y<HEIGHT) y=HEIGHT;
	    bresenham_line(screen, x, oldy,x+1, y,0x00FF00);
	    oldy=y;
        }

    SDL_Flip(screen); 
}





int main(int argc, char* argv[])
{

int buflen=WIDTH;
buf= (unsigned int *) calloc(buflen,sizeof(int));

    SDL_Surface *screen;
    SDL_Event event;
  
    int keypress = 0;
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT*1.31, DEPTH, SDL_SWSURFACE|SDL_RESIZABLE)))
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
    while(!keypress) 
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




