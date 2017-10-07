#include "../stdafx.h"
#include "SkyUILib.h"
#include "../DInput/DInput.hpp"
#include "../Overlay/GUI.h"

namespace SkyUILib
{
	// Helper Funcs:
	bool IsRussianLanguage() {
		return TheGUI && TheGUI->IsRussianLanguage();
	}
	bool IsCustomMenuOpen() {
		return MenuManager::GetSingleton()->IsMenuOpen("CustomMenu");
	}
	void CloseCustomMenu(bool resetControls = true) {
		SET_TIMER(150, [=] {
			MenuManager::GetSingleton()->CloseMenu("CustomMenu");
			if (resetControls)
			{
				sd::Wait(0);
				sd::ToggleMenus();
				MenuManager::GetSingleton()->OpenMenu("Console");
				sd::Wait(0);
				MenuManager::GetSingleton()->CloseMenu("Console");
				sd::ToggleMenus();
			}
		});
	}
	void CloseMsgboxMenu() {
		SET_TIMER(100, [] {
			keybd_event(VK_LSHIFT, DIK_RETURN, NULL, NULL);
			keybd_event(VK_LSHIFT, DIK_RETURN, KEYEVENTF_KEYUP, NULL);
		});
	}

	void ShowDialogList(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex, Callback callback);
	void ShowDialogListImpl(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex);
	void SetDataList(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex, Callback callback);
	void ShowDialogInput(const std::string &title, const std::string &defaultText, Callback callback);
	void ShowDialogInputImpl(const std::string &title, const std::string &defaultText);
	void SetDataInput(const std::string &title, const std::string &defaultText, Callback callback);
	bool HideDialog();

	enum {
		DUMMY = NULL,
	};

	enum StringID {
		LIST_TITLE =				108,
		LIST_NUM_ITEMS =			109,
		LIST_ITEM_START =			110,
		LIST_DEFAULT_INDEX =		108108,
		INPUT_TITLE =				111,
		INPUT_DEFAULT_TEXT =		112,
	};

	enum class MenuType
	{
		None = 0,
		List,
	};

	bool showListResultCalled = false;
	Callback gCallback, gCallbackMsgbox, gCallbackInput;
	bool showInputResultCalled = false;
	dlf_mutex mutex;
	static clock_t lastClose = 0;

	void ShowDialogList(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex, Callback callback)
	{
		std::lock_guard<dlf_mutex> l(mutex);

		if (lastClose + 1000 > clock())
		{
			SET_TIMER(1000, [=] {
				ShowDialogList(title, listItems, defaultIndex, callback);
			});
			return;
		}
		const bool isOpen = MenuManager::GetSingleton()->IsMenuOpen("CustomMenu");
		if (!isOpen)
		{
			SET_TIMER(0, [=] {
				ShowDialogListImpl(title, listItems, defaultIndex);
			});
			gCallback = callback;
		}
	}

	auto prepareStrings = [](const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex) {
		if (defaultIndex < 0)
		{
			defaultIndex = 0;
			ErrorHandling::SendError("WARN:SkyUILib Negative index");
		}

		Costile2::CreateStaticString(title, LIST_TITLE);

		std::string numItems;
		for (size_t i = 0; i != listItems.size(); ++i)
			numItems += "1";
		Costile2::CreateStaticString(numItems, LIST_NUM_ITEMS);

		for (size_t i = 0; i != listItems.size(); ++i)
			Costile2::CreateStaticString(listItems[i], LIST_ITEM_START + i);

		std::string defaultIndexS;
		for (size_t i = 0; i != defaultIndex; ++i)
			defaultIndexS += "1";
		Costile2::CreateStaticString(defaultIndexS, LIST_DEFAULT_INDEX);
	};

	void ShowDialogListImpl(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex)
	{
		prepareStrings(title, listItems, defaultIndex);

		cd::ShowList(DUMMY, [](int32_t) {});

		std::thread([=] {
			std::lock_guard<dlf_mutex> l(mutex);
			if (showListResultCalled == false)
			{
				showListResultCalled = true;
				SET_TIMER(0, [] {
					cd::ShowListResult(DUMMY, [](int32_t index) {
						std::lock_guard<dlf_mutex> l(mutex);
						auto callback = gCallback;
						if (callback)
						{
							std::thread([=] {
								DialogResult result;
								result.inputText = Costile2::GetStaticString(LIST_ITEM_START + index);
								result.listItem = index;
								callback(result);
							}).detach();
						}
						showListResultCalled = false;
						lastClose = clock();
						CloseCustomMenu(false);
					});
				});
			}
		}).detach();
	}

	void SetDataList(const std::string &title, const std::vector<std::string> &listItems, int32_t defaultIndex, Callback callback)
	{
		SET_TIMER(0, [=] {
			prepareStrings(title, listItems, defaultIndex);
			cd::SetDataList(DUMMY, [](int32_t) {});
			std::thread([=] {
				std::lock_guard<dlf_mutex> l(mutex);
				gCallback = callback;
			}).detach();
		});
	}

