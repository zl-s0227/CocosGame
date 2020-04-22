#pragma once
#include "2d/CCScene.h"
#include "CCScrollView/CCTableView.h"

class BaseGame;
class RootScene :public cocos2d::Scene, 
				 public cocos2d::extension::TableViewDataSource,
				 public cocos2d::extension::TableViewDelegate
{
public:
	static RootScene* create();

	RootScene();
	~RootScene();

	void addTableChildName(const std::string& childName);

	bool init() override;
	void initTableView();

	void playGame(std::string& gameName);

	/*override functions*/
	cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
	ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
	void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
	cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx) override;

	void onEnter() override;

private:
	std::vector<std::string> _childNames;
	cocos2d::Size _tableCellSize;
	BaseGame* _game;
};

