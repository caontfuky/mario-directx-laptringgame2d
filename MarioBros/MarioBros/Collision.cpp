#include "Collision.h"
#include "math.h"
#include <vector>
int Collision::RectVsCircle(RECT rect, float xO, float yO, float R)
{
	//AB
	//CD
	//Hình chữ nhật ABCD, đường tròn tâm O, bán kính R
	//Tìm điểm M, là điểm gần O nhất
	float xM, yM, xA, yA, xB, yB, xC, yC, xD, yD;
	xA=xD=rect.left;	
	xB=xC=rect.right;
	yA=yB=rect.top;
	yC=yD=rect.bottom;
	
	xM = MiddleNumber(xA, xO, xC);       //Tìm tọa độ M
	yM = MiddleNumber(yA, yO, yC);
	if (Distance(xM,yM,xO,yO) < R)            //Xảy ra va chạm
	{
		if (PointInLine(xM,yM,xA,yA,xB,yB)==0)                  //Nếu M nằm trên AB
		{
			if (PointInLine(xM,yM, xB, yB, xC, yC)==0) return 6;    //Va chạm tại B
			if(PointInLine(xM,yM,xA,yA,xD,yD)==0)return 5;        //Va chạm tại A
			else return 1;                         //Va chạm tại AB
		}
		if(PointInLine(xM,yM,xC,yC,xD,yD)==0)
		{
			if(PointInLine(xM,yM,xB,yB,xC,yC)==0)return 7;          //C
			if (PointInLine(xM,yM, xA, yA, xD,yD)==0) return 8;      //D
			else return 3;                              //CD
		}
		if (PointInLine(xM,yM, xB,yB, xC,yC)==0) return 2;          //BC
		if (PointInLine(xM,yM, xD,yD, xA,yA) == 0) return 4;
		else return 9;                              //Nằm trọn trong ABCD
	}
	else return 0;
}

int Collision::RectVsCircle(RECT rect, float xO1, float yO1,float xO2,float yO2, float R,int differential)
{
	float deltaX=(xO2-xO1)/differential;
	float deltaY=(yO2-yO1)/differential;
	for(char i=0;i<=differential;i++)
	{
		float xNew=xO1+deltaX*i,yNew=yO1+deltaY*i;
		int checkCollision=RectVsCircle(rect,xNew,yNew,R);
		if(checkCollision!=0)
			return checkCollision;
	}
	return 0;
}
float Collision::Distance(float xA, float yA,float xB, float yB)
{
	return sqrt((xA-xB)*(xA-xB)+(yA-yB)*(yA-yB));
}
int Collision::PointInLine(float xM,float yM, float xA, float yA, float xB, float yB)
{
	//Trả về 0 nếu M nằm trên AB, -1 hoặc 1 nếu ở một trong hai phía.
	float rValue;
	rValue = ((xA - xB) * (yM - yB) - (xM - xA) * (yA - yB));
	if (rValue == 0) return rValue;
	else return rValue / abs(rValue);  
}
float MiddleNumber(float a,float b,float c)
{
	if (b > a)                    //a,b
	{
		if (c > b)
			return b;       //a,b,c
		else if (c > a)
			return c;       //a,c,b
		else return a;      //c,a,b
	}
	else                        //b,a
	{
		if (c > a)
			return a;       //b,a,c
		else if (c > b)
			return c;       //b,c,a
		else return b;      //c,b,a
	}
}
float Collision::Symmetrical(float a, float mirror)
{
	return a+2*(mirror-a);
}
void Collision::SymmetricalPoint(float &xA,float &yA,float xM,float yM)
{
	xA=Symmetrical(xA,xM);
	yA=Symmetrical(yA,yM);
}
void Collision::SymmetricalLine(float &xM,float &yM,float xA, float yA,float a)
{
	//Phương trình đường thẳng Ax là y=ax+b1
	//Phương trình đường thẳng My, vuông góc với Ax, là y=-ax+b2
	float b1=yA-a*xA,b2=yM+a*xM;
	float xI=(b2-b1)/(2*a);
	float yI=(b1+b2)/2;
	xM=2*xI-xM;
	yM=2*yI-yM;
}
bool Collision::RectVsRect(RECT r1, RECT r2)
{
	return (r1.left<r2.right&&r1.right>r2.left&&r1.top<r2.bottom&&r1.bottom>r2.top);
}
bool Collision::RectVsRect(RECT r1Old, RECT r1, RECT r2Old,RECT r2,int differential)
{
	float deltaX1=(r1.left-r1Old.left)/differential;
	float deltaY1=(r1.top-r1Old.top)/differential;
	float deltaX2=(r2.left-r2Old.left)/differential;
	float deltaY2=(r2.top-r2Old.top)/differential;
	
	for(char i=0;i<=differential;i++)
	{
		RECT r1New, r2New;
		r1New.left=r1Old.left+deltaX1*i;
		r1New.right=r1Old.right+deltaX1*i;
		r1New.top=r1Old.top+deltaY1*i;
		r1New.bottom=r1Old.bottom+deltaY1*i;
		r2New.left=r2Old.left+deltaX2*i;
		r2New.right=r2Old.right+deltaX2*i;
		r2New.top=r2Old.top+deltaY2*i;
		r2New.bottom=r2Old.bottom+deltaY2*i;
		bool checkCollision=RectVsRect(r1New,r2New);
		if(checkCollision)
			return checkCollision;
	}
	return 0;
}