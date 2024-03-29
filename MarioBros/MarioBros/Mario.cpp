#include "Mario.h"
#include "Object.h"
#include "Info.h"
#include "Animation.h"
#include "Sprite.h"
#include "Collision.h"
#include "Utilities.h"
#include "LifeFungus.h"
#include "Coin.h"
#include "PowerUp.h"
#include "CheckPoint.h"
#include "QBrick.h"

Mario::Mario():Object(){}
Mario::Mario(float _x, float _y,float width,float height,int _ID,Sprite* _sSmall,Sprite* _sBig,Sprite* _bullet1,Sprite* _bullet2,Game* _game):Object(_x,_y,width,height)
{
	game		=_game;
	sprite		= _sSmall;
	bullet1		= _bullet1;//k
	bullet2		= _bullet2;//k
	sBig		= _sBig;
	ID			=_ID;
	ani			= new Animation(8,4,0.1,sprite);
	aBig		= new Animation(8,4,0.1,sBig);
	checkPoint	= new CheckPoint(_x,_y,game->stage);
	collisionList	=new list<Object*>;
	xOld		=x;
	yOld		=y;
	camera		=game->camera;

	isFaceRight	=true;
	timeJumped	=0;
	
	vx=vy=ax=ay	=0;
	isJumming	=false;
	isBig		=false;
	isShotable	=false;
	isHasStar	=false;
	oldRECT		=rect;
	state		=MS_NORMAL;
	waitInFlag	=0;
	waitProtect	=10;
	waitProtectHidden	=0;
	isPlayingCoin	=false;
	waitInPipe	=0;
	waitbullet	=0;
	isProtected	=false;
	isProtectedHidden	=false;
	isShotting	=false;
	waitShotting	=true;
	vMax	=250;	//H
	vyOld		=vy;
	waitChangeDir	=0;
	isChangeDirectionL= isChangeDirectionR	=false;	
	
}
Mario::~Mario()
{
	if(collisionList) delete collisionList;
	collisionList=NULL;
	if(checkPoint) delete checkPoint;
	checkPoint=NULL;
	//delete game->a; 
}
void Mario::Update(list<Object*>* staObjs,list<Object*>* dynObjs,Input* input, float TPF)
{
	list<Object*>::iterator i;
	Object* obj;
	if(isPlayingCoin==true)
		waitsound+=TPF;
	switch(state)
	{
	case MS_NORMAL: case MS_SIT:
		game->a->PlaySound(game->sBackground);
		ProcessInput(input);
		Move(TPF,staObjs,dynObjs);
		isCanGoDown=false;
		
		if(game->coin>=100)
		{
			game->coin=0;
			game->numLife++;
			game->a->PlaySound(game->sLife);
		}
		if(isShotable==true)
		{
			waitbullet+=TPF;
		}
		//Xử lý thời gian, hết thời gian thì chết 
		if(game->timeGame==0)		
			ChangeState(MS_DIEING);

		//Xử lý khi phanh
		if(isChangeDirectionL==true)
		{
			waitChangeDir+=TPF;
			if(waitChangeDir>0.5)
			{
				waitChangeDir-=0.5;
				isChangeDirectionL=false;
			}
		}
		if(isChangeDirectionR==true)
		{
			waitChangeDir+=TPF;
			if(waitChangeDir>0.5)
			{
				waitChangeDir-=0.5;
				isChangeDirectionR=false;
			}
		}

		//Xét va chạm thông thường
		
		for(i=dynObjs->begin();i!=dynObjs->end();i++)
		{
			obj=*i;
			if(Collision::RectVsRect(rect,obj->rect)==true)
			{
				switch(obj->ID)
				{
				case 31:			//Nấm mạng
					if(obj->GetVar("state")==IS_MOVING)
					{
						obj->SetVar("state",IS_ATE);
						EatLife(obj->x,obj->y);
						game->a->PlaySound(game->sPowerUp); 
					}
					break;
				case 30:			//Nấm bự
					if(obj->GetVar("state")==IS_MOVING)		//Là nấm
					{
						obj->SetVar("state",IS_ATE);
						SetVar("isBig",true);
						game->AddScore(1000,obj->x,obj->y-40);
						game->a->PlaySound(game->sPowerUp); 
					}
					else if(obj->GetVar("state")==IS_READY)	//Là hoa
					{
						game->AddScore(1000,obj->x,obj->y-40);		
						obj->SetVar("state",IS_ATE);
						if(isBig==true)
							SetVar("isShotable",true);
						else SetVar("isBig",true);
						ChangeState(MS_EAT_FLOWER);
						game->a->PlaySound(game->sPowerUp); 
					}
					break;
				case 32:			//Sao
					if(obj->GetVar("state")==IS_ACTIVING)
					{
						game->AddScore(1000,obj->x,obj->y-40);
						obj->SetVar("state",IS_ATE);
						SetVar("isHasStar",true);
						game->a->PlaySound(game->sPowerUp); 
					}
					break;
				case 33:				//Cây ăn thịt
					if(Collision::RectVsRect(rect,obj->rect)&&obj->GetVar("state")==IS_ACTIVING)
						CollisionEnemy();
					break;
				case 41:			//Goomba
					if(isHasStar==false)							//Nếu không có sao
					{
						if(obj->GetVar("state")==ES_ACTIVING)
						{
							if(vy>0)			//Nếu mà dậm từ trên xuống
							{
								game->AddScore(100,obj->x,obj->y-40);
								obj->SetVar("state",ES_IDLE);
								vy=-600;
								isProtectedHidden=true;
								game->a->PlaySound(game->sMip); 
							}
							else CollisionEnemy();
						}
					}
					else if(obj->GetVar("state")!=ES_FALL) obj->SetVar("state",ES_FALL);
					break;
				case 42:					//Koopa
					RECT rLeft;
					if(isHasStar==false)
					{
						switch(obj->GetVar("state"))
						{
						case ES_ACTIVING:
							if(vy>0)			//Nếu mà dậm từ trên xuống
							{
								obj->SetVar("state",ES_IDLE);
								y=obj->y-height-1;
								vy=-600;
								isProtectedHidden=true;
								game->a->PlaySound(game->sMip); 
							}
							else 
							{
								CollisionEnemy();
							}
							break;
						case ES_IDLE:					
							rLeft.left=obj->x; rLeft.right=obj->x+1; rLeft.top=y; rLeft.bottom=y+TILE_SIZE;
							if(Collision::RectVsRect(rect,rLeft)==true)				//Bị đạp bên trái...
							{
								obj->SetVar("state",ES_MOVE_SHELL_RIGHT);		//... thì chạy sang bên phải
								game->a->PlaySound(game->sKick); 
							}
							else 
							{	
								obj->SetVar("state",ES_MOVE_SHELL_LEFT);
								game->a->PlaySound(game->sKick); 
							}
							if(vy>0)			//Nếu mà dậm từ trên xuống
							{
								y=obj->y-height-1;
								vy=-800;
							}
							isProtectedHidden=true;		//H
							break;
						case ES_MOVE_SHELL_LEFT: case ES_MOVE_SHELL_RIGHT:
							if(vy>0)	//Dậm từ trên xuống
							{
								obj->SetVar("state",ES_IDLE);
								vy=-800;
							}	
							else					//Va chạm bình thường.
							{
								//CollisionEnemy(MS_DIEING);
								CollisionEnemy();
							}
							break;
						}
					}
					else if(obj->GetVar("state")!=ES_FALL)				//Có sao
						obj->SetVar("state",ES_FALL);
				}
			}
			if(obj->ID==33)////////k
			{
				if(x>=obj->x-80&&x<=obj->x+90)
				{
					if(obj->GetVar("state")!=IS_ACTIVING&&obj->GetVar("vy")>0)
						obj->SetVar("state",IS_IDLE);
				}
			}
		}

		for(i=staObjs->begin();i!=staObjs->end();i++)
		{
			Object *obj=*i;
			RECT rFlagColumn;
			switch(obj->ID)
			{
			case 23:				//Ống ngang.
				if(Collision::RectVsRect(rect,obj->rect))
				{
					ChangeState(MS_AUTO_RIGHT_PIPE);
					game->a->PlaySound(game->sPipeWarp);
				}
				break;
			case 29:				//Cờ
				rFlagColumn.left=obj->x+58; rFlagColumn.right=obj->x+62;
				rFlagColumn.top=y; rFlagColumn.bottom=y+10*TILE_SIZE;
				if(Collision::RectVsRect(rect,rFlagColumn)==true&&state==MS_NORMAL)
				{
					ChangeState(MS_PULL_FLAG);
					obj->SetVar("state",TS_ACTIVING);
					yTemp=obj->y;
					x=obj->x+62-width;
				}
				break;			
			case 34:				//Tiền
				if(Collision::RectVsRect(rect,obj->rect))
				{	
					if(obj->GetVar("state")==IS_FREE)
					{
						EatCoin(obj->x,obj->y);				
						obj->SetVar("state",IS_ATE);
					}					
					switch(game->coin%4)
					{
					case 0: game->a->PlaySound(game->sCoin1); break;
					case 1: game->a->PlaySound(game->sCoin2); break;
					case 2: game->a->PlaySound(game->sCoin3); break;
					case 3: game->a->PlaySound(game->sCoin4); break;
					}
					/*isPlayingCoin=true;
					if(waitsound>0.5)
					{
						game->a->StopSound(game->sCoin);
						isPlayingCoin=false;
						waitsound=0;
					}*/										
				}
				break;
			
			case 52:				//Dấu đi xuống.
				if(Collision::RectVsRect(rect,obj->rect)==true)
				{
					isCanGoDown=true;
				}
			}
			if(obj->ID==50)						//CheckPoint
			{
				if(((obj->x>game->checkPoint->x&&game->checkPoint->stage!=game->stage)
					||game->checkPoint->stage==game->stage)&&obj->x<x)
				{
					game->checkPoint->x=obj->x;
					game->checkPoint->y=obj->y;
					game->checkPoint->stage=game->stage;
				}
			}
		}
		
		//Khi có sao
		if(isHasStar)
		{
			waitLostStar+=TPF;
			if(waitLostStar>=10)
			{
				waitLostStar=0;
				isHasStar=false;
			}
		}

		if(y>HEIGHT+10)						//Kiểm tra có rơi xuống hố không
			ChangeState(MS_DIEING);
		//Tạo quán tính
		if(vx>0)
			ax=-G/4;
		else if(vx<0)
			ax=G/4;

		//Thời gian bảo vệ khi bị thu nhỏ.
		if(isProtected==true)
		{
			waitProtect+=TPF;
			if(waitProtect>2)
			{
				isProtected=false;
				isRender=true;
			}
		}
		if(isProtectedHidden==true)
		{
			waitProtectHidden+=TPF;
			if(waitProtectHidden>0.5)
			{
				isProtectedHidden=false;
				waitProtectHidden-=0.5;
			}
		}

		//Xét Quay mặt hướng nào
		if(vx>0)							
			isFaceRight=true;
		else if(vx<0) isFaceRight=false;

		//Tính thời gian nhảy
		if(isJumming==true)	
		{
			timeJumped+=TPF;
		}
		
		break;
	case MS_PULL_FLAG:
		for(i=staObjs->begin();i!=staObjs->end();i++)
		{
			obj=*i;
			char flagState;
			switch(obj->ID)
			{
			case 29:				//Cờ
				flagState=obj->GetVar("state");
				if(flagState==TS_ACTIVING)						//Khi cờ đang xuống...
				{
					vx=0;
					if(y>yTemp+TILE_SIZE*10-height-10)			//... nếu Mario đã xuống tới nơi
						vy=0;						
				}
				else if(flagState==TS_IDLE_2)					//Nếu cờ được kéo xuống hoàn toàn.
				{
					isFaceRight=false;
					x=obj->x+58;
					waitInFlag+=TPF;							//Chờ để di chuyển tiếp
					if(waitInFlag>0.5)
					{
						waitInFlag=0;
						ChangeState(MS_AUTO_TO_CASTLE);
					}
				}
				break;
			}
		}
		y+=vy*TPF+0.5*ay*TPF*TPF;
		break;
	case MS_AUTO_TO_CASTLE:
		Move(TPF,staObjs,dynObjs);
		for(i=staObjs->begin();i!=staObjs->end();i++)
		{
			Object *obj=*i;
			switch(obj->ID)
			{
			case 18:				//Lâu đài
				if(Collision::RectVsRect(rect,obj->rect))
				{
					game->ChangeState(GS_NEXT_STAGE);
					memcpy(game->cloneMario,this,sizeof(Mario));
				}
				break;
			}
		}
		break;
	case MS_DIEING:
		vy+=ay*TPF;
		y+=vy*TPF+0.5*ay*TPF*TPF;
		waitDieing+=TPF;
		game->a->PlaySound(game->sDeath); 
		if(waitDieing>2)					//Chờ chết :D
			ChangeState(MS_DIED);
		break;
	case MS_AUTO_RIGHT_PIPE:
		x+=vx*TPF;
		waitInPipe+=TPF;
		if(waitInPipe>1)
		{
			waitInPipe-=1;
			game->ChangeState(GS_RETURN_STAGE);			//Trở lại màn chính.		
			memcpy(game->cloneMario,this,sizeof(Mario));
		}
		else if(waitInPipe>0.5)
			vx=0;
		break;
	case MS_EAT_FLOWER:
		waitEatFlower+=TPF;
		if(waitEatFlower>1)	//H
		{
			ChangeState(MS_NORMAL);
			isHasStar=false;
		}
		break;
	case MS_ZOOM_IN:
		waitFrmZoom+=TPF;
		if(waitFrmZoom>0.1)
		{
			waitFrmZoom-=0.1;
			frameZoom++;
		}
		if(frameZoom==6)
			ChangeState(MS_NORMAL);
		break;
	case MS_ZOOM_OUT:
		waitFrmZoom+=TPF;
		if(waitFrmZoom>0.1)
		{
			waitFrmZoom-=0.1;
			frameZoom--;
		}
		if(frameZoom==0)
			ChangeState(MS_NORMAL);
		break;
	}
	vyOld=vy;
	UpdateAnimation(TPF);
}
void Mario::Render(Camera* camera)
{
	if(isRender==false) return;
	switch(state)				//H
	{
	case MS_ZOOM_IN:
		switch(frameZoom)
		{
		case 0: case 2:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sprite->Draw(x-camera->x,y+TILE_SIZE,ZOOM,ZOOM,ani->rect,0.4);
			else sprite->Draw(x+TILE_SIZE-camera->x,y+TILE_SIZE,-ZOOM,ZOOM,ani->rect,0.4);
			break;
		case 1: case 3: case 5:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sBig->Draw(x-camera->x+10,y+20,ZOOM*0.75,ZOOM*0.75,aBig->rect,0.4);
			else sBig->Draw(x+TILE_SIZE-camera->x+10,y+20,-ZOOM*0.75,ZOOM*0.75,aBig->rect,0.4);
			break;
		case 4: case 6:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sBig->Draw(x-camera->x,y,ZOOM,ZOOM,aBig->rect,0.4);
			else sBig->Draw(x+TILE_SIZE-camera->x,y,-ZOOM,ZOOM,aBig->rect,0.4);
			break;
		}
		break;
	case MS_ZOOM_OUT:
		switch(frameZoom)
		{
		case 0: case 2:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sprite->Draw(x-camera->x,y,ZOOM,ZOOM,ani->rect,0.4);
			else sprite->Draw(x+TILE_SIZE-camera->x,y,-ZOOM,ZOOM,ani->rect,0.4);
			break;
		case 1: case 3: case 5:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sBig->Draw(x-camera->x+10,y-20,ZOOM*0.75,ZOOM*0.75,aBig->rect,0.4);
			else sBig->Draw(x+TILE_SIZE-camera->x+10,y-20,-ZOOM*0.75,ZOOM*0.75,aBig->rect,0.4);
			break;
		case 4: case 6:
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sBig->Draw(x-camera->x,y-TILE_SIZE,ZOOM,ZOOM,aBig->rect,0.4);
			else sBig->Draw(x+TILE_SIZE-camera->x,y-TILE_SIZE,-ZOOM,ZOOM,aBig->rect,0.4);
			break;
		}
		break;
	default:
		if(isBig)
		{
			//Nếu quay mặt sang phải và không phanh trái hoặc đang phanh phải, thì vẽ về bên phải.
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sBig->Draw(x-camera->x,y,ZOOM,ZOOM,aBig->rect,0.4);
			else sBig->Draw(x+TILE_SIZE-camera->x,y,-ZOOM,ZOOM,aBig->rect,0.4);
		}
		else
		{
			if((isFaceRight&&isChangeDirectionL==false)||isChangeDirectionR==true)	
				sprite->Draw(x-camera->x,y,ZOOM,ZOOM,ani->rect,0.4);
			else sprite->Draw(x+TILE_SIZE-camera->x,y,-ZOOM,ZOOM,ani->rect,0.4);
		}
	}
}

