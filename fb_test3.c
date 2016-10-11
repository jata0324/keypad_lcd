#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>

#define FBDEVFILE "/dev/fb"

typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
  return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

int main()
{
  int fbfd, dev;
  int ret;
  struct fb_var_screeninfo fbvar;
  unsigned short pixel;
  int offset;
  int i;
  int two = 2;
  int r_val;
  char buff;

  /***************키패드 오픈*******************/
  dev = open("/dev/keypad_device", O_RDONLY);
  printf("dev:%d\n", dev);
  if(dev<0){
    printf("device open error\n");
    return -1;
  }

  /***************프레임버퍼 오픈****************/
  fbfd = open(FBDEVFILE, O_RDWR);
  if(fbfd < 0)
    {
      perror("fbdev open");
      exit(1);
    }

  ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
      if(ret < 0)
	{
	  perror("fbdev ioctl");
	  exit(1);
	}

      if(fbvar.bits_per_pixel != 16)
	{
	  fprintf(stderr, "bpp is not 16n");
	  exit(1);
	}



  //하얀색으로 밀어

  for(i=0; i<640*480-54; i++){
    offset = i*(16/8);
    if(lseek(fbfd, offset, SEEK_SET) < 0){
      perror("fbdev lseek");
      exit(1);
    }
    pixel = makepixel(255,255,255);
    write(fbfd, &pixel, 2);
  }

  //가운데 점하나 찍음
  
  offset = 240*fbvar.xres*(16/8)+320*(16/8);
  if(lseek(fbfd, offset, SEEK_SET) < 0)
    {
      perror("fbdev lseek");
      exit(1);
    }
  pixel = makepixel(0,0,0);
  write(fbfd, &pixel, 2);
  printf("out %d",offset);

  //이동시작
  while(1){

    if(read(dev,&buff,10)!=0){
      r_val = (int)buff;
       printf("in %d",offset);
      switch(r_val){
      case 4: //UP
	printf("UP4\n");
	offset = offset - fbvar.xres*(16/8);	
	break;
      case 6: //DOWN
	printf("DOWN6\n");
	offset = offset + fbvar.xres*(16/8);
	break;
      case 1: //RIGHT
	printf("RIGHT1\n");	
	offset=offset+2;
	break;
      case 9: //LEFT
	printf("LEFT9\n");
	offset=offset-2;
	break;
     
      }

      if(lseek(fbfd, offset, SEEK_SET) < 0){
	  perror("fbdev lseek");
	  exit(1);
      }
      pixel = makepixel(0,0,0);
      write(fbfd, &pixel, 2);

    }
  }
  close(dev);
  close(fbfd);
  exit(0);
  return 0;
}
