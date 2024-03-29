#include "QuadTree.h"
#include "Object.h"

QuadTree::QuadTree(int x,int y,int size,char _level)
{
	RECT r={x,y,x+size,y+size};
	rect=r;
	level=_level;
	topLeft=topRight=bottomLeft=bottomRight=NULL;
}
QuadTree::~QuadTree()
{
	if(topLeft!=NULL) delete topLeft;
	topLeft=NULL;
	if(topRight!=NULL) delete topRight;
	topRight=NULL;
	if(bottomLeft!=NULL) delete bottomLeft;
	bottomLeft=NULL;
	if(bottomRight!=NULL) delete bottomRight;
	bottomRight=NULL;
	list<Object*>::iterator i;
	for(i=staticObj.begin();i!=staticObj.end();i++)
	{
		if((*i)!=NULL) delete *i;
		(*i)=NULL;
	}
	for(i=dynamicObj.begin();i!=dynamicObj.end();i++)
	{
		if((*i)!=NULL) delete *i;
		(*i)=NULL;
	}
}

QuadTree** QuadTree::GetChildNodeFromChildRect(char childRectType)
{
	switch (childRectType)
	{
	case TR_TL:
		return &topLeft;
	case TR_TR:
		return &topRight;
	case TR_BL:
		return &bottomLeft;
	case TR_BR:
		return &bottomRight;
	default:
		return NULL;
	}
}
bool QuadTree::Add(Object* obj, bool isStatic)
{
	//Nếu Rect của object không thuộc thì không thêm
	if(obj==NULL) return false;
	char childRectType=CheckInChildRect(obj->rect);
	if(childRectType==TR_NONE||level>=MAX_LEVEL)			//Nếu không thể chứa vào hình chữ nhật con
	{
		if(isStatic==true)
			staticObj.push_back(obj);
		else dynamicObj.push_back(obj);
		obj->oldNode=this;
		return true;
	}
	else
	{
		RECT r;//=GetChildRect(rect,childRect);
		QuadTree** qChild=GetChildNodeFromChildRect(childRectType);
		if(*qChild==NULL)
		{
			r=GetChildRect(rect,childRectType);
			(*qChild)=new QuadTree(r.left,r.top,r.right-r.left,level+1);
		}
		(*qChild)->Add(obj,isStatic);
	}
}

void QuadTree::Update(/*list<Object*> l*/)
{
	list<Object*> l;
	GetObjectsAllNode(&l,false);
	list<Object*>::iterator i;
	for(i=l.begin();i!=l.end();i++)
	{
		Object* obj=(*i);
		QuadTree* oldNode=obj->oldNode;
		if(!oldNode->IsExistDynamic(obj))break;		//Nếu object không còn tồn tại thì bỏ qua
		if(!oldNode->IsContainable(obj))			//Nếu object không thể chứa được trong list cũ
		{
			oldNode->dynamicObj.remove(obj);
			Add(obj,false);
		}
	}
}
void QuadTree::Update(list<Object*> l)
{
	int n=l.size();
	if(n==0)
		n=1;
	list<Object*>::iterator i;
	for(i=l.begin();i!=l.end();i++)
	{
		Object* obj=(*i);
		if(obj->ID==40)
			n=1;
		QuadTree* oldNode=obj->oldNode;
		if(!oldNode->IsExistDynamic(obj))break;		//Nếu object không còn tồn tại thì bỏ qua
		if(!oldNode->IsContainable(obj))			//Nếu object không thể chứa được trong list cũ
		{
			oldNode->dynamicObj.remove(obj);
			Add(obj,false);
		}
	}
}
//Kiểm tra một hình chữ nhật có thuộc rect con nào không.
char QuadTree::CheckInChildRect(RECT rInner)
{
	if(RectInRect(rInner,GetChildRect(rect,TR_TL))) return TR_TL;
	if(RectInRect(rInner,GetChildRect(rect,TR_TR))) return TR_TR;
	if(RectInRect(rInner,GetChildRect(rect,TR_BL))) return TR_BL;
	if(RectInRect(rInner,GetChildRect(rect,TR_BR))) return TR_BR;
	else return TR_NONE;
}

