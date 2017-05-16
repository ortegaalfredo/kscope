#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define BLKFLSBUF  _IO(0x12,97)	/* flush buffer cache */


#define DISK_BUF_BYTES 64

int c = 0;
unsigned char *buf;
int fd;

void flush_buffer_cache (int fd)
{
	sync();
	fsync(fd);				/* flush buffers */
	fdatasync(fd);				/* flush buffers */
	sync();
	if (ioctl(fd, BLKFLSBUF, NULL))		/* do it again, big time */
		perror("BLKFLSBUF failed");
	sync();
}

static void *prepare_timing_buf (unsigned int len)
{
	unsigned int i;
	unsigned char *buf;

	buf = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (buf == MAP_FAILED) {
		perror("could not allocate timing buf");
		return NULL;
	}
	for (i = 0; i < len; i += 4096)
		buf[i] = 0; /* guarantee memory is present/assigned */
	if (-1 == mlock(buf, len)) {
		perror("mlock() failed on timing buf");
		munmap(buf, len);
		return NULL;
	}
	mlockall(MCL_CURRENT|MCL_FUTURE); // don't care if this fails on low-memory machines
	sync();

	/* give time for I/O to settle */
	sleep(1);
	return buf;
}

inline void task(void)	{
	int i;
	//ioctl(fd, BLKFLSBUF, NULL);
	i=read(fd,buf,DISK_BUF_BYTES);
	/* access all sectors of buf to ensure the read fully completed */
	//for (i = 0; i < DISK_BUF_BYTES; i += 512)
	//	buf[i] &= 1;
	}



int main(int argc, char **argv, char **arge) {
  int cont2;
  struct timespec tps, tpe;
  cont2=0;
  buf = prepare_timing_buf(DISK_BUF_BYTES);
  //fd=open("/dev/sda",O_RDONLY | O_DIRECT | O_SYNC);
  fd=open("/dev/sda",O_RDONLY );
  flush_buffer_cache(fd);
  unsigned int i;
  while (1) {
	  //fprintf(stderr,"%d: %d\n",cont2,i);
  	  clock_gettime(CLOCK_REALTIME, &tps);
	  task();
	  clock_gettime(CLOCK_REALTIME, &tpe);
	  i = tpe.tv_nsec-tps.tv_nsec;
	  if (i<100000000)
		  fwrite(&i,sizeof(int),1,stdout);
	  cont2++;
	  tps=tpe;
  	}
  return 0;
}
