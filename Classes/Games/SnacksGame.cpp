#include "SnacksGame.h"
#include "physics/CCPhysicsWorld.h"
#include "base/CCDirector.h"

USING_NS_CC;

bool SnacksGame::s_bRegister = BaseGame::regCreateFunction(SNACKS_GAME, 
	[](std::string& className) 
	{
		auto game = new(std::nothrow) SnacksGame(className);
		game->init();
		return game;
	});

SnacksGame::SnacksGame(std::string& className):
	BaseGame(className),
	_snack(nullptr),
	_foods(nullptr),
	_moveControl(nullptr),
	_accelerateControl()
{
}


SnacksGame::~SnacksGame()
{
	cocos2d::log("%s",__FUNCTION__);
}

void SnacksGame::collect(SpriteBatchNode* batchNode)
{
	_collected.push_back(batchNode);

	if (_snack == batchNode)
	{
		resetSnack();
	}
	
	_mutex.lock();
	for (auto iter : _npcSnacks)
	{
		if (iter.second == batchNode)
		{
			_npcSnacks.erase(iter.first);
			break;
		}
	}
	_mutex.unlock();
}

void SnacksGame::play()
{
	scheduleUpdate();
	createSnack();

	_foods->start();
	_menu->setVisible(false);

	for (int i = 0; i < 5; i++)
	{
		createNpc();
	}
}

void SnacksGame::stop()
{
	//_foods->stop();
	unscheduleUpdate();
}


void SnacksGame::beforeUpdate(EventCustom* event)
{
	for (auto batchNode : _collected)
	{
		_foods->generateFoodsByBatchNode(batchNode);
	}
	_collected.clear();
}

void SnacksGame::update(float dt)
{
	if (_snack != nullptr)
	{
		auto headPos = _snack->getLocation();
		auto off = headPos - _background->getPosition() + _background->getAnchorPointInPoints();
		auto backgroundSize = _background->getContentSize();

		auto cameraNewPos = _defaultCamera->getPosition();
		auto visibleSize = _director->getVisibleSize();

		cameraNewPos.x = off.x > visibleSize.width / 2
						&& off.x < backgroundSize.width - visibleSize.width / 2
						? headPos.x : cameraNewPos.x;

		cameraNewPos.y = off.y > visibleSize.height / 2
						&& off.y < backgroundSize.height - visibleSize.height / 2
						? headPos.y : cameraNewPos.y;

		_defaultCamera->setPosition(cameraNewPos);


		auto origin = Vec2(cameraNewPos.x - visibleSize.width / 2,
						   cameraNewPos.y - visibleSize.height / 2);

		if (_moveControl != nullptr)
			_moveControl->setPosition(origin + Vec2(80, 80));

		if (_accelerateControl != nullptr)
			_accelerateControl->setPosition(origin + Vec2(visibleSize.width - 80, 80));
	}
}

void SnacksGame::afterUpdate(cocos2d::EventCustom* event)
{

}

void SnacksGame::onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event)
{
	if (_snack == nullptr)
		return;

	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_CTRL:
		_snack->setAccelerate(true);
		break;
	default:
		break;
	}
}

void SnacksGame::onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event)
{
	if (_snack == nullptr)
		return;

	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_CTRL:
		_snack->setAccelerate(false);
		break;
	default:
		break;
	}
}

void SnacksGame::createNpc()
{
	auto snack = SnackSprite::create(_background, FILE_IMAGE_BALL);
	snack->setPosition((CCRANDOM_0_1() - 0.5f) * _contentSize.width, (CCRANDOM_0_1() - 0.5f) * _contentSize.height);
	addChild(snack);

	auto control = ControlAI::create();
	control->setScene(this);
	control->setActor(snack);
	addChild(control);

	_npcSnacks[control] = snack;
}

inline void SnacksGame::createSnack(float)
{
	_snack = SnackSprite::create(_background, FILE_IMAGE_BALL);
	addChild(_snack);

	_moveControl->setActor(_snack);
	_accelerateControl->setActor(_snack);

	_defaultCamera->setPosition(_snack->getPosition());
}

void SnacksGame::resetSnack()
{
	_snack = nullptr;
	again(5);
}

void SnacksGame::again(int delayTime)
{
	BaseGame::again(delayTime);
	scheduleOnce(CC_SCHEDULE_SELECTOR(SnacksGame::createSnack), (float)delayTime);
}

bool SnacksGame::init()
{
	if (!BaseGame::init())
		return false;

	initWithPhysics();
	_physicsWorld->setGravity(Vec2::ZERO);

	/*_foods*/
	_foods = FoodBatchNode::create(FILE_IMAGE_BALL);
	_foods->setBackground(_background);
	addChild(_foods);

	/*_moveControl*/
	_moveControl = ControlRocker::create();
	addChild(_moveControl);

	/*_accelerateControl*/
	_accelerateControl = ControlAccelerator::create();
	addChild(_accelerateControl);

	return true;
}
