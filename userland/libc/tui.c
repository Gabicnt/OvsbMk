#include <tui.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
static void out(const char*s,int n){write(1,s,n);}
static void outs(const char*s){write(1,s,strlen(s));}
static int fmt_int(char*buf,int n){int p=0;if(n<0){buf[p++]='-';n=-n;}char tmp[12],*t=tmp;do{*t++='0'+(n%10);n/=10;}while(n);while(t>tmp)buf[p++]=*--t;return p;}
static void ansi_goto(int y,int x){char buf[24];int p=0;buf[p++]=0x1B;buf[p++]='[';p+=fmt_int(buf+p,y+1);buf[p++]=';';p+=fmt_int(buf+p,x+1);buf[p++]='H';out(buf,p);}
typedef struct{char ch;unsigned char attr;}cell_t;
static unsigned char mk_attr(int fg,int bg){return (unsigned char)((bg<<4)|fg);}
struct tui_win_s{int x,y,w,h,flags,cx,cy,scroll_off,scroll_ok,fg,bg,bold;cell_t*buf;int*dirty;char*title;};
struct tui_s{int w,h;tui_win_t**windows;int nwindows,cap;cell_t*composite;int*comp_dirty;cell_t*phys;};
tui_t *tui_init(void){
    tui_t*t=malloc(sizeof(tui_t));if(!t)return 0;
    t->w=80;t->h=25;t->windows=0;t->nwindows=0;t->cap=0;
    t->composite=malloc(80*25*sizeof(cell_t));t->phys=malloc(80*25*sizeof(cell_t));t->comp_dirty=malloc(25*sizeof(int));
    if(!t->composite||!t->phys||!t->comp_dirty){free(t->composite);free(t->phys);free(t->comp_dirty);free(t);return 0;}
    cell_t blank={' ',mk_attr(15,0)};for(int i=0;i<80*25;i++)t->composite[i]=blank;for(int i=0;i<80*25;i++)t->phys[i]=blank;for(int i=0;i<25;i++)t->comp_dirty[i]=1;
    outs("\x1b[?25l\x1b[2J");t->phys[0].ch=0;return t;
}
void tui_end(tui_t*t){if(!t)return;for(int i=0;i<t->nwindows;i++){free(t->windows[i]->buf);free(t->windows[i]->dirty);free(t->windows[i]->title);free(t->windows[i]);}free(t->windows);free(t->composite);free(t->phys);free(t->comp_dirty);outs("\x1b[2J\x1b[H\x1b[?25h");free(t);}