void QuadTree::GetObjectsInNode(list<Object*>* staticList,list<Object*>* dynamicList)
{
	staticList->insert(staticList->end(),staticObj.begin(),staticObj.end());
	dynamicList->insert(dynamicList->end(),dynamicObj.begin(),dynamicObj.end());
}
void QuadTree::GetObjectsInNode(list<Object*>* l,bool isStatic) 
{
	if(isStatic==true)
		l->insert(l->end(),staticObj.begin(),staticObj.end());
	else
		l->insert(l->end(),dynamicObj.begin(),dynamicObj.end());
}
void QuadTree::GetObjectsAllNode(list<Object*>* l,bool isStatic)
{
	GetObjectsInNode(l,isStatic);
	if(topLeft!=NULL) topLeft->GetObjectsAllNode(l,isStatic);
	if(topRight!=NULL) topRight->GetObjectsAllNode(l,isStatic);
	if(bottomLeft!=NULL) bottomLeft->GetObjectsAllNode(l,isStatic);
	if(bottomRight!=NULL) bottomRight->GetObjectsAllNode(l,isStatic);
}
//Node - TL - TR - BL - BR
void QuadTree::GetObjectsAllNode(list<Object*>* staticList,list<Object*>* dynamicList)
{
	GetObjectsInNode(staticList,dynamicList);
	if(topLeft!=NULL) topLeft->GetObjectsAllNode(staticList,dynamicList);
	if(topRight!=NULL) topRight->GetObjectsAllNode(staticList,dynamicList);
	if(bottomLeft!=NULL) bottomLeft->GetObjectsAllNode(staticList,dynamicList);
	if(bottomRight!=NULL) bottomRight->GetObjectsAllNode(staticList,dynamicList);
}
void QuadTree::GetObjectsFromCamera(RECT rCamera,list<Object*>* staticList,list<Object*>* dynamicList)
{
	char childRect=CheckInChildRect(rCamera);
	if(childRect==TR_NONE||level>=MAX_LEVEL)
	{
		GetObjectsAllNode(staticList,dynamicList);
	}
	else
	{
		GetObjectsInNode(staticList,dynamicList);
		QuadTree** qChild=GetChildNodeFromChildRect(childRect);
		if(*qChild!=NULL) (*qChild)->GetObjectsFromCamera(rCamera,staticList,dynamicList);
	}

}
bool QuadTree::IsExistDynamic(Object* obj)
{
	list<Object*>::iterator i;
	for(i=dynamicObj.begin();i!=dynamicObj.end();i++)
		if(*i==obj) return true;
	return false;
}
bool QuadTree::IsContainable(Object* obj)
{
	//RECT của Object thuộc Rect của Quadtree nhưng không thuộc rect con thì return true
	if(RectInRect(obj->rect,rect)&&CheckInChildRect(obj->rect)==TR_NONE)
		return true;
	else return false;
}
bool RectInRect(RECT rInner,RECT outerRect)
{
	return (rInner.left>=outerRect.left&&rInner.right<=outerRect.right
		&&rInner.top>=outerRect.top&&rInner.bottom<=outerRect.bottom);
}

RECT GetChildRect(RECT rect,int typeRect)
{
	int vCenterLine=(rect.left+rect.right)/2;
	int hCenterLine=(rect.top+rect.bottom)/2;
	RECT r;
	switch(typeRect)
	{
	case TR_TL:					//TypeRect TopLeft
		r.left=rect.left; r.top=rect.top; r.right=vCenterLine; r.bottom=hCenterLine;
		return r;
	case TR_TR:
		r.left=vCenterLine; r.top=rect.top; r.right=rect.right; r.bottom=hCenterLine;
		return r;
	case TR_BL:
		r.left=rect.left; r.top=hCenterLine; r.right=vCenterLine; r.bottom=rect.bottom;
		return r;
	case TR_BR:
		r.left=vCenterLine; r.top=hCenterLine; r.right=rect.right; r.bottom=rect.bottom;
		return r;
	}
}
void QuadTree::DeleteObj(Object* obj,bool isStatic)
{
	if(obj->oldNode==this)
	{
		Object* o;
		list<Object*>::iterator i;
		if(isStatic==true)
		{
			for(i=staticObj.begin();i!=staticObj.end();i++)
			{
				o=*i;
				if(o==obj)
				{
					staticObj.remove(obj);
					if(obj!=NULL) delete obj;
					obj=NULL;
					return;
				}
			}
		}
		else
		{
			for(i=dynamicObj.begin();i!=dynamicObj.end();i++)
			{
				o=*i;
				if(obj==o)
				{
					dynamicObj.remove(obj);
					if(obj!=NULL) delete obj;
					obj=NULL;
					return;
				}
			}
		}
	}
	if(topLeft!=NULL) topLeft->DeleteObj(obj,isStatic);
	if(topRight!=NULL) topRight->DeleteObj(obj,isStatic);
	if(bottomLeft!=NULL) bottomLeft->DeleteObj(obj,isStatic);
	if(bottomRight!=NULL) bottomRight->DeleteObj(obj,isStatic);
}