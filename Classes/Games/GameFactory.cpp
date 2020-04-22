#include "GameFactory.h"
#include "base/CCDirector.h"
#include "2d/CCMenuItem.h"
#include "2d/CCLabel.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/ccUTF8.h"
#include "2d/CCSprite.h"

USING_NS_CC;

std::map<const std::string, create_game_func> GameFactory::_createFuncs;

void GameFactory::regCreateFunction(const std::string& className, create_game_func func)
{
	if (_createFuncs.find(className) == _createFuncs.end())
	{
		_createFuncs[className] = func;
	}
}

BaseGame* GameFactory::CreateGame(const std::string& className)
{
	if (_createFuncs.find(className) == _createFuncs.end())
		return nullptr;
	else
		return _createFuncs[className](const_cast<std::string&>(className));
}



BaseGame::BaseGame(std::string& className):
	_className(className),
	_againDelayTime(0),
	_menu(nullptr),
	_background(nullptr)
{
}

BaseGame::~BaseGame()
{
}

cocos2d::Vec2 BaseGame::convertGLPositionToSceneSpace(const Vec2& glPoint)
{
	auto pos = glPoint;
	auto rect = Director::getInstance()->getOpenGLView()->getViewPortRect();
	pos.x += _defaultCamera->getPositionX() - rect.size.width / 2;
	pos.y += _defaultCamera->getPositionY() - rect.size.height / 2;

	return pos;
}

bool BaseGame::regCreateFunction(const std::string& className, create_game_func func)
{
	GameFactory::regCreateFunction(className, func);
	return true;
}

void BaseGame::end()
{
	unschedule(CC_SCHEDULE_SELECTOR(BaseGame::waitForAgainPerSecond));
	stop();
	auto director = Director::getInstance();
	director->popScene();
}

bool BaseGame::init()
{
    if (!Scene::init())
    {
        return false;
    }

	using namespace cocos2d::backend;
	auto origin = _director->getVisibleOrigin();
	_background = Sprite::create("Images/background.jpg", Rect(Vec2::ZERO, _contentSize * 2));
	Texture2D::TexParams tp(SamplerFilter::LINEAR, SamplerFilter::LINEAR, SamplerAddressMode::REPEAT, SamplerAddressMode::REPEAT);
	_background->getTexture()->setTexParameters(tp);
	addChild(_background);

	auto visibleSize = _director->getVisibleSize();
	_menu = Menu::create();
	auto closeItem = MenuItemImage::create("Images/close.png", "Images/close.png", [&](Ref*) {end(); });
	closeItem->setScale(2);
	closeItem->setPosition(visibleSize / 2 - closeItem->getContentSize());

	_menu->addChild(closeItem);

	auto label = Label::create();
	label->setTTFConfig(TTFConfig("fonts/arial.ttf", 40.0f));
	label->setColor(Color3B::BLUE);
	auto labelItem = MenuItemLabel::create(label);
	labelItem->setTag(GAME_MENU_LABEL_ITEM);
	_menu->addChild(labelItem);
	addChild(_menu, 1);

	addEventListener();

	return true;
}

void BaseGame::waitForAgainPerSecond(float /*= 0.0f*/)
{
	auto label = (MenuItemLabel*)_menu->getChildByTag(GAME_MENU_LABEL_ITEM);
	label->setString(StringUtils::format("%d",--_againDelayTime));

	if (_againDelayTime == 0)
	{
		_menu->setVisible(false);
	}
}

void BaseGame::again(int delayTime)
{
	_againDelayTime = delayTime;
	_menu->setPosition(_defaultCamera->getPosition());
	_menu->setVisible(true);

	auto label = (MenuItemLabel*)_menu->getChildByTag(GAME_MENU_LABEL_ITEM);
	label->setString(StringUtils::format("%d", _againDelayTime));

	schedule(CC_SCHEDULE_SELECTOR(BaseGame::waitForAgainPerSecond), 1.0f, delayTime, 0.0f);
}

void BaseGame::addEventListener()
{
	auto listenBeforeUpdate = EventListenerCustom::create(Director::EVENT_BEFORE_UPDATE, CC_CALLBACK_1(BaseGame::beforeUpdate, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenBeforeUpdate, this);

	auto listenAfterUpdate = EventListenerCustom::create(Director::EVENT_BEFORE_UPDATE, CC_CALLBACK_1(BaseGame::afterUpdate, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenAfterUpdate, this);

	auto listenerTouch = EventListenerTouchAllAtOnce::create();
	listenerTouch->onTouchesBegan = CC_CALLBACK_2(BaseGame::onTouchesBegan, this);
	listenerTouch->onTouchesMoved = CC_CALLBACK_2(BaseGame::onTouchesMove, this);
	listenerTouch->onTouchesEnded = CC_CALLBACK_2(BaseGame::onTouchesEnded, this);
	listenerTouch->onTouchesCancelled = CC_CALLBACK_2(BaseGame::onTouchesCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, this);

	auto listenKeyboard = EventListenerKeyboard::create();
	listenKeyboard->onKeyPressed = CC_CALLBACK_2(BaseGame::onKeyPressed, this);
	listenKeyboard->onKeyReleased = CC_CALLBACK_2(BaseGame::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenKeyboard, this);
}

