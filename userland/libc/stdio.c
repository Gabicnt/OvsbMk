#include <stdio.h>
#include <stdarg.h>
#include <string.h>
FILE __stdin_file={.fd=0},__stdout_file={.fd=1},__stderr_file={.fd=2};
static long _syscall(long n,long a1,long a2,long a3,long a4){long r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"D"(a1),"S"(a2),"d"(a3),"c"(a4):"r11","memory");return r;}
int open(const char*p,int f){return _syscall(5,(long)p,f,0,0);}
int close(int f){return _syscall(6,f,0,0,0);}
int read(int f,void*b,int c){return _syscall(3,f,(long)b,c,0);}
int write(int f,const void*b,int c){return _syscall(4,f,(long)b,c,0);}
int lseek(int f,int o,int w){return _syscall(200,f,o,w,0);}
int unlink(const char*p){return _syscall(10,(long)p,0,0,0);}
int mkdir(const char*p){return _syscall(136,(long)p,0,0,0);}
int rmdir(const char*p){return _syscall(137,(long)p,0,0,0);}
int stat(const char*p,struct stat*b){return _syscall(188,(long)p,(long)b,0,0);}
int fstat(int f,struct stat*b){return _syscall(189,f,(long)b,0,0);}
int kbhit(void){return _syscall(198,0,0,0,0);}
void putchar(char c){write(1,&c,1);}
void puts(const char*s){write(1,s,strlen(s));write(1,"\n",1);}
char getchar(void){char c;read(0,&c,1);return c;}
char *gets(char*buf){int i=0;while(1){char c=getchar();if(c=='\n')break;if(c=='\b'){if(i>0)i--;continue;}buf[i++]=c;}buf[i]=0;return buf;}
