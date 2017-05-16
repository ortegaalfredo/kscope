#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <SDL/SDL.h>

#define DEPTH 3

int main(int argc, char* argv[])
{
 unsigned int buf[DEPTH]; // input buffer
 unsigned int prom;
 // main loop
    while(1) 
    {
	// Read data
	int len=fread(buf,1,sizeof(buf),stdin);
	if (len!=sizeof(buf))
		exit(0); // EOF
	prom=0;
	for (int i=0;i<DEPTH;i++)
		prom+=buf[i];
	prom=prom/DEPTH;
       if (prom>50) {
		//fprintf(stderr,"%d\n",prom);
		fwrite(&prom,1,sizeof(int),stdout);
		}
    }

  
    return 0;
}




