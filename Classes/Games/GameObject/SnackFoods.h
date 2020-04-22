#pragma once
#include "2d/CCSprite.h"
#include "2d/CCSpriteBatchNode.h"

class Food :public cocos2d::Sprite
{
public:
	static Food* create(cocos2d::Texture2D* texture);
	void setScore(int score) { _score = score; }
	int getScore() { return _score; }

	Food();
private:
	int _score;
};

class FoodBatchNode : public cocos2d::SpriteBatchNode
{
public:
	static FoodBatchNode* create(const std::string& fileImage);
	void generateFoodsByBatchNode(cocos2d::SpriteBatchNode* batchNode);
	void setBackground(cocos2d::Sprite* background) { _background = background; }

	void start();
	void stop();
	
private:
	void  generateFoods(float = 0.0f);
	Food* generateFood(cocos2d::Texture2D* texture);

private:
	cocos2d::Sprite* _background;
};