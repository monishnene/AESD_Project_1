/*********************
*floattostring
*Author: Sanika Dongre
*Date created: 03/30/19
* Refernce: https://www.geeksforgeeks.org/gcvt-convert-float-value-string-c/
***********************/
#include <stdio.h> 
#define MAX 100 
  
int main() 
{ 
    float x = 123.4567; 
    char buf[MAX]; 
  
    gcvt(x, 6, buf); 
  
    printf("buffer is: %s\n", buf); 
  
    return 0; 
} 
