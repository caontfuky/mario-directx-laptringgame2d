#pragma once

#include "Animation.h"
#include "Sprite.h"
#include "Object.h"
#include <list>
using namespace std;

//Là nấm khi Mario còn nhỏ, nếu Mario lớn rồi thì nó là hoa.
class Star:public Object
{
private:
	bool	isCheckedContainer;
	list<Object*>*	collisionList;
	float vx;
	float vy;
	float ay;
	float xOld;
	float yOld;
	RECT	oldRect;
	bool	isNeedDelete;
public:
	Object* container;
	char	state;

	Star();
	Star(float _x, float _y,float width,float height,int _ID,Sprite* _sprite);
	~Star();
	void Update(list<Object*>* staObjs,list<Object*>* dynObjs,Input* input, float TPF);
	void Render(Camera* camera);
	void CheckObjects(list<Object*>* staObjs);
	void Move(float TPF,list<Object*>* staObjs,list<Object*>* dynObjs);
	void ChangeState(char _state);
	void SetVar(char* varName,int val);
	int GetVar(char* varName);
	void ResetWhenReload(Sprite* _sprite);
	void Save(fstream *fs);
	void Load(fstream *fs);
};