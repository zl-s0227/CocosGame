#pragma once
#include "2d/CCSprite.h"

class Actor
{
public:
	Actor();
	virtual ~Actor();

	void moveByDeltaPos(float deltaX, float deltaY);
	virtual void moveByDeltaPos(const cocos2d::Vec2& deltaPos) = 0;
	virtual void setAccelerate(bool accelerate) = 0;
	virtual cocos2d::Vec2 getLocation() = 0;
	virtual cocos2d::Size getSize() = 0;

};

class Control
{
public:
	Control();
	virtual ~Control();

	Actor* getActor() { return _actor; }
	virtual void setActor(Actor* actor) {_actor = actor; }
	virtual void onActorDestroy(cocos2d::EventCustom* event);

protected:
	Actor* _actor;
	cocos2d::EventListener* _eventListener;
};

class ControlUI : public cocos2d::Sprite, public Control
{
protected:
	ControlUI();

	bool init() override;
	virtual void onCapture(bool capture) {}
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

protected:
	bool _capture;
	cocos2d::Rect _boundingRect;
	cocos2d::Touch* _touch;
};

class ControlRocker : public ControlUI
{
public:
	static ControlRocker* create();

protected:
	bool init() override;
	void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event) override; 
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
	void onActorDestroy(cocos2d::EventCustom* event) override;

private:
	void reset();
private:
	cocos2d::Sprite* _background;
	cocos2d::Sprite* _ball;
};

class ControlAccelerator :public ControlUI
{
public:
	static ControlAccelerator* create();

protected:
	bool init() override;
	void onCapture(bool capture) override;
private:
	cocos2d::Sprite* _background;
};



class ControlAI :  public cocos2d::Node, public Control
{
public:
	static ControlAI* create();

	void setScene(cocos2d::Scene* scene) { _scene = scene; }
	cocos2d::Scene* getScene() { return _scene; }

	void queryRect(float = 0.0f);
	void setActor(Actor* actor) override;

protected:
	ControlAI();
	~ControlAI();
	bool queryRectCallBack(cocos2d::PhysicsWorld& world, cocos2d::PhysicsShape& shape, void* data);
	void onActorDestroy(cocos2d::EventCustom* event) override;
	

private:
	cocos2d::Scene* _scene;
};