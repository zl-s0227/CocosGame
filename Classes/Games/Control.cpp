#include "Control.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventCustom.h"
#include "2d/CCScene.h"
#include "physics/CCPhysicsWorld.h"
#include "2d/CCCamera.h"
#include "Games/GameDefs.h"
#include "base/CCScheduler.h"

USING_NS_CC;

ControlRocker* ControlRocker::create()
{
	auto rocker = new(std::nothrow)ControlRocker;

	if (rocker != nullptr && rocker->init())
	{
		rocker->autorelease();
		return rocker;
	}

	CC_SAFE_DELETE(rocker);
	return nullptr;
}

ControlUI::ControlUI():
	_capture(false),
	_touch(nullptr)
{

}

bool ControlUI::init()
{
	bool ret = Sprite::init();

	if (ret)
	{
		auto eventListenerTouch = EventListenerTouchOneByOne::create();
		eventListenerTouch->setSwallowTouches(true);
		eventListenerTouch->onTouchBegan = CC_CALLBACK_2(ControlUI::onTouchBegan, this);
		eventListenerTouch->onTouchMoved = CC_CALLBACK_2(ControlUI::onTouchMove, this);
		eventListenerTouch->onTouchEnded = CC_CALLBACK_2(ControlUI::onTouchEnded, this);
		eventListenerTouch->onTouchCancelled = CC_CALLBACK_2(ControlUI::onTouchCancelled, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListenerTouch, this);
	}

	return ret;
}

bool ControlUI::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	auto touchPoint = touch->getLocation();
	auto camera = dynamic_cast<Scene*>(_parent)->getDefaultCamera();
	auto size = dynamic_cast<Scene*>(_parent)->getContentSize();
	auto buttonPos = _position - camera->getPosition() + size / 2;

	Rect rect;
	rect.origin = buttonPos - _boundingRect.origin * getScale();
	rect.size = _boundingRect.size * getScale();

	if (rect.containsPoint(touchPoint))
	{
		_touch = touch;
		_capture = true;
		onCapture(_capture);
		return true;
	}

	return false;
}

void ControlUI::onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void ControlUI::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (_touch == touch)
	{
		_touch = nullptr;
		_capture = false;
		onCapture(_capture);
	}
}

void ControlUI::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

bool ControlRocker::init()
{
	bool ret = ControlUI::init();

	if (ret)
	{
		_background = Sprite::create(FILE_IMAGE_CIRCLE);
		_background->setColor(Color3B::BLACK);
		_background->setOpacity(128);
		addChild(_background);

		_ball = Sprite::create(FILE_IMAGE_CIRCLE);
		_ball->setScale(_background->getScale() / 3);
		addChild(_ball);

		_boundingRect.origin = _background->getAnchorPointInPoints()  * _background->getScale();
		_boundingRect.size = _background->getContentSize() * _background->getScale();
	}

	return ret;
}

void ControlRocker::onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (_touch == touch && _actor != nullptr)
	{
		auto touchPoint = touch->getLocation();
		auto camera = dynamic_cast<Scene*>(_parent)->getDefaultCamera();
		auto size = dynamic_cast<Scene*>(_parent)->getContentSize();
		auto buttonPos = _position - camera->getPosition() + size / 2;

		auto deltaPos = touchPoint - buttonPos;
		_actor->moveByDeltaPos(deltaPos);

		auto backgroudSize = _background->getContentSize() * _background->getScale();
		auto ballSize = _background->getContentSize() * _ball->getScale();
		auto maxOff = backgroudSize.width / 2 - ballSize.width / 2;

		if (deltaPos.length() > maxOff)
			_ball->setPosition(deltaPos / deltaPos.length() * maxOff);
		else
			_ball->setPosition(deltaPos);
	}
}

void ControlRocker::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	reset();
}

void ControlRocker::onActorDestroy(EventCustom* event)
{
	ControlUI::onActorDestroy(event);
	reset();
}

inline void ControlRocker::reset()
{
	if (_capture)
	{
        _touch = nullptr;
		_capture = false;
		_ball->setPosition(0, 0);
	}
}

