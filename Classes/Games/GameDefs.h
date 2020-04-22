#pragma once
/*Game Class Name*/
#define SNACKS_GAME "SnacksGame"
#define BOMBER_GAME "BomberGame"

/*Image file*/
#define FILE_IMAGE_BALL "Images/ball.png"
#define FILE_IMAGE_CIRCLE "Images/circle.png"
#define FILE_IMAGE_ACCELERATE "Images/accelerate.png"

/*Menu Item*/
#define GAME_MENU_TAG 1000
#define GAME_MENU_LABEL_ITEM 1001

/*Snacks Game Default Value*/
#define PI 3.14f
#define SNACK_MAX_ANGLE_OF_TURN (PI/3)
#define SNACK_DEFAULT_LENGTH 5
#define SNACK_TRACK_LENGTH 20
#define SNACK_ACTION_DURATION 0.1f

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GAME_OBJECT(x) (0x7F000000 | x)

/*Snacks Game*/
#define SNACK_GAME_OBJECT(x)  (GAME_OBJECT(0x00100000) | x)

#define OBJECT_SNACK SNACK_GAME_OBJECT(0x01)
#define OBJECT_FOOD	 SNACK_GAME_OBJECT(0x02)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Custom Event Name*/
#define CUSTOM_EVENT_ACTOR_DESTROY	"DestroyActorEvent"