#include "SnackSprite.h"
#include "2d/CCSprite.h"
#include "2d/CCActionInterval.h"
#include "2d/CCActionEase.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "Games/GameDefs.h"
#include "Games/SnacksGame.h"
#include "../GameDefs.h"

USING_NS_CC;

SnackSprite* SnackSprite::create(Node* boundary,const std::string& fileImage)
{
	SnackSprite* ret = new(std::nothrow) SnackSprite(boundary);

	if (ret && ret->initWithFile(fileImage))
	{
		ret->autorelease();
		ret->born();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

SnackSprite::SnackSprite(Node* boundary) :
	_actionDuration(SNACK_ACTION_DURATION),
	_updateDuration(_actionDuration),
	_head(nullptr),
	_tail(nullptr),
	_boundary(boundary)
{
	registerEventListeners();
}
SnackSprite::~SnackSprite()
{
	log("%s", __FUNCTION__);
}

void SnackSprite::moveByDeltaPos(const Vec2& deltaPos)
{
	if (deltaPos == Vec2::ZERO)
		return;

	_dstDirection.x = deltaPos.x / deltaPos.length();
	_dstDirection.y = deltaPos.y / deltaPos.length();

}

void SnackSprite::setAccelerate(bool accelerate)
{
	_accelerate = accelerate;
}

cocos2d::Vec2 SnackSprite::getLocation()
{
	return _position + _head->getPosition() * _scaleX;
}

cocos2d::Size SnackSprite::getSize()
{
	if (_head != nullptr)
		return _head->getContentSize() * _head->getScale();
	else
		return Size::ZERO;
}

void SnackSprite::beforeUpdate(cocos2d::EventCustom* event)
{
	if (_startMove && _tail != nullptr)
	{
		_trackPositions.push_back(_tail->getPosition());

		if (_trackPositions.size() > SNACK_TRACK_LENGTH)
		{
			_trackPositions.pop_front();
		}
	}
}

void SnackSprite::update(float dt)
{
	if (isOutOfBoundary())
	{
		die();
		return;
	}

	if (_startMove)
	{
		_startMove = false;
		_updateDuration = 0;
	}
	else
	{
		_updateDuration += dt;
	}
	// move finish
	if (_updateDuration >= _actionDuration)
	{
		addChildren();
		move();
	}
}

void SnackSprite::addChildren()
{
	int max = _growCount > _trackPositions.size() ? _trackPositions.size(): _growCount;

	if (max == 0)
		return;

	for (int i = 0; i < max; i++)
	{
		auto child = createChild();
		auto pos = _trackPositions.back();
		child->setPosition(pos);
		addChild(child);
		_trackPositions.pop_back();
		_growCount--;
	}
}


void SnackSprite::born()
{
	scheduleUpdate();

	_tag = OBJECT_SNACK;
	_dstDirection = _curDirection = Vec2(1.0f, 0.0f);
	_growCount = 0;
	_score = 0;
	_alive = true;
	_startMove = false;
	_accelerate = false;

	_head = createChild();
	_head->setColor(Color3B::RED);
	addChild(_head);

	auto origin = _head->getPosition();
	auto off = _head->getContentSize().width;
	for (int i = 1; i < SNACK_DEFAULT_LENGTH; i++)
	{
		auto child = createChild();
		child->setPosition(origin.x - i * off, origin.y);
		addChild(child);
	}
}

void SnackSprite::registerEventListeners()
{
	auto listenerContact = EventListenerPhysicsContact::create();
	listenerContact->onContactBegin = CC_CALLBACK_1(SnackSprite::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerContact, this);

	auto listenerBeforeUpdate = EventListenerCustom::create(Director::EVENT_BEFORE_UPDATE, CC_CALLBACK_1(SnackSprite::beforeUpdate, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerBeforeUpdate, this);
}

bool SnackSprite::onContactBegin(PhysicsContact & contact)
{
	auto nodeA = contact.getShapeA()->getBody()->getOwner();
	auto nodeB = contact.getShapeB()->getBody()->getOwner();
	
	bool isChildA = _children.contains(nodeA);
	bool isChildB = _children.contains(nodeB);
	
	Node* contactNode = nullptr;
	Node* childNode = nullptr;
	if (isChildA && !isChildB)
	{
		contactNode = nodeB;
		childNode = nodeA;
	}
	else if (!isChildA && isChildB)
	{
		contactNode = nodeA;
		childNode = nodeB;
	}
	
	if (contactNode != nullptr)
	{
		switch (contactNode->getTag())
		{
		case OBJECT_FOOD:
			eat(dynamic_cast<Food*>(contactNode));
			break;
		default:
			if (childNode == _head)
				die();
			break;
		}
	}
	return true;
}

void SnackSprite::grow()
{
	_growCount++;
}


bool SnackSprite::isOutOfBoundary()
{
	CC_ASSERT(_boundary != nullptr);

	auto visibleSize = _director->getVisibleSize();
	auto headPos = getLocation();
	auto headSize = getSize();
	auto off = headPos - _boundary->getPosition() + _boundary->getAnchorPointInPoints();
	auto backgroundSize = _boundary->getContentSize();
	Rect rect(headSize / 2, backgroundSize - headSize);

	return !rect.containsPoint(off);
}

void SnackSprite::die()
{
	if (_alive)
	{
		_alive = false;
		unscheduleUpdate();
		dynamic_cast<SnacksGame*>(_parent)->collect(this);
	}
}

Sprite* SnackSprite::createChild()
{
	Sprite* sprite = Sprite::createWithTexture(_textureAtlas->getTexture());
	

	if (sprite != nullptr)
	{
		_tail = sprite;

		sprite->setScale(1);

		auto physicsBody = PhysicsBody::createCircle(sprite->getContentSize().width / 2);
		physicsBody->setCollisionBitmask(0x00);
		physicsBody->setContactTestBitmask(0x01);
		sprite->addComponent(physicsBody);
		sprite->setLocalZOrder(_bodyNodes.size() * -1);
		_bodyNodes.push_back(sprite);
	}
	
	return sprite;
}

void SnackSprite::move()
{
	if (_accelerate)
	{
		_actionDuration = SNACK_ACTION_DURATION / 2;
	}
	else
	{
		_actionDuration = SNACK_ACTION_DURATION;
	}

	Node* preChild = nullptr;
	for (Node* child : _bodyNodes)
	{
		move(child, preChild);
		preChild = child;
	}

	_startMove = true;
}

void SnackSprite::move(Node* curChild, Node* preChild)
{
	ActionInterval* action = nullptr;
	if (preChild == nullptr)
	{
		bool turnLeft = true;
		auto deltaAngle = _dstDirection.getAngle() - _curDirection.getAngle();

		if (abs(deltaAngle) > PI)
		{
			deltaAngle -= abs(deltaAngle) / deltaAngle * 2 * PI;
		}

		if (deltaAngle < 0)
		{
			turnLeft = false;
		}

		if (abs(deltaAngle) > SNACK_MAX_ANGLE_OF_TURN)
		{
			auto curAngle = _curDirection.getAngle();
			curAngle = curAngle + abs(deltaAngle) / deltaAngle * SNACK_MAX_ANGLE_OF_TURN;
			_curDirection = Vec2(cos(curAngle), sin(curAngle));
		}
		else
		{
			_curDirection = _dstDirection;
		}

		action = MoveBy::create(_actionDuration, _curDirection * _head->getContentSize().width);
	}
	else
	{
		action = MoveTo::create(_actionDuration, preChild->getPosition());
	}

	curChild->runAction(action);
}

void SnackSprite::eat(Food* food)
{
	_score += food->getScore();
	if (_score % 20 == 0)
		grow();

	food->removeFromParent();
}
