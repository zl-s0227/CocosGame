#include "SnackFoods.h"
#include "Games/GameDefs.h"

USING_NS_CC;

Food* Food::create(Texture2D* texture)
{
	auto food = new(std::nothrow) Food;

	if (food != nullptr && food->initWithTexture(texture))
	{
		food->autorelease();
		return food;
	}

	CC_SAFE_DELETE(food);
	return nullptr;
}

Food::Food():
	_score(5)
{
	_tag = OBJECT_FOOD;
}

FoodBatchNode* FoodBatchNode::create(const std::string& fileImage)
{
	auto foodBatchNode = new(std::nothrow)FoodBatchNode;

	if (foodBatchNode != nullptr && foodBatchNode->initWithFile(fileImage))
	{
		foodBatchNode->autorelease();
		return foodBatchNode;
	}

	CC_SAFE_DELETE(foodBatchNode);
	return nullptr;
}

void FoodBatchNode::generateFoodsByBatchNode(SpriteBatchNode* batchNode)
{
	for (auto child : batchNode->getChildren())
	{
		Food* food = generateFood(batchNode->getTexture());
		food->setScale(batchNode->getScale());

		float offX = (rand() % 200 - 100) / 100.0f * child->getContentSize().width;
		float offY = (rand() % 200 - 100) / 100.0f * child->getContentSize().width;
		auto pos = batchNode->convertToWorldSpace(Vec2(child->getPositionX() + offX, child->getPositionY() + offY));
		pos = this->convertToNodeSpace(pos);

		auto off = _position - _background->getPosition() + _background->getAnchorPointInPoints();
		auto boundary = _background->getContentSize();
		auto foodSize = food->getContentSize() * food->getScale();
		auto borderSize = foodSize * 2 / 3;

		pos.x = pos.x + off.x > boundary.width - borderSize.width ? boundary.width - borderSize.width - off.x
				: pos.x + off.x < borderSize.width ? borderSize.width - off.x
				: pos.x;

		pos.y = pos.y + off.y > boundary.height - borderSize.height ? boundary.height - borderSize.height - off.y
				: pos.y + off.y < borderSize.height ? foodSize.height- off.y
				: pos.y;
		
		food->setPosition(pos);
		addChild(food);
	}

	batchNode->removeFromParent();
}

void FoodBatchNode::start()
{
	for (int i = 0; i < 20; i++)
	{
		generateFoods();
	}
	schedule(CC_SCHEDULE_SELECTOR(FoodBatchNode::generateFoods), 10.0f, CC_REPEAT_FOREVER, 0.0f);
}

void FoodBatchNode::stop()
{
	unschedule(CC_SCHEDULE_SELECTOR(FoodBatchNode::generateFoods));
}

void FoodBatchNode::generateFoods(float /*= 0.0f*/)
{
	auto count = CCRANDOM_0_1() * 100;
	for (int i = 0; i < count; i++)
	{
		addChild(generateFood(_textureAtlas->getTexture()));
	}
}

inline Food* FoodBatchNode::generateFood(Texture2D* texture)
{
	CC_ASSERT(_background);
	CC_ASSERT(texture);

	auto food = Food::create(texture);
	food->setColor(Color3B((uint8_t)(CCRANDOM_0_1() * 255), (uint8_t)(CCRANDOM_0_1() * 255), (uint8_t)(CCRANDOM_0_1() * 255)));
	food->setScale(0.5);

	auto contentSize = food->getContentSize();
	auto backgroundSize = _background->getContentSize();
	auto anchorPoint = _background->getAnchorPointInPoints();
	
	auto positionX =  CCRANDOM_0_1() * (backgroundSize.width - contentSize.width) + contentSize.width / 2  - anchorPoint .x + _background->getPositionX() - _position.x;
	auto positionY =  CCRANDOM_0_1() * (backgroundSize.height - contentSize.height) + contentSize.height / 2 - anchorPoint .y + _background->getPositionY() - _position.y;
	food->setPosition(positionX, positionY);

	auto physicsBody = PhysicsBody::createCircle(contentSize.width / 2);
	physicsBody->setCollisionBitmask(0x00);
	physicsBody->setContactTestBitmask(0x01);
	food->addComponent(physicsBody);

	return food;
}
