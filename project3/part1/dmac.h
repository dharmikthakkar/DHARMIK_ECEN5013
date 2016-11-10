#ifndef _DMAC_H_
#define _DMAC_H_
#include "MKL25Z4.h"
#include <stdlib.h>
void dma_init();
void dma_use();
#define length 10
#define wordsend 0
char *a,*b,*d;

char c[1]={0};

uint32_t k,l;
int flag,flag1;
int time=0,count=0;
void dma_memmove(char *src,char *dest,uint32_t len);
void dma_memzero(char *dest,uint32_t len);
#endif