ControlAccelerator* ControlAccelerator::create()
{
	auto accelerator = new(std::nothrow)ControlAccelerator;

	if (accelerator != nullptr && accelerator->init())
	{
		accelerator->autorelease();
		return accelerator;
	}

	CC_SAFE_DELETE(accelerator);
	return nullptr;
}

bool ControlAccelerator::init()
{
	bool ret = ControlUI::init();

	if (ret)
	{
		_background = Sprite::create(FILE_IMAGE_ACCELERATE);
		addChild(_background);

		_boundingRect.origin = _background->getAnchorPointInPoints()  * _background->getScale();
		_boundingRect.size = _background->getContentSize() * _background->getScale();
	}

	return ret;
}

void ControlAccelerator::onCapture(bool capture)
{
	if (_actor != nullptr)
	{
		_actor->setAccelerate(capture);
	}
}

Control::Control():
	_actor(nullptr)
{
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	_eventListener = dispatcher->addCustomEventListener(CUSTOM_EVENT_ACTOR_DESTROY, CC_CALLBACK_1(Control::onActorDestroy, this));
}

Control::~Control()
{
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->removeEventListener(_eventListener);
}

void Control::onActorDestroy(cocos2d::EventCustom* event)
{
	if (static_cast<Actor*>(event->getUserData()) == _actor)
	{
		_actor = nullptr;
	}
}

Actor::Actor()
{

}

Actor::~Actor()
{
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->dispatchCustomEvent(CUSTOM_EVENT_ACTOR_DESTROY, this);
}

void Actor::moveByDeltaPos(float deltaX, float deltaY)
{
	moveByDeltaPos(Vec2(deltaX, deltaY));
}

ControlAI::ControlAI():
	_scene(nullptr)
{
	schedule(CC_SCHEDULE_SELECTOR(ControlAI::queryRect), 2.0f, CC_REPEAT_FOREVER, 0);
}

ControlAI::~ControlAI()
{
	unschedule(CC_SCHEDULE_SELECTOR(ControlAI::queryRect));
}

bool ControlAI::queryRectCallBack(PhysicsWorld& world, PhysicsShape& shape, void* data)
{
	int& count = *static_cast<int*>(data);
	count++;
	return true;
}

void ControlAI::onActorDestroy(cocos2d::EventCustom* event)
{
	if (static_cast<Actor*>(event->getUserData()) == _actor)
	{
		_scheduler->pauseTarget(this);
		_actor = nullptr;
	}
}

void ControlAI::setActor(Actor* actor)
{
	if (actor != nullptr)
	{
		_actor = actor;
		_scheduler->resumeTarget(this);
	}

}

ControlAI* ControlAI::create()
{
	auto ret = new(std::nothrow)ControlAI;

	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

void ControlAI::queryRect(float /* = 0.0f*/)
{
	if (_scene != nullptr)
	{
		auto pos = _actor->getLocation();
		auto size = _actor->getSize() * 16;

		Rect rect(pos.x - size.width / 2, pos.y - size.height / 2, size.width / 2, size.height);
		int leftCount = 0;
		_scene->getPhysicsWorld()->queryRect(CC_CALLBACK_3(ControlAI::queryRectCallBack, this), rect, &leftCount);

		rect.origin.x = pos.x;
		int rightCount = 0;
		_scene->getPhysicsWorld()->queryRect(CC_CALLBACK_3(ControlAI::queryRectCallBack, this), rect, &rightCount);

		if (rightCount > 0 && leftCount > 0)
		{
			if (leftCount - rightCount > 10)
			{
				_actor->moveByDeltaPos(1.0f, 0.0f);
			}
			else if (rightCount - leftCount > 10)
			{
				_actor->moveByDeltaPos(0.0f, 1.0f);
			}
			else
			{
				_actor->moveByDeltaPos(CCRANDOM_0_1() - 0.5f, CCRANDOM_0_1() - 0.5f);
			}
		}
	}
}