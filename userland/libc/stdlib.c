#include <stdlib.h>
static long _syscall(long num, long a1, long a2, long a3, long a4) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "D"(a1), "S"(a2), "d"(a3), "c"(a4) : "r11", "memory");
    return ret;
}
int atoi(const char *s) { int n=0,sign=1; while(*s==' ')s++; if(*s=='-'){sign=-1;s++;}else if(*s=='+')s++; while(*s>='0'&&*s<='9'){n=n*10+(*s-'0');s++;} return sign*n; }
char *itoa(int n,char*buf){char*p=buf;unsigned u;if(n<0){*p++='-';u=-n;}else u=n;char tmp[12],*t=tmp;do{*t++='0'+(u%10);u/=10;}while(u);while(t>tmp)*p++=*--t;*p=0;return buf;}
void exit(int code){_syscall(1,code,0,0,0);for(;;)__asm__("hlt");}
