#pragma once
#include <MessageIdentifiers.h>

enum : RakNet::MessageID {
	// client ----> server
	ID_HANDSHAKE = ID_USER_PACKET_ENUM,
	ID_UPDATE_MOVEMENT,
	ID_UPDATE_WORLDSPACE,
	ID_UPDATE_CELL,
	ID_UPDATE_LOOK,
	ID_LOADING_START,
	ID_LOADING_STOP,
	ID_PAUSE_START,
	ID_PAUSE_STOP,
	ID_ACTIVATE,
	ID_LOCKPICK,
	ID_DROPITEM,
	ID_USEITEM,
	ID_CRAFT_INGREDIENT,
	ID_CRAFT_FINISH,
	ID_CONTAINER_CHANGED,
	ID_HOST_START,
	ID_HOSTED_OBJECT_MOVEMENT,
	ID_ANIMATION_EVENT_HIT,
	ID_HIT_OBJECT,
	ID_HIT_PLAYER,
	ID_DIALOG_RESPONSE,
	ID_AV_CHANGED,
	ID_EQUIP_ITEM,
	ID_UNEQUIP_ITEM,
	ID_EQUIP_SPELL,
	ID_UNEQUIP_SPELL,
	ID_BOW_SHOT,
	ID_SPELL_RELEASE,
	ID_DATASEARCH_RESULT,
	ID_FORGET_PLAYER,
	ID_POISON_ATTACK,

	// client <---> server
	ID_MESSAGE,
	ID_LEARN_EFFECT,

	// client <---- server
	ID_WRONG_PASS,
	ID_NAME_ALREADY_USED,
	ID_NAME_INVALID,
	ID_WELCOME,
	ID_SERVER_CLOSED_THE_CONNECTION,
	ID_MOVE_TO,
	ID_PLAYER_CREATE,
	ID_PLAYER_DESTROY,
	ID_PLAYER_MOVEMENT,
	ID_PLAYER_NAME,
	ID_PLAYER_PAUSE,
	ID_PLAYER_LOOK,
	ID_PLAYER_FURNITURE,
	ID_PLAYER_INVENTORY,
	ID_PLAYER_SPELLLIST,
	ID_PLAYER_ANIM,
	ID_PLAYER_AV,
	ID_PLAYER_EQUIPMENT,
	ID_PLAYER_MAGIC_EQUIPMENT,
	ID_PLAYER_BOW_SHOT,
	ID_PLAYER_SPELL_RELEASE,
	ID_PLAYER_ACTIVE_EFFECTS,
	ID_SHOW_RACE_MENU,
	ID_SHOW_DIALOG,
	ID_OBJECT_CREATE,
	ID_OBJECT_DESTROY,
	ID_OBJECT_POS_ROT_LOCATION,
	ID_OBJECT_NAME,
	ID_OBJECT_BEHAVIOR,
	ID_OBJECT_KEYWORDS,
	ID_ITEMTYPES,
	ID_WEATHER,
	ID_GLOBAL_VARIABLE,
	ID_SILENT,
	ID_COMMAND,
	ID_CHAT_BUBBLE,
	ID_SPELL,
	ID_EFFECT,
	ID_DATASEARCH_INIT,
	ID_TEXT_CREATE,
	ID_TEXT_DESTROY,
	ID_REGISTER_ANIMATION,
};