void Mario::Move(float TPF,list<Object*>* staObjs,list<Object*>* dynObjs)
{
	Object* obj;
	list<Object*>::iterator i;
	list<Object*> lItems;
	GetListItems(&lItems,staObjs,dynObjs);

	//Theo phương x
	vx+=ax*TPF;
	if(vx>vMax) vx=vMax;	//H
	else if(vx<-vMax) vx=-vMax;
	x+=vx*TPF+1/2*ax*TPF*TPF;
	if(x<camera->x)									//Không cho đi quá Camera
		x=camera->x;
	if(x>camera->x+WIDTH-TILE_SIZE)
		x=camera->x+WIDTH-TILE_SIZE;
	ResetRect();

	for(i=staObjs->begin();i!=staObjs->end();i++)
	{
		obj=*i;
		if(obj->ID>=20&&obj->ID<=29&&obj->ID!=23)		//Nếu là thùng, đá , gạch...
		{
			if(Collision::RectVsRect(rect,obj->rect)==true
				&&obj->GetVar("state")!=TS_BREAKING&&obj->GetVar("state")!=TS_BREAKED)
			{
				vx=ax=0;
				x=xOld;
			}
		}
	}
	//Theo phương y
	ay=G;
	vy+=ay*TPF;
	y+=vy*TPF+1.0/2*ay*TPF*TPF;
	oldRECT=rect;
	ResetRect();

	collisionList->clear();
	for(i=staObjs->begin();i!=staObjs->end();i++)
	{
		obj=*i;
		if((obj->ID>=20&&obj->ID<=29)||obj->ID==51)		//Nếu là thùng, đá , gạch...
		{
			if(Collision::RectVsRect(rect,obj->rect)==true
				&&obj->GetVar("state")!=TS_BREAKING&&obj->GetVar("state")!=TS_BREAKED)
			{	
				if(!(obj->ID==51&&obj->GetVar("isChanged")==false)) //Loại bỏ gạch ẩn.
					collisionList->push_back(obj);						//Xét va chạm để không bị rơi

				if(Collision::RectVsRect(rHead,obj->rect)==true&&vyOld<0)	//Va chạm của rHead
				{
					//Xét va chạm với item
					list<Object*>::iterator j;
					Object* item;
					for(j=lItems.begin();j!=lItems.end();j++)	
					{
						item=*j;
						if(Collision::RectVsRect(rHead,item->rect))
						{
							switch(item->ID)
							{
							case 34:					//Tiền
								int numItemCoin;
								switch(game->coin%4)
								{
								case 0: game->a->PlaySound(game->sCoin1); break;
								case 1: game->a->PlaySound(game->sCoin2); break;
								case 2: game->a->PlaySound(game->sCoin3); break;
								case 3: game->a->PlaySound(game->sCoin4); break;
								}
								numItemCoin=item->GetVar("numCoin");
								if(numItemCoin>0&&item->GetVar("state")==IS_IDLE)
								{
									item->SetVar("numCoin",numItemCoin-1);
									EatCoin(item->x,item->y);
									item->SetVar("state",IS_ACTIVING);
								}
								break;
							case 30: case 32: case 31:				//Nấm bự, sao, nấm mạng
								if(item->GetVar("state")==IS_IDLE)
								{
									item->SetVar("state",IS_ACTIVING);
									game->a->PlaySound(game->sBonusRise); 
								}
							}
						}
					}

					//Xét với các Terrain
					switch(obj->ID)
					{
					case 24:							//Gạch
						if(obj->GetVar("isChanged")==0)
						{
							if((isBig==false||(isBig==true&&obj->GetVar("isContainCoin")==true))
								&&obj->GetVar("state")!=TS_MOVEUP)
							{
								obj->SetVar("state",TS_MOVEUP);
								if(obj->GetVar("isContainCoin")==false)
									game->a->PlaySound(game->sBump);
							}
							else if(obj->GetVar("state")==TS_IDLE)
							{
								obj->SetVar("state",TS_BREAKING);
								game->a->PlaySound(game->sBrickBreaked); 
							}
						}
						break;
					case 25:
  						if(obj->GetVar("state")==TS_IDLE)
						{
							obj->SetVar("state",TS_MOVEUP);
						}
					}
					CollisionOnTop(obj,staObjs,dynObjs);		//Xét va chạm, đụng gạch thì ăn tiền, chết quái bên trên
				}

				//Nếu húc từ dưới lên
				if(isJumming==true&&vy<0)	//Nếu đụng trần				
				{
					ay=G;
					vy=200;
					y=obj->y+TILE_SIZE+1;
				}
				
			}
			//---- Hết va chạm với rect
		}
		//----- Hết if(Collision::RectVsRect(rect,obj->rect)==true)
	}

	//Duyệt danh sách va chạm theo chiều dọc, để không bị rơi
	if(collisionList->size()>0)
	{
		Object* obj;
		list<Object*>::iterator i;
		for(i=collisionList->begin();i!=collisionList->end();i++)
		{
			obj=*i;
			if(oldRECT.bottom<obj->rect.bottom)
				if(obj->y-height<y)
				{
					if(obj->ID==29)				//Nếu là cờ
						y=obj->y+TILE_SIZE*10-height;
					else
						y=obj->y-height;
					isJumming=false;
					timeJumped=0;
					vy=0;
				}
		}
		ResetRect();
	}

	xOld=x;
	yOld=y;

	if(vy>0)
	{
		isJumming=true;
		timeJumped=10;
	}

	if(vx*ax<0)							//Ngăn gia tốc làm Mario quay lại
	{
		if(vx>-50&&vx<50)
		{
			vx=0;
			ax=0;
		}
	}
}
void Mario::CollisionOnTop(Object* obj,list<Object*>* staObjs,list<Object*>* dynObjs)
{
	if((obj->ID==24||obj->ID==25)&&obj->GetVar("isChanged")==false)		
	{
		RECT rTop;					//Rect ngay trên viên gạch
		list<Object*>::iterator k;
		Object* objTop;
		rTop.left=obj->rect.left;			rTop.right=obj->rect.right;
		rTop.top=obj->rect.top-TILE_SIZE;	rTop.bottom=obj->rect.top;
		for (k=staObjs->begin();k!=staObjs->end();k++)
		{
			objTop=*k;
			if(objTop->ID==34)			//Tiền
				if(Collision::RectVsRect(rTop,objTop->rect))
				{
					if(objTop->GetVar("state")==IS_FREE)
					{
						EatCoin(objTop->x,objTop->y);
						objTop->SetVar("state",IS_ACTIVING);
						switch(game->coin%4)
						{
						case 0: game->a->PlaySound(game->sCoin1); break;
						case 1: game->a->PlaySound(game->sCoin2); break;
						case 2: game->a->PlaySound(game->sCoin3); break;
						case 3: game->a->PlaySound(game->sCoin4); break;
						}
					}
				}
		}
		for(k=dynObjs->begin();k!=dynObjs->end();k++)
		{
			objTop=*k;
			switch(objTop->ID)
			{
			case 41:case 42:
				if(Collision::RectVsRect(rTop,objTop->rect))
					objTop->SetVar("state",ES_FALL);
				break;
			case 30: case 31:
				if(Collision::RectVsRect(rTop,objTop->rect)&&objTop->GetVar("state")!=IS_IDLE)
					objTop->SetVar("state",IS_ACTIVING);
				break;
			}
		}
	}
}
void Mario::UpdateAnimation(float TPF)
{
	switch(state)
	{
	case MS_NORMAL: case MS_AUTO_TO_CASTLE: case MS_SIT: case MS_AUTO_RIGHT_PIPE: case MS_EAT_FLOWER:	//H
		if(isBig==false)					//Nhỏ
		{
			if(isJumming==false)
				if(isChangeDirectionL==true||isChangeDirectionR==true) 
					ani->SetIndex(2);
				else if(vx!=0)					//Di chuyển
					ani->Update(3,5,TPF);
				else ani->SetIndex(0);			//Đứng im
			else if(vy<0)
			{	
				ani->SetIndex(6);				//Nhảy
			}
		}
		else								//Lớn
		{
			if(isShotable==false)
			{
				
				if(state==MS_SIT)	
					aBig->SetIndex(0);				//Ngồi
				else if(isJumming==false)
					if(isChangeDirectionL==true||isChangeDirectionR==true) //H
						aBig->SetIndex(2);
					else if(vx!=0)					//Di chuyển
						aBig->Update(3,5,TPF);
					else aBig->SetIndex(1);			//Đứng im
				else if(vy<0)
					aBig->SetIndex(6);				//Nhảy
				
			}
			else
			{
				if(isShotting==true)
				{
					waitShotting+=TPF;
					if(waitShotting>0.2)
					{
						waitShotting-=0.2;
						isShotting=false;
					}
					aBig->SetIndex(12);
				}
				else if(state==MS_SIT)
					aBig->SetIndex(8);				//Ngồi
				else if(isJumming==false)
					if(isChangeDirectionL==true||isChangeDirectionR==true) 
						aBig->SetIndex(10);
					else if(vx!=0)						//Di chuyển
						aBig->Update(11,13,TPF);
					else aBig->SetIndex(9);			//Đứng im
				else if(vy<0)
					aBig->SetIndex(14);				//Nhảy
			}
		}
		if(isHasStar==true)						//Tạo hiệu ứng nhấp nháy
		{
			waitNextRow+=TPF;
			if(waitNextRow>0.1)
			{
				waitNextRow=0;
				if(renderRow<3)
					renderRow++;
				else renderRow=0;
			}
			if(isBig==true)
			{
				aBig->rect.top=renderRow*aBig->heightOfFrame;
				aBig->rect.bottom=aBig->rect.top+aBig->heightOfFrame;
			}
			else
			{
				ani->rect.top=renderRow*ani->heightOfFrame;
				ani->rect.bottom=ani->rect.top+ani->heightOfFrame;
			}
		}
		//Tạo sự nhấp nháy khi được bảo vệ.
		if(isProtected==true)
		{
			waitRender+=TPF;
			if(waitRender>0.05)
			{
				waitRender-=0.05;
				isRender=!isRender;
			}
		}
		break;
	case MS_PULL_FLAG:
		if(isBig==true)
		{
			if(isShotable==true)
				aBig->SetIndex(15);
			else aBig->SetIndex(7);
		}
		else ani->SetIndex(7);
		break;
	case MS_DIEING:
		ani->SetIndex(1);
		break;
	case MS_ZOOM_IN: case MS_ZOOM_OUT:	//H
		if(frameZoom==0||frameZoom==2)
			ani->SetIndex(0);
		else aBig->SetIndex(1);
	}
}
void Mario::ProcessInput(Input *input)
{
	if(state==MS_EAT_FLOWER) return;
	
	if(input->KeyDown(DIK_D))		//Nhảy
	{
		if(timeJumped<0.2)
		{
			vy=-700;
			isJumming=true;
			game->a->PlaySound(game->sJump);
		}
	}
	if(input->KeyDown(DIK_DOWN))		//Ngồi
	{
		if(isJumming==false)
		{
			ChangeState(MS_SIT);
			if(isCanGoDown==true)
			{
				game->ChangeState(GS_SUB_STAGE);
				memcpy(game->cloneMario,this,sizeof(Mario));
			}
		}
	}
	else
	{
		if(input->KeyDown(DIK_RIGHT))
		{
			ax=G/3;
			if(vx<0&&isChangeDirectionR==false)			//Phanh
			{
				isChangeDirectionR=true;
				waitChangeDir=0;
			}
			if(input->KeyDown(DIK_F))
			{
				vMax=400;
				ani->timePerImage=aBig->timePerImage=0.05;
			}
			else
			{
				vMax=250;
				ani->timePerImage=aBig->timePerImage=0.1;
			}
		}
		if(input->KeyDown(DIK_LEFT))
		{
			ax=-G/3;
			if(vx>0&&isChangeDirectionL==false)	
			{
				isChangeDirectionL=true;
				waitChangeDir=0;
			}
			if(input->KeyDown(DIK_F))
			{
				vMax=400;
				ani->timePerImage=aBig->timePerImage=0.05;
			}
			else
			{
				vMax=250;
				ani->timePerImage=aBig->timePerImage=0.1;
			}
		}
		ChangeState(MS_NORMAL);
	}

	if(input->KeyDown(DIK_F))		//Bắn
		if(isShotable==true)
		{						
			if(waitbullet>0.35)
			{
				isShotting=true;
				Object* obj = new Bullet(isFaceRight?x+TILE_SIZE:x,y+20,8,8,isFaceRight?500:-500, bullet1,bullet2);
				obj->SetVar("state",BS_ACTIVING);
				game->quadTree->Add(obj,false);
				game->a->PlaySound(game->sFireBall); 
				waitbullet=0;
			}
		}
}
int Mario::GetVar(char* varName)
{
	if(strcmp(varName,"isBig")==0)
		return isBig;
	return -1;
}
void Mario::SetVar(char* varName,int val)
{
	if(strcmp(varName,"isBig")==0)
	{
		if(isBig==true&&val==0)
		{
			isBig=false;
			isShotable=false;
			y+=TILE_SIZE;
			height/=2;
			yOld=y;
			ChangeState(MS_ZOOM_OUT);
		}
		else if(isBig==false&&val==1)
		{
			isBig	=true;
			y-=TILE_SIZE;
			height*=2;
			yOld=y;
			ChangeState(MS_ZOOM_IN);
		}
	}
	if(strcmp(varName,"isHasStar")==0)
	{
		isHasStar=val;
		waitNextRow=0;
		waitLostStar=0;
		renderRow=0;
	}
	if(strcmp(varName,"isShotable")==0)
	{
		isShotable=val;
	}
}
void Mario::ResetRect()
{
	rect.left=x+2;
	rect.top=y;
	rect.right=rect.left+width-2;
	rect.bottom=rect.top+height;
	
	rHead.left=x+15;
	rHead.right=rHead.left+10;
	rHead.top=y;
	rHead.bottom=y+height/4;

	rAboveHead.left=rHead.left;
	rAboveHead.right=rHead.right;
	rAboveHead.top=rHead.top-TILE_SIZE;
	rAboveHead.bottom=rHead.bottom-TILE_SIZE;
}
void Mario::ResetWhenReload(Sprite* _sprite)
{
	sprite=_sprite;
	collisionList	=new list<Object*>;
	ani			= new Animation(8,4,0.1,sprite);
	aBig		= new Animation(8,4,0.1,sBig);
}
void Mario::Save(fstream *fs)
{
	fs->write(reinterpret_cast<char*>(&ID),sizeof(ID));	//int ID, để khi load biết được đang load Obj nào
	int sizeObj=sizeof(*this);
	fs->write(reinterpret_cast<char*>(&sizeObj),sizeof(int));			//int sizeof(obj)
	fs->write(reinterpret_cast<char*>(this),sizeObj);
}
void Mario::Load(fstream *fs)
{
	int sizeObj;
	fs->read(reinterpret_cast<char*>(&sizeObj),sizeof(int));			//int sizeof(obj)
	fs->read(reinterpret_cast<char*>(this),sizeObj);
}
void Mario::ChangeState(char _state)
{
	//Nếu còn được bảo vệ thì không cho chết.
	if(state==MS_NORMAL&&(isProtectedHidden==true||isProtected==true)&&_state==MS_DIEING) return;
	state=_state;
	switch(state)
	{
	case MS_PULL_FLAG:
		vy=200;
		ay=0;
		break;
	case MS_AUTO_TO_CASTLE: case MS_AUTO_RIGHT_PIPE:
		vx=200;
		vy=0;
		ay=0;
		ax=0;
		isFaceRight=true;
		break;
	case MS_DIEING:
		game->a->StopSound(game->sBackground);
   		waitDieing=0;
		vx=ax=0;
		vy=-1000;
		ay=G;
		isBig=false;
		game->numLife--;
		break;
	case MS_DIED:
		yTemp=y;
		game->ChangeState(GS_REPLAY);
		break;
	case MS_EAT_FLOWER:
		SetVar("isHasStar",true);
		waitEatFlower=0;
		break;
	case MS_ZOOM_IN:
		frameZoom=0;
		waitFrmZoom=0;
		break;
	case MS_ZOOM_OUT:
		frameZoom=6;
		waitFrmZoom=0;
		break;
	}
}
void Mario::CollisionEnemy()
{
	if(isBig==true)	//Lớn sẽ bị thu nhỏ
	{
		SetVar("isBig",0);
		isProtected=true;
		waitProtect=0;
		waitRender=0;
	}
	else ChangeState(MS_DIEING);
}
void Mario::EatCoin(float _x,float _y)			
{
	game->coin++;
	game->AddScore(200,_x,_y-40);
}
void Mario::EatLife(float _x,float _y)
{
	game->numLife++;
	game->a->PlaySound(game->sLife);
	game->AddScore(1000,_x,_y-40);
}