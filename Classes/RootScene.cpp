#include "RootScene.h"
#include "Games/GameFactory.h"

#include "base/CCDirector.h"
#include "base/CCEventMouse.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerMouse.h"
#include "base/ccUTF8.h"

#include "2d/CCMenu.h"
#include "2d/CCMenuItem.h"
#include "2d/CCLabel.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define TABLE_LABEL_TAG 1024
#define TABLE_CELL_HEIGHT 30
#define TABLE_VIEW_WIDTH 400

class CustomTableView : public TableView
{
public:
	static CustomTableView* create(TableViewDataSource* dataSource, Size size)
	{
		auto tableView = new(std::nothrow) CustomTableView;
		if (tableView != nullptr)
		{
			tableView->initWithViewSize(size, nullptr);
			tableView->autorelease();
			tableView->setDataSource(dataSource);
			tableView->_updateCellPositions();
			tableView->_updateContentSize();
		}
		return tableView;
	}

	void onTouchEnded(Touch *touch, Event *event) override
	{
		if (!this->isVisible())
		{
			return;
		}

		if (_touchedCell)
		{
			auto label = (Label*)_touchedCell->getChildByTag(TABLE_LABEL_TAG);

			Rect bbox = label->getBoundingBox();
			bbox.origin = _touchedCell->convertToWorldSpace(bbox.origin);

			if (bbox.containsPoint(touch->getLocation()) && _tableViewDelegate != nullptr)
			{
				_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
				_tableViewDelegate->tableCellTouched(this, _touchedCell);
			}

			_touchedCell = nullptr;
		}

		ScrollView::onTouchEnded(touch, event);
	}

	void onMouseScroll(EventMouse *mouseEvent)
	{
		auto conH = _container->getContentSize().height * _container->getScaleY();
		auto height = _contentSize.height;
		if (conH >= height)
		{
			float moveY = mouseEvent->getScrollY() * 20;

			auto minOffset = this->minContainerOffset();
			auto maxOffset = this->maxContainerOffset();

			auto offset = this->getContentOffset();
			offset.y += moveY;

			if (offset.y < minOffset.y)
			{
				offset.y = minOffset.y;
			}
			else if (offset.y > maxOffset.y)
			{
				offset.y = maxOffset.y;
			}
			this->setContentOffset(offset);
		}
	
	}

protected:
	CustomTableView()
	{
		auto mouseListener = EventListenerMouse::create();
		mouseListener->onMouseScroll = CC_CALLBACK_1(CustomTableView::onMouseScroll, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
	}

};

RootScene* RootScene::create()
{
	auto root = new(std::nothrow)RootScene;
	if (root && root->init())
	{
		root->autorelease();
		return root;
	}

	CC_SAFE_DELETE(root);
	return nullptr;
}

RootScene::RootScene():
	_tableCellSize(TABLE_VIEW_WIDTH, TABLE_CELL_HEIGHT),
	_game(nullptr)
{
}


RootScene::~RootScene()
{
	log("%s", __FUNCTION__);
}

void RootScene::addTableChildName(const std::string& childName)
{
	if (!childName.empty())
	{
		_childNames.emplace_back(StringUtils::format("%d:%s", _childNames.size() + 1, childName.c_str()));
	}
}

bool RootScene::init()
{
	if (!Scene::init())
		return false;

	auto visibleSize = _director->getVisibleSize();
	auto visibleOrigin = _director->getVisibleOrigin();

	/*Label*/
	auto label = Label::createWithTTF("Cocos Game", "fonts/arial.ttf", 40.0f);
	label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - label->getContentSize().height / 2);
	label->setName("CocosGameLabel");
	addChild(label);

	/*Menu*/
	auto item = MenuItemImage::create("Images/close.png", "Images/close.png", [](Ref*) {Director::getInstance()->end(); });
	item->setScale(2);
	auto menu = Menu::create();
	menu->addChild(item);
	menu->setPosition(visibleOrigin + visibleSize - item->getContentSize());
	addChild(menu);

	return true;
	
}

void RootScene::initTableView()
{
	/*TableView*/
	auto director = Director::getInstance();
	auto visibleSize = director->getVisibleSize();
	auto origin = director->getVisibleOrigin();

	auto label = (Label*)getChildByName("CocosGameLabel");
	auto tableView = CustomTableView::create(this, Size(TABLE_VIEW_WIDTH, visibleSize.height - label->getContentSize().height * 2));
	if (tableView != nullptr)
	{
		tableView->setPosition(origin.x + (visibleSize.width - TABLE_VIEW_WIDTH) / 2, origin.y);
		tableView->setDirection(ScrollView::Direction::VERTICAL);
		tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
		tableView->setDelegate(this);
		addChild(tableView);
	}
}

TableViewCell* RootScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto cell = table->dequeueCell();

	if (cell == nullptr)
	{
		cell = new TableViewCell;
		auto label = Label::createWithTTF(_childNames[idx], "fonts/arial.ttf", 20.0f);
		label->setTag(TABLE_LABEL_TAG);
		label->setPosition(TABLE_VIEW_WIDTH / 2, TABLE_CELL_HEIGHT / 2);
		cell->addChild(label);
	}
	else
	{
		auto label = (Label*)cell->getChildByTag(TABLE_LABEL_TAG);
		label->setString(_childNames[idx]);
	}

	return cell;
}

ssize_t RootScene::numberOfCellsInTableView(TableView *table)
{
	return _childNames.size();
}

void RootScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
	std::string gameName = dynamic_cast<Label*>(cell->getChildByTag(TABLE_LABEL_TAG))->getString();
	gameName = gameName.substr(gameName.find_first_of(':') + 1);

	playGame(gameName);
}

Size RootScene::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
	return _tableCellSize;
}

void RootScene::playGame(std::string& gameName)
{
	_game = GameFactory::CreateGame(gameName);
	if (_game == nullptr)
		log("Error | Create %s Game fail", gameName.c_str());
	else
	{
		log("Create %s Game Success", gameName.c_str());
		_game->autorelease();
		Director::getInstance()->pushScene(_game);
		_game->play();
	}
}

void RootScene::onEnter()
{
	Scene::onEnter();
	
	if (_game != nullptr)
	{
		_game = nullptr;
	}
}
