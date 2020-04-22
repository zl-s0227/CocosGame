#pragma once
#include "2d/CCSpriteBatchNode.h"
#include "Games/GameDefs.h"
#include "Games/Control.h"
#include "2d/CCSprite.h"
#include "physics/CCPhysicsContact.h"
#include "SnackFoods.h"

class SnackSprite :public cocos2d::SpriteBatchNode, public Actor
{
public:
	static SnackSprite* create(cocos2d::Node* boundary, const std::string& fileImage);
	
	void setBoundary(cocos2d::Node* boundary) { _boundary = boundary; }
	bool isOutOfBoundary();

	

	/*override functions*/
	void update(float dt) override;
	void moveByDeltaPos(const cocos2d::Vec2& deltaPos) override;
	void setAccelerate(bool accelerate) override;
	cocos2d::Vec2 getLocation() override;
	cocos2d::Size getSize() override;

private:
	SnackSprite(cocos2d::Node* boundary);
	~SnackSprite();

	void born();
	void move();
	void eat(Food* food);
	void grow();
	void die();
	
	cocos2d::Sprite* createChild();
	void move(cocos2d::Node* curChild, cocos2d::Node* preChild);
	void addChildren();

	/*EventListeners*/
	void registerEventListeners();
	bool onContactBegin(cocos2d::PhysicsContact & contact);
	void beforeUpdate(cocos2d::EventCustom* event);

private:
	/*update*/
	float _actionDuration;
	float _updateDuration;

	/*move*/
	cocos2d::Vec2 _dstDirection;
	cocos2d::Vec2 _curDirection;
	cocos2d::Sprite* _head;

	/*flags*/
	bool _alive;
	bool _startMove;
	bool _accelerate;

	/*grow*/
	std::vector<cocos2d::Node*> _bodyNodes;
	std::list<cocos2d::Vec2> _trackPositions;
	int _score;
	unsigned int _growCount;
	cocos2d::Sprite* _tail;

	cocos2d::Node* _boundary;
	
};