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
		* ���������� �������� ��� ������� ����
		* @note �� ����������, ���� ������� ���� �� ����������
		*/
		virtual void OnStartup() = 0;

		/**
		* ���������� �� ������ �������� �������� ����� while(true)
		* @note ��� � OnUpdate ����������� ������ �� ������������������
		*/
		virtual void OnUpdate() = 0;

		/**
		* ����������, ����� ��� ����������������
		* @note ������������� ���� ���������� ������ ��� ��� �������� �� �������� ���� � ��������������� ����
		*/
		virtual void OnWorldInit() = 0;

		/**
		* ���������� ��� ����� ������� ���������
		* @param text - ����� ���������
		*/
		virtual void OnChatMessage(const std::wstring &text) = 0;

		/**
		* ���������� ��� ����� ������� �������
		* @param cmdText - �������� �������, ������� '/'
		* @param arguments - ������ �� ���������� (���������� ��������� �����, �������� ����� �������� �������)
		*/
		virtual void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) = 0;

		/**
		* ����������, ����� �������� ��������� ���������
		*/
		virtual void OnRaceMenuExit() = 0;

		/**
		* ����������, ����� ����� ����������� �������(�)
		* @param itemType - ��� ��������
		* @param count - ����������
		*/
		virtual void OnItemDropped(const ci::ItemType *itemType, uint32_t count) = 0;

		/**
		* ����������, ����� ����� ���������� (����������) �������
		* @param itemType - ��� ��������
		*/
		virtual void OnItemUsed(const ci::ItemType *itemType) = 0;

		/**
		* ����������, ����� ����� ���������� ������� ��� ���������� ��� ������ (�������, ����������� � �.�.)
		* @param itemType - ��� ��������
		* @param count - ����������
		*/
		virtual void OnItemUsedInCraft(const ci::ItemType *itemType, uint32_t coutn) = 0;

		/**
		* ����������, ����� ����� ������ �������
		* @param isPoison - �������� �� ��������� ������� ����
		* @param itemType - ��� ��������, nullptr ��� ������������
		* @param numCraftedItems - ���������� ��������� ��������� (������ 1 ��� ������������)
		*/
		virtual void OnCraftFinish(bool isPoison, const ci::ItemType *itemType, uint32_t numCraftedItems) = 0;

		/**
		* ����������, ����� ����� ������� �����
		*/
		virtual void OnLockpick() = 0;

		/**
		* ����������, ����� ����� ������� �������, �� ������� ������ ��
		*/
		virtual void OnPoisonAttack() = 0;

		/**
		* ����������, ����� ����� ����������� ����������� �� �������
		* @param itemType - ��� ��������
		* @param ench - �����������
		*/
		virtual void OnItemEnchanting(const ci::ItemType *itemType, const ci::Enchantment *ench) = 0;

		static void QueueCallback(std::function<void()> fn);

	protected:
		 static IClientLogic *clientLogic;
	};
}