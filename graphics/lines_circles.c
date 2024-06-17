
// Overlapping z88dk graphics and sw implementation of lines and circles


#include <stdio.h>
#include <graphics.h>


void ddaAlg(int x1,int y1,int x2,int y2)
{
	int dx=x2-x1;
	int dy=y2-y1;
	int steps=dx>dy?dx:dy;
	float xInc=dx/(float)steps;
	float yInc=dy/(float)steps;
	float x=x1;
	float y=y1;
	for(int i=0;i<=steps;i++)
	{
		plot(x,y);
		x+=xInc;
		y+=yInc;
	}
}

void display(int xc,int yc,int x,int y)
{
	plot(xc+x, yc+y);
	plot(xc-x, yc+y);
	plot(xc+x, yc-y);
	plot(xc-x, yc-y);
	plot(xc+y, yc+x);
	plot(xc-y, yc+x);
	plot(xc+y, yc-x);
	plot(xc-y, yc-x);
}

void CircleB(int x1,int y1,int r)
{
	int x=0,y=r;
	int d=3-2*r;
	display(x1,y1,x,y);
	while(y>=x)
	{
		x++;
		if(d>0)
		{
			y--;
			d=d+4*(x-y)+10;
		}
		else
		{
			d=d+4*x+6;
		}
		display(x1,y1,x,y);
	}
}

int main() {

	int cx,cy;

	clg();

	cx=getmaxx()/2;
	cy=getmaxy()/2;

	circle(cx,cy,57,1);
	circle(cx,cy,57/2,1);

	CircleB(cx,cy,57);
	CircleB(cx,cy,57/2);

	draw(cx-48,cy-29,cx+48,cy-29);
	draw(cx-48,cy-29,cx,cy+56);
	draw(cx,cy+56,cx+48,cy-29);

	ddaAlg(cx-48,cy-29,cx+48,cy-29);
	ddaAlg(cx-48,cy-29,cx,cy+56);
	ddaAlg(cx,cy+56,cx+48,cy-29);

}