	void ShowDialogInput(const std::string &title, const std::string &defaultText, Callback callback)
	{
		std::lock_guard<dlf_mutex> l(mutex);

		if (lastClose + 1000 > clock())
		{
			SET_TIMER(1000, [=] {
				ShowDialogInput(title, defaultText, callback);
			});
			return;
		}
		const bool isOpen = MenuManager::GetSingleton()->IsMenuOpen("CustomMenu");
		if (!isOpen)
		{
			SET_TIMER(0, [=] {
				ShowDialogInputImpl(title, defaultText);
			});
			gCallbackInput = callback;
		}
	}

	void ShowDialogInputImpl(const std::string &title, const std::string &defaultText)
	{
		std::lock_guard<dlf_mutex> l(mutex);

		Costile2::CreateStaticString(title, INPUT_TITLE);
		Costile2::CreateStaticString(defaultText, INPUT_DEFAULT_TEXT);

		cd::ShowInput(DUMMY, [](int32_t) {});

		std::thread([=] {
			std::lock_guard<dlf_mutex> l(mutex);
			if (showInputResultCalled == false)
			{
				showInputResultCalled = true;
				SET_TIMER(0, [] {
					cd::ShowInputResult(DUMMY, [](std::string str) {
						std::lock_guard<dlf_mutex> l(mutex);
						auto callback = gCallbackInput;
						if (callback)
						{
							std::thread([=] {
								auto pos = str.rfind(' ');
								if (pos != str.npos)
								{
									const std::string text = { str.begin(), str.begin() + pos };
									const bool cancelled = str[pos + 1] == '1';
									DialogResult result;
									result.inputText = text;
									result.listItem = cancelled ? 1 : 0;
									callback(result);
								}
							}).detach();
						}
						showInputResultCalled = false;
						lastClose = clock();
						CloseCustomMenu(true);
					});
				});
			}
		}).detach();
	}

	void SetDataInput(const std::string &title, const std::string &defaultText, Callback callback)
	{
		SET_TIMER(0, [=] {
			Costile2::CreateStaticString(title, INPUT_TITLE);
			Costile2::CreateStaticString(defaultText, INPUT_DEFAULT_TEXT);
			cd::SetDataInput(DUMMY, [](int32_t) {});
			std::thread([=] {
				std::lock_guard<dlf_mutex> l(mutex);
				gCallbackInput = callback;
			}).detach();
		});
	}

	bool HideDialog()
	{
		std::lock_guard<dlf_mutex> l(mutex);
		{
			const bool isOpen = MenuManager::GetSingleton()->IsMenuOpen("CustomMenu");
			if (isOpen)
			{
				gCallback = nullptr;
				gCallbackInput = nullptr;
				SET_TIMER(0, [] {
					keybd_event(VK_LSHIFT, DIK_ESCAPE, NULL, NULL);
					keybd_event(VK_LSHIFT, DIK_ESCAPE, KEYEVENTF_KEYUP, NULL);
					CloseCustomMenu();
				});
				lastClose = clock();
				return true;
			}
		}

		{
			const bool isOpen = MenuManager::GetSingleton()->IsMenuOpen("MessageBoxMenu");
			if (isOpen)
			{
				gCallbackMsgbox = nullptr;
				lastClose = clock();
				CloseMsgboxMenu();
				return true;
			}
		}
		return false;
	}

	void ShowPlayerDialog(const std::wstring &wTitle, int32_t dialogStyle, const std::wstring &wText, int32_t defaultIndex, Callback callback)
	{
		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			return;

		auto title = WstringToString(wTitle);
		auto text = WstringToString(wText);
		switch (dialogStyle)
		{
		case DIALOG_STYLE_MSGBOX:
		{
			const bool isOpen = MenuManager::GetSingleton()->IsMenuOpen("MessageBoxMenu");
			if (!isOpen)
			{
				SET_TIMER(0, [=] {
					std::thread([=] {
						sd::ShowMessageBox((char *)text.data());
						{
							std::lock_guard<dlf_mutex> l(mutex);
							gCallbackMsgbox = callback;
						}
					}).detach();
					std::thread([] {
						while (1)
						{
							Sleep(100);
							if (!MenuManager::GetSingleton()->IsMenuOpen("MessageBoxMenu"))
								break;
						}
						std::lock_guard<dlf_mutex> l(mutex);
						if (gCallbackMsgbox)
							gCallbackMsgbox({ "", 0 });
					}).detach();
				});
			}
			else
			{
				HideDialog();
				SET_TIMER(100, [=] {
					ShowPlayerDialog(wTitle, dialogStyle, wText, defaultIndex, callback);
				});
			}
			break;
		}

		case DIALOG_STYLE_INPUT:
		{
			auto showInput = IsCustomMenuOpen() ? SetDataInput : ShowDialogInput;
			showInput(title, text, callback);
			break;
		}

		case DIALOG_STYLE_LIST:
			std::vector<std::string> listItems;
			std::istringstream iss(text);
			std::string token;
			while (std::getline(iss, token, '\n'))
				listItems.push_back(token);

			auto showList = IsCustomMenuOpen() ? SetDataList : ShowDialogList;
			showList(title, listItems, defaultIndex, callback);
			break;
		}
	}

}