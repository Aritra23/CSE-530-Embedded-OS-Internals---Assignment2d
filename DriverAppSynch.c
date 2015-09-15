#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
void *threadWrite(void *arg);
void *threadRead(void * arg);
int main(int argc, char *argv[])
{
 /* Initialization */
    	pthread_t pth1;	// this is our thread identifier
    	pthread_t pth2;
	pthread_t pth3;	// this is our thread identifier
    	pthread_t pth4;
	pthread_t pth5;	// this is our thread identifier
	pthread_t pth6;

	/* Create worker thread */
	if ( argc = 4) 
	{
	pthread_create(&pth1,NULL,threadWrite,argv[1]);
    	pthread_create(&pth2,NULL,threadRead,NULL);
	pthread_create(&pth3,NULL,threadWrite,argv[2]);
	pthread_create(&pth4,NULL,threadRead,NULL);
	pthread_create(&pth5,NULL,threadWrite,argv[3]);
	pthread_create(&pth6,NULL,threadRead,NULL);
	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);
	pthread_join(pth3, NULL);
	pthread_join(pth4, NULL);
	pthread_join(pth5, NULL);
	pthread_join(pth6, NULL);
	}
	else
	{
	 printf(" You need to provide 3 strings as argument.. ");
	}
}

 void *threadWrite(void *arg)
{
     char *str;
	 str=(char*)arg;
 
     int fd,size,wc = 0;
     char write_buf[1];
 
      
     strcpy(write_buf,str);
   
    fd = open("/dev/DriverMain", O_RDWR);
 
    if ( fd == -1 )
    {
		perror("Unable to open..");
		wc = fd;
		exit(-1);
    }
    printf(" File opened: successfully.. \n");
    wc = write(fd,write_buf,1);

    if ( wc == -1 ) 
    {
              perror("Unable to write..");
              close(fd);
             exit(-1);
    }
   printf(" write:Operation Successful - %i characters\n", wc);
   
   close(fd);
   return NULL;
}

void *threadRead(void * arg)
 {
 int fop;
 char read_buf[16];
 int i=0;
 int rc = 0;
 
      printf("Read: entered\n");
      /* Device Open block */
 
      fop = open("/dev/DriverMain", O_RDWR);
     if ( fop == -1 )
    {
         perror("Read:Unable to Open..");
         rc = fop;
         exit(-1);
    }
	
    printf("file: opened: successfully\n");
    /* Issue a read */
    rc = read(fop, read_buf, 0);
    
	if ( rc == -1 ) 
   {
	     perror("Could not read from file");
	     close(fop);
	     exit(-1);
   }
 
   printf(" File: read: returning %d bytes!\n contents in the buffer: ", rc);
   for(i; i<rc; i++)
 
   {
	printf("%c",read_buf[i]);
   }
	printf("\n");
	close(fop);
	return NULL;
}
