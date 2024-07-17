
/*

	Testing the BGI macro set with a Turbo C style program

	https://www.javatpoint.com/computer-graphics-programs
	"Write a Program to print your name in Hindi script on console output in C."


	zcc +ts2068  -DGFXSCALEX=4/5 -DGFXSCALEY=2/5 -create-app -lm -pragma-define:CLIB_DEFAULT_SCREEN_MODE=6  hindi.c

*/

#include<stdio.h>  
#include<conio.h>  
// #include<graphics.h>   <-- the only thing we need to change, we are going to use <bgi.h>
#include<bgi.h>  

void main()  
{     
    int gd=DETECT,gm;  
    initgraph(&gd,&gm,"c:\\tc\\bgi");  
    setbkcolor(9);  
    line(100,100,370,100);  
      
    line(120,100,120,170);  
    arc(143,100,0,180,23);  
    line(165,100,165,155);  
    arc(150,155,100,0,15);  
    line(180,100,180,170);  
  
    circle(210,140,10);  
    line(210,130,250,130);  
  
    circle(280,140,10);  
    line(280,130,330,130);  
    line(330,100,330,170);  
    line(345,100,345,170);  
    ellipse(337,100,0,180,9,18);  
    getch();  
}  
