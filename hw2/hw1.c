#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "hw1.h"
char *reversestr;


char reverse( char *str, int length)
{

      int i;
      for(i=length-1;i>=0;i--)
      {
             *reversestr++=*(str+i);
      }
     
}


void main()
{
     int j;
      char arevstr[100];
        reversestr=arevstr;
      char astr[]="Does it reverse \n\0\t correctly?\0";
      char *str1;
      printf("\n The string in astr is %s\n",astr);
     
      str1=astr;
      reverse(str1,strlen(astr));

      reversestr-=strlen(astr);

       printf("The reverse string is:");
      for(j=0;j<=strlen(astr);j++)
      {
          
             printf("%c",*(reversestr+j));
      }
      printf("\n");   
}
