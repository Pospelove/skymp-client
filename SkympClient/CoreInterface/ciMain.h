#pragma once
#include <string>
#include <mutex>
#include <vector>

class SkympClientDll;
class CIAccess;

namespace ci
{
	class IClientLogic
	{
		friend class ::SkympClientDll;
		friend class ::CIAccess;

	public:
		IClientLogic();

		/**
		* Вызывается единожды при запуске игры
		* @note Не вызывается, пока главное меню не отображено
		*/
		virtual void OnStartup() = 0;

		/**
		* Вызывается на каждой итерации главного цикла while(true)
		* @note Код в OnUpdate значительно влияет на производительность
		*/
		virtual void OnUpdate() = 0;

		/**
		* Вызывается, когда мир инициализируется
		* @note Инициализация мира происходит каждый раз при переходе из главного меню в непосредственно игру
		*/
		virtual void OnWorldInit() = 0;

		/**
		* Вызывается при вводе игроком сообщения
		* @param text - текст сообщения
		*/
		virtual void OnChatMessage(const std::wstring &text) = 0;

		/**
		* Вызывается при вводе игроком команды
		* @param cmdText - название команды, включая '/'
		* @param arguments - вектор из аргументов (разделённых пробелами строк, введённых после названия команды)
		*/
		virtual void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) = 0;

		/**
		* Вызывается, когда создание персонажа завершено
		*/
		virtual void OnRaceMenuExit() = 0;

		/**
		* Вызывается, когда игрок выбрасывает предмет(ы)
		* @param itemType - тип предмета
		* @param count - количество
		*/
		virtual void OnItemDropped(const ci::ItemType *itemType, uint32_t count) = 0;

		/**
		* Вызывается, когда игрок использует (уничтожает) предмет
		* @param itemType - тип предмета
		*/
		virtual void OnItemUsed(const ci::ItemType *itemType) = 0;

		/**
		* Вызывается, когда игрок использует предмет как ингредиент при крафте (алхимия, зачарование и т.д.)
		* @param itemType - тип предмета
		*/
		virtual void OnItemUsedInCraft(const ci::ItemType *itemType) = 0;

		/**
		* Вызывается, когда игрок создал предмет
		*/
		virtual void OnCraftFinish(bool isPoison) = 0;

		/**
		* Вызывается, когда игрок взломал замок
		*/
		virtual void OnLockpick() = 0;

	protected:
		 static IClientLogic *clientLogic;
		 static Mutex callbacksMutex;
	};
}