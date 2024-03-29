#pragma once

#include "Animation.h"
#include "Sprite.h"
#include "Object.h"
#include <list>
using namespace std;

//Cây ăn thịt
class Pirhana:public Object
{
private:
	float	yOld;
	float	vy;
	char	state;
	bool	isNeedDelete;
	float	waitIdle;
public:
	Pirhana();
	Pirhana(float _x, float _y,float width,float height,int _ID,Sprite* _sprite);
	void Update(list<Object*>* staObjs,list<Object*>* dynObjs,Input* input, float TPF);
	void Render(Camera* camera);
	void ChangeState(char _state);
	void SetVar(char* varName,int val);
	int GetVar(char* varName);
	void ResetWhenReload(Sprite* _sprite);
	void ResetRect();
	void Save(fstream *fs);
	void Load(fstream *fs);	
};