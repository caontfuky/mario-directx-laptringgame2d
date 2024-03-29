#pragma once
#include "math.h"
#include <list>
using namespace std;

#include "Camera.h"
class QuadTree;
class Cloud;
class Sprite;
class Object;
class Graphic;
class Game;

void ReadMap(char* fileName,bool isBright,Game* game);
int* ReadIntFromFile(char* fileName);
void trace(const char* sLog,char* filePath="D:\\TraceLog.log");
void GetListItems(list<Object*>* lItems,list<Object*>* staObjs,list<Object*>* dynObjs);
void DrawNumber(Sprite* sprite,int number,int x,int y,char zoomX,char zoomY);
void DrawNumber(Sprite* sprite,int number,int x,int y);
void SaveGame(char* fileName,Game* game);
void LoadGame(char* fileName,Game* game);

/*char sTrace[10];
itoa(needTraceVar,sTrace,10);
trace(sTrace);*/
