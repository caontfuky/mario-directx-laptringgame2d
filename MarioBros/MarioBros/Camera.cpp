#pragma once
#include "Camera.h"
#include "Object.h"
#include "QuadTree.h"

Camera::Camera(float _x, float _y, float _width, float _height)
{
	x	=_x;
	y	=_y;
	width	=_width;
	height	=_height;
	vx		=0;
	ResetRect();
}
void Camera::Update(float TPF,QuadTree* quadTree)
{
	if(x+width<quadTree->rect.right)
	{
		if(obj->x>x+width/3)			//Nếu đối tượng đi quá 1/3 màn hình ...
			x=obj->x-width/3;			//... thì Camera dịch chuyển theo
		ResetRect();
	}
	else x=quadTree->rect.right-width;
}
void Camera::ResetRect()
{
	rect.left=x;
	rect.right=x+width;
	rect.top=y;
	rect.bottom=y+height;
}