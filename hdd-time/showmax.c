#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <SDL/SDL.h>

#define DEPTH 16

int main(int argc, char* argv[])
{
 unsigned int buf[DEPTH]; // input buffer
 unsigned int prom,c;
 // main loop
    while(1) 
    {
	// Read data
    int i;
	int len=fread(buf,1,sizeof(buf),stdin);
	if (len!=sizeof(buf))
		exit(0); // EOF
	prom=0;
	for (i=0;i<DEPTH;i++) {
        if (buf[i]>30000000) {
            if (i>0) fprintf(stderr,"-1: %i ns\n",buf[i-1]);
            fprintf(stderr," 0: %f ms\n",buf[i]/1000000.0);
            if (i<DEPTH) fprintf(stderr,"+1: %i ns\n",buf[i+1]);
            if (i<DEPTH-1) fprintf(stderr,"+2: %i ns\n",buf[i+2]);
            fprintf(stderr,"\n");
	        fwrite(&buf[i],1,sizeof(int),stdout);

            // Experimental time-proportional output
            /*
            int equivtime=buf[i]/500000;
            fprintf(stderr,"equivtime: %i \n",equivtime);
            for (c=0;c<equivtime;c++) {
	            fwrite(&buf[i],1,sizeof(int),stdout);
	            fwrite(&i,1,sizeof(int),stdout);
                }
            */
            }
		prom+=buf[i];
        }
	prom=prom/(DEPTH);
 //   prom*=200;
 //       if (prom>50) {		fprintf(stderr,"%d\n",prom);		}
	fwrite(&prom,1,sizeof(int),stdout);
    }

  
    return 0;
}




