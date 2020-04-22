#pragma once
#include "2d\CCScene.h"
#include "GameDefs.h"
#include "base/CCEventMouse.h"
#include "2d/CCCamera.h"
#include "2d/CCMenu.h"

class BaseGame;

typedef std::function<BaseGame*(std::string&)> create_game_func;

class GameFactory
{
public:
	static std::map<const std::string, create_game_func> _createFuncs;

	static void regCreateFunction(const std::string& className, create_game_func func);

	static BaseGame* CreateGame(const std::string& className);
};

class BaseGame :public cocos2d::Scene
{
public:
	static bool regCreateFunction(const std::string& className, create_game_func func);

	std::string getClassName() { return _className; }

	virtual void again(int delayTime);
	void end();

	bool init() override;
	virtual void waitForAgainPerSecond(float = 0.0f);

	virtual void play() = 0;
	virtual	void stop() = 0;

	virtual void addEventListener();
	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {}
	virtual void onTouchesMove(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {}
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {}
	virtual void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {}

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d:: Event* event) {}
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) {}
	
	virtual void beforeUpdate(cocos2d::EventCustom* event) {}
	virtual void afterUpdate(cocos2d::EventCustom* event) {}

	BaseGame(std::string& className);
	~BaseGame();

protected:
	cocos2d::Vec2 convertGLPositionToSceneSpace(const cocos2d::Vec2& glPoint);

protected:
	std::string _className;
	int _againDelayTime;
	cocos2d::Menu* _menu;
	cocos2d::Sprite* _background;
};
