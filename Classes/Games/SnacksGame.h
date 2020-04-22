#pragma once
#include "GameFactory.h"
#include "GameObject/SnackSprite.h"
#include "Games/GameObject/SnackFoods.h"


class SnacksGame:public BaseGame
{
public:
	SnacksGame(std::string& className);
	~SnacksGame();

	void collect(cocos2d::SpriteBatchNode* batchNode);
	void createSnack(float=0.0f);
	void resetSnack();



protected:
	/*override functions*/
	void play() override;
	void stop() override;
	void again(int delayTime) override;

	bool init() override;

private:
	void beforeUpdate(cocos2d::EventCustom* event) override;
	void update(float dt) override;
	void afterUpdate(cocos2d::EventCustom* event) override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;
	void createNpc();
public:
	static bool s_bRegister;

private:
	SnackSprite* _snack;
	std::map<ControlAI*, SnackSprite*> _npcSnacks;
	std::mutex	_mutex;

	FoodBatchNode* _foods;
	ControlRocker* _moveControl;
	ControlAccelerator* _accelerateControl;
	std::vector<cocos2d::SpriteBatchNode*> _collected;
};