#pragma once

#include "d3dx9.h"

#include <list>
#include <fstream>
using namespace std;

#include "Camera.h"

class QuadTree;
class Sprite;
class Animation;
class Input;
class Game;
class Object
{
public:

	//Thông tin cơ bản
	float	x;				//Tọa độ x trong màn chơi
	float	y;				//Tọa độ y trong màn chơi
	float	width;				//Chiều rộng trên màn hình (pixel)
	float	height;				//Chiều rộng trên màn hình (pixel)

	int		ID;				//Mã của từng loại đối tượng.
	char	Huong;//k

	RECT	rect;
	QuadTree* oldNode;
	Game*	game;
	
	//Thông tin render
	Sprite* sprite;
	Animation* ani;

	Object();
	Object(float _x, float _y, float _width, float _height);
	~Object();
	
	virtual void	ResetRect();
	//Hàm cấp phát lại Sprite* và Animation* trong mỗi đối tượng, sau khi load từ file save.
	virtual void	ResetWhenReload(Sprite* _sprite){};						//Sử dụng khi load map
	virtual void	SetVar(char* varName,int val){};
	virtual int		GetVar(char* varName){return -1;};
	virtual void	Update(list<Object*>* staObjs,list<Object*>* dynObjs,Input* input, float TPF){};
	virtual void	Render(Camera* camera){};
	virtual	void	Save(fstream *fs){};
	virtual void	Load(fstream *fs){};
};