#include "Castle.h"
#include "Object.h"
#include "Info.h"
#include "Animation.h"
#include "Sprite.h"

Castle::Castle():Object(){}
Castle::Castle(float _x, float _y,float width,float height,int _ID,Sprite* _sprite):Object(_x,_y,width,height)
{
	sprite	=_sprite;
	ID	=_ID;	
	ani=new Animation(3,2,0,sprite);
	ResetRect();
}
void Castle::Render(Camera *camera)
{
	sprite->Draw(x+TILE_SIZE-camera->x,y,ZOOM,ZOOM,ani->GetRect(0),0.5);		//Tường nhấp nhô
	sprite->Draw(x+2*TILE_SIZE-camera->x,y,ZOOM,ZOOM,ani->GetRect(0),0.5);
	sprite->Draw(x+3*TILE_SIZE-camera->x,y,ZOOM,ZOOM,ani->GetRect(0),0.5);

	sprite->Draw(x+TILE_SIZE-camera->x,y+TILE_SIZE,ZOOM,ZOOM,ani->GetRect(1),0.5);	//Cửa sổ
	sprite->Draw(x+2*TILE_SIZE-camera->x,y+TILE_SIZE,ZOOM,ZOOM,ani->GetRect(4),0.5);	//Gạch
	sprite->Draw(x+4*TILE_SIZE-camera->x,y+TILE_SIZE,-ZOOM,ZOOM,ani->GetRect(1),0.5);

	sprite->Draw(x+TILE_SIZE*0-camera->x,y+TILE_SIZE*2,ZOOM,ZOOM,ani->GetRect(0),0.5);
	sprite->Draw(x+TILE_SIZE*1-camera->x,y+TILE_SIZE*2,ZOOM,ZOOM,ani->GetRect(5),0.5);	//Tường nhấp nhô 2
	sprite->Draw(x+TILE_SIZE*2-camera->x,y+TILE_SIZE*2,ZOOM,ZOOM,ani->GetRect(5),0.5);
	sprite->Draw(x+TILE_SIZE*3-camera->x,y+TILE_SIZE*2,ZOOM,ZOOM,ani->GetRect(5),0.5);
	sprite->Draw(x+TILE_SIZE*4-camera->x,y+TILE_SIZE*2,ZOOM,ZOOM,ani->GetRect(0),0.5);

	sprite->Draw(x+TILE_SIZE*0-camera->x,y+TILE_SIZE*3,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*1-camera->x,y+TILE_SIZE*3,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*2-camera->x,y+TILE_SIZE*3,ZOOM,ZOOM,ani->GetRect(3),0.5);	//Cổng
	sprite->Draw(x+TILE_SIZE*3-camera->x,y+TILE_SIZE*3,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*4-camera->x,y+TILE_SIZE*3,ZOOM,ZOOM,ani->GetRect(4),0.5);

	sprite->Draw(x+TILE_SIZE*0-camera->x,y+TILE_SIZE*4,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*1-camera->x,y+TILE_SIZE*4,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*2-camera->x,y+TILE_SIZE*4,ZOOM,ZOOM,ani->GetRect(2),0.5);	//Cổng
	sprite->Draw(x+TILE_SIZE*3-camera->x,y+TILE_SIZE*4,ZOOM,ZOOM,ani->GetRect(4),0.5);
	sprite->Draw(x+TILE_SIZE*4-camera->x,y+TILE_SIZE*4,ZOOM,ZOOM,ani->GetRect(4),0.5);
}
void Castle::ResetWhenReload(Sprite* _sprite)
{
	sprite=_sprite;
	ani=new Animation(3,2,0,sprite);
}
void Castle::Save(fstream *fs)
{
	fs->write(reinterpret_cast<char*>(&ID),sizeof(ID));	//int ID, để khi load biết được đang load Obj nào
	int sizeObj=sizeof(*this);
	fs->write(reinterpret_cast<char*>(&sizeObj),sizeof(int));			//int sizeof(obj)
	fs->write(reinterpret_cast<char*>(this),sizeObj);
}
void Castle::Load(fstream *fs)
{
	int sizeObj;
	fs->read(reinterpret_cast<char*>(&sizeObj),sizeof(int));			//int sizeof(obj)
	fs->read(reinterpret_cast<char*>(this),sizeObj);
}
void Castle::ResetRect()
{
	rect.left=x+TILE_SIZE*2;
	rect.right=rect.left+TILE_SIZE;
	rect.top=y;
	rect.bottom=y+5*TILE_SIZE;
}