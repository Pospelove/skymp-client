#include "Hook_Game.h"
#include "Settings.h"
#include "Tools.h"
#include "Task.h"
#include "InputMap.h"

#include <SKSE/GameMenus.h>
#include <SKSE/SafeWrite.h>
#include <SKSE/GameRTTI.h>
#include <SKSE/PluginAPI.h>
#include <SKSE/HookUtil.h>
#include <Skyrim/BSDevices/InputDevice.h>
#include <Skyrim/BSDevices/InputManager.h>
#include <Skyrim/BSDevices/KeyCode.h>
#include <Skyrim/Forms/BGSEquipSlot.h>
#include <Skyrim/Forms/PlayerCharacter.h>
#include <Skyrim/BSDevices/InputMappingManager.h>
#include <Skyrim/BSDevices/MenuControls.h>
#include <Skyrim/Camera/PlayerCamera.h>
#include <SKyrim/BSDevices/PlayerControls.h>
#include <Skyrim/NetImmerse/NiAVObject.h>
#include <Skyrim/Forms/Character/Components/ActorProcessManager.h>
#include <Skyrim/Animation/IAnimationGraphManagerHolder.h>
#include <Skyrim/Menus/Inventory3DManager.h>
#include <Skyrim/FileIO/BGSSaveLoadManager.h>
#include <Skyrim/BSMain/Setting.h>
#include <Skyrim/FileIO/TESDataHandler.h>
#include <Skyrim/FormComponents/TESFullName.h>
//#include <Skyrim/BSCore/BSTArray.h>

#include <regex>
#include <vector>
#include <string>
#include <algorithm>

#define STDFN __stdcall


class MenuOpenCloseEventHandler : public BSTEventSink<MenuOpenCloseEvent>
{
public:
	static SInt32							unpausedCount;
	//127248C		disable/enable save?
	//12C2414		float,time multipler?
	virtual EventResult ReceiveEvent(MenuOpenCloseEvent *evn, BSTEventSource<MenuOpenCloseEvent> *src) override
	{
		//_MESSAGE("menuName�� %s", evn->menuName.c_str());
		auto it = settings.m_menuConfig.find(std::string(evn->menuName.c_str()));
		if (it != settings.m_menuConfig.end())
		{
			MenuManager* mm = MenuManager::GetSingleton();
			UIStringHolder* holder = UIStringHolder::GetSingleton();

			if (evn->opening)
			{
				MenuControls* menuControls = MenuControls::GetSingleton();
				*((bool*)(menuControls->unk28) + 0x25) = true;

				InputMappingManager* input = InputMappingManager::GetSingleton();
				if (evn->menuName == holder->containerMenu 
					|| evn->menuName == holder->barterMenu 
					|| evn->menuName == holder->giftMenu 
					|| evn->menuName == holder->inventoryMenu 
					|| evn->menuName == holder->magicMenu)
				{
					input->DisableControl(InputMappingManager::ContextType::kContext_Gameplay);
				}
				if (unpausedCount++ == 0)
				{
					if (mm->IsMenuOpen(holder->dialogueMenu) && (evn->menuName == holder->containerMenu 
						|| evn->menuName == holder->barterMenu 
						|| evn->menuName == holder->giftMenu 
						|| evn->menuName == holder->trainingMenu))
					{
						GFxMovieView* view = mm->GetMovieView(holder->dialogueMenu);
						view->SetVisible(false);
					}
					PlayerControls* control = PlayerControls::GetSingleton();
					for (auto element : control->handlers)
					{
						*((UInt32*)element + 1) = false;
					}
					//*((UInt32*)control->lookHandler + 1) = true;
				}
				//input->DisableControl(InputMappingManager::ContextType::kContext_ItemMenu);
				//input->EnableControl(InputMappingManager::ContextType::kContext_Gameplay);

				if (evn->menuName == holder->containerMenu)
				{
					TESObjectREFR* ref = nullptr;
					void(__cdecl* GetContainerOwner)(void*, TESObjectREFR*&) = (void(__cdecl*)(void*, TESObjectREFR*&))0x004A9180;
					GetContainerOwner((void*)0x01B3E764, ref);
					if (ref != nullptr)
					{
						UInt32& lootMode = *(UInt32*)0x01B3E6FC;
						UInt32 iActivateDist = g_gameSettingCollection->Get("iActivatePickLength")->data.u32 + 50;//iActivatePickLength

						if (settings.m_fadeOutDist >= iActivateDist && ref->Is(FormType::Character) && !ref->IsDead(false) && lootMode == 2)
						{
#ifdef DEBUG_LOG
							_MESSAGE("PICKPOCKETING...");
							float distance = g_thePlayer->GetDistance(ref, true, false);
							_MESSAGE("formType: %d    formName: %s", (UInt32)ref->formType, ref->GetFullName());
#endif
							auto fn = [=]()->bool {
								UIStringHolder* holder = UIStringHolder::GetSingleton();
								MenuManager* mm = MenuManager::GetSingleton();
								while (mm->IsMenuOpen(holder->containerMenu) && !ref->IsDisabled() && !ref->IsDeleted())
								{
									float distance = g_thePlayer->GetDistance(ref, true, false);
									if (distance > settings.m_fadeOutDist)
									{
										mm->CloseMenu(holder->containerMenu);
										break;
									}
#ifdef DEBUG_LOG
									_MESSAGE("distance: %.2f", distance);
#endif
									std::this_thread::sleep_for(std::chrono::milliseconds(300));
								}
								return true;
							};
							really_async(fn);
						}
					}
				}
			}
			else
			{
				InputMappingManager* input = InputMappingManager::GetSingleton();
				if (evn->menuName == holder->containerMenu 
					|| evn->menuName == holder->barterMenu 
					|| evn->menuName == holder->giftMenu 
					|| evn->menuName == holder->inventoryMenu 
					|| evn->menuName == holder->magicMenu)
				{
					input->EnableControl(InputMappingManager::ContextType::kContext_Gameplay);
				}
				if (--unpausedCount < 0)
					unpausedCount = 0;
				if (unpausedCount == 0)
				{
					PlayerControls* control = PlayerControls::GetSingleton();
					for (auto element : control->handlers)
					{
						*((UInt32*)element + 1) = true;
					}
					MenuControls* menuControls = MenuControls::GetSingleton();
					*((bool*)(menuControls->unk28) + 0x25) = false;

					if (mm->IsMenuOpen(holder->dialogueMenu))
					{
						GFxMovieView* view = mm->GetMovieView(holder->dialogueMenu);
						if (!view->GetVisible())
						{
							view->SetVisible(true);
						}
					}
				}
				if (evn->menuName == holder->console)
				{
					IMenu* console = mm->GetMenu(holder->console);
					if (console != nullptr)
					{
						if (settings.m_menuConfig[holder->console.c_str()])
							console->flags &= ~IMenu::kType_PauseGame;
						else
							console->flags |= IMenu::kType_PauseGame;
					}
				}
			}
		}
		return kEvent_Continue;
	}

	static MenuOpenCloseEventHandler* GetSingleton()
	{
		static MenuOpenCloseEventHandler instance;
		return &instance;
	}
};
SInt32		MenuOpenCloseEventHandler::unpausedCount = 0;


void* STDFN Hook_GetFavoritesItem(void* form)
{
	MenuManager* manager = MenuManager::GetSingleton();
	UIStringHolder* holder = UIStringHolder::GetSingleton();
	if (holder && manager && manager->IsMenuOpen(holder->favoritesMenu))
	{
		form = nullptr;
	}
	return form;
}

void STDFN Hook_KillActor(Actor* actor)
{
	PlayerCharacter* player = DYNAMIC_CAST<PlayerCharacter*>(actor);
	if (player != nullptr)
	{
		bool isProcessing = false;
		MenuManager* mm = MenuManager::GetSingleton();
		for (auto pair_ = settings.m_menuConfig.begin(); pair_ != settings.m_menuConfig.end(); ++pair_)
		{
			auto &pair = *pair_;
			BSFixedString menu(pair.first.c_str());
			if (mm->IsMenuOpen(menu))
			{
				mm->CloseMenu(menu);
				isProcessing = true;
			}
		}
		if (isProcessing) Sleep(200);
	}
	//MenuControls* menuControls = MenuControls::GetSingleton();
	//menuControls->CloseAllMenus();
}

bool STDFN Hook_IsInMenuMode()
{
	MenuManager* mm = MenuManager::GetSingleton();
	for (auto pair_ = settings.m_menuConfig.begin(); pair_ != settings.m_menuConfig.end(); ++pair_)
	{
		auto &pair = *pair_;
		BSFixedString menu(pair.first.c_str());
		if (mm->IsMenuOpen(menu))
			return true;
	}
	if (*(UInt8*)0x1B2E85F || *(UInt8*)0x1B2E85E)
		return true;
	return false;
}

bool STDFN Hook_AddUIMessage(const BSFixedString& strData, UInt32 msgID)
{
	UIStringHolder* holder = UIStringHolder::GetSingleton();
	if (msgID == UIMessage::kMessage_Open && MenuOpenCloseEventHandler::unpausedCount != 0 && (strData == holder->sleepWaitMenu || strData == holder->favoritesMenu|| strData == "Loot Menu"))
	{
		return false;
	}
	return true;
}


class PlayerEquipemntUpdater : public TaskDelegate
{
public:
	TES_FORMHEAP_REDEFINE_NEW();

	void Run() override
	{
		g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
	}
	void Dispose() override
	{
		delete this;
	}
	static void Register()
	{
		const SKSEPlugin *plugin = SKSEPlugin::GetSingleton();
		const SKSETaskInterface *task = plugin->GetInterface(SKSETaskInterface::Version_2);
		if (task)
		{
			PlayerEquipemntUpdater *delg = new PlayerEquipemntUpdater();
			task->AddTask(delg);
		}
	}
};



class FavoritesMenu : public IMenu,
					  public MenuEventHandler
{
public:

	struct ItemData
	{
		TESForm*				form;
		InventoryEntryData*	    objDesc;
	};

	struct EquipManager
	{
		static EquipManager * GetSingleton(void)
		{
			return *((EquipManager **)0x012E5FAC);
		}
		DEFINE_MEMBER_FN(EquipSpell, void, 0x006EFAC0, Actor * actor, TESForm * item, BGSEquipSlot* slot);
		DEFINE_MEMBER_FN(EquipShout, void, 0x006F0110, Actor * actor, TESForm * item);
		DEFINE_MEMBER_FN(EquipItem, void, 0x006EF820, Actor * actor, InventoryEntryData* unk0, BGSEquipSlot* slot, bool unk1);
	};

	void EquipItem(BGSEquipSlot* slot)
	{
		GFxMovieView* view = this->GetMovieView();
		GFxValue result;
		view->Invoke("_root.GetSelectedIndex", &result, nullptr, 0);

		if (result.GetType() == GFxValue::ValueType::VT_Number && result.GetNumber() != -1.00f)
		{
			if (this->unk40 && unk38 != nullptr)
			{
				UInt32 index = static_cast<UInt32>(result.GetNumber());
				TESForm* form = unk38[index].form;
				if (form != nullptr)
				{
					if (form->formType == FormType::Spell)
					{
						EquipManager::GetSingleton()->EquipSpell(g_thePlayer, form, slot);
					}
					else if (form->formType == FormType::Shout)
					{
						if (!g_thePlayer->IsEquipShoutDisabled())
						{
							EquipManager::GetSingleton()->EquipShout(g_thePlayer, form);
						}
					}
					else
					{
						InventoryEntryData* objDesc = unk38[index].objDesc;
						EquipManager::GetSingleton()->EquipItem(g_thePlayer, objDesc, slot, true);
						if (g_thePlayer->processManager != nullptr)
						{
							PlayerEquipemntUpdater::Register();
							//g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
							if (form->formType == FormType::Armor)
							{
								void* unk1 = g_thePlayer->sub_4D6630();
								void* unk2 = ((void* (__cdecl*)(TESForm*))0x447CA0)(form);
								bool(__fastcall* sub_447C20)(void*, void*, void*) = (bool(__fastcall*)(void*, void*, void*))0x00447C20;
								if (sub_447C20(unk2, nullptr, unk1))
								{
									g_thePlayer->sub_73D9A0();
								}
							}
						}
					}
					((void(__cdecl*)(char*))0x00899620)("UIFavorite");
					this->unk45 = true;
					((void(__cdecl*)(Actor*, bool))0x00897F90)(g_thePlayer, false);
				}
			}
		}
		if ((result.GetType() >> 6) & 1)
		{
			result.pObjectInterface->ObjectRelease(&result, result.Value.pData);
		}
	}

	static void InitHook()
	{
		SafeWrite8(0x0085BE1D, 0x00);
		WriteRelJump(0x0085B5E0, GetFnAddr(&FavoritesMenu::EquipItem));
	}

	UInt32						unk28;
	UInt32						unk2C;
	UInt32						unk30;
	UInt32						unk34;
	ItemData*					unk38;
	UInt32						unk3C;
	UInt32						unk40;
	bool						unk44;
	bool						unk45;
	//
	// ....
};
static_assert(sizeof(FavoritesMenu) == 0x48, "sizeof(FavoritesMenu) != 0x48");



class BookMenu : public IMenu,
				 public SimpleAnimationGraphManagerHolder,
				 public BSTEventSink<BSAnimationGraphEvent>
{
public:

	class GlobalBookData
	{
	public:
		ExtraTextDisplayData*			extraData;
		UInt32							unk04;
		TESObjectBOOK*					book;
		UInt32							padding;
		char*							content;
		UInt16							textLen;
		UInt16							buffLen;
		TESObjectREFR*					reference;
		float							unk1C;
		float							unk20;
		float							unk24;
		UInt32							unk28;

		static GlobalBookData* GetSingleton()
		{
			return (GlobalBookData*)0x01B3E520;
		}
	};

	class BookContentUpdater : public TaskDelegate
	{
	public:
		TES_FORMHEAP_REDEFINE_NEW();

		BookContentUpdater(bool direction) : m_direction(direction){}

		virtual void Run() override
		{
			UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
			MenuManager* mm = MenuManager::GetSingleton();
			if (mm->IsMenuOpen(stringHolder->bookMenu))
			{
				BookMenu* bookMenu = static_cast<BookMenu*>(mm->GetMenu(stringHolder->bookMenu));
				if (bookMenu != nullptr)
				{
					bookMenu->bookView->SetPause(false);
					if (bookMenu->bookView->GetVariableDouble("BookMenu.BookMenuInstance.iPaginationIndex") != -1.00f)
					{
						GFxValue result;
						bookMenu->bookView->Invoke("BookMenu.BookMenuInstance.CalculatePagination", &result, nullptr, 0); //ֻ�ܷ�һҳ��
						bookMenu->bookView->Invoke("BookMenu.BookMenuInstance.CalculatePagination", &result, nullptr, 0); //ֻ�ܷ�һҳ��
					}
					bookMenu->bookView->SetPause(true);
					bookMenu->TurnPage(m_direction);
				}
			}
		}

		virtual void Dispose() override
		{
			delete this;
		}

		static void Queue(bool direction)
		{
			UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
			MenuManager* mm = MenuManager::GetSingleton();

			if (mm->IsMenuOpen(stringHolder->bookMenu))
			{
				BookMenu* bookMenu = static_cast<BookMenu*>(mm->GetMenu(stringHolder->bookMenu));
				if (bookMenu != nullptr)
				{
					bool(__cdecl* IsGameRuning)(bool mask) = (bool(__cdecl*)(bool mask))0x006F3570;

					if (!IsGameRuning(true))
					{
						bookMenu->TurnPage(direction);
					}
					else if(!(bookMenu->flags & IMenu::kType_PauseGame))
					{
						const SKSEPlugin *plugin = SKSEPlugin::GetSingleton();
						const SKSETaskInterface *task = plugin->GetInterface(SKSETaskInterface::Version_2);
						if (task)
						{
							BookContentUpdater *delg = new BookContentUpdater(direction);
							task->AddTask(delg);
						}
					}
				}
			}
		}

	private:
		bool							m_direction;
	};


	typedef UInt32(BookMenu::*FnProcessMessage)(UIMessage*);
	typedef EventResult(BookMenu::*FnReceiveEvent)(BSAnimationGraphEvent*, BSTEventSource<BSAnimationGraphEvent>*);

	static FnProcessMessage					fnProcessMessage;
	static FnReceiveEvent					fnReceiveEvent;



	EventResult	ReceiveEvent_Hook(BSAnimationGraphEvent * evn, BSTEventSource<BSAnimationGraphEvent> * source)
	{
		EventResult result = (this->*fnReceiveEvent)(evn, source);
		if (evn->animName == "PageBackStop" || evn->animName == "PageForwardStop" || evn->animName == "OpenStop")
		{

		}
		return result;
	}

	void TurnPage(bool direction)
	{
		bool animationVariable = false;
		this->GetAnimationVariableBool("bPageFlipping", animationVariable);//�Ƿ����ڷ�ҳ�С�
		if (!animationVariable)
		{
			GFxValue result;
			GFxValue argument;

			((void(__stdcall*)(GFxValue&, UInt32, UInt32, void*))0x00402290)(argument, 0x10, 1, (void*)0x0040F490);//����GFxValue����,����ֻ��Ҫһ���͹��ˡ�

			double turnPageNumber = (this->isNote) ? 1.00f : 2.00f;
			if (!direction)
				turnPageNumber = -turnPageNumber;

			argument.SetNumber(turnPageNumber);
			result.SetBoolean(false);

			bool(__fastcall* Invoke)(GFxMovieView*, void*, char*, GFxValue*, GFxValue*, UInt32) = (bool(__fastcall*)(GFxMovieView*, void*, char*, GFxValue*, GFxValue*, UInt32))(*(*(UInt32**)this->bookView + 0x17));
			if (Invoke(this->bookView, nullptr, "BookMenu.BookMenuInstance.TurnPage", &result, &argument, 1) && result.GetBool())
			{
				if (direction)
					this->SendAnimationEvent("PageForward");
				else
					this->SendAnimationEvent("PageBack");

				if (this->isNote)
				{
					((void(__cdecl*)(char*))0x00899620)("ITMNotePageTurn");
				}
			}
			((void(__stdcall*)(GFxValue&, UInt32, UInt32, void*))0x004022D0)(argument, 0x10, 1, (void*)0x00691680); //GFxValue���������������
		}
	}


	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		UIStringHolder* holder = UIStringHolder::GetSingleton();
		if (settings.m_menuConfig[holder->bookMenu.c_str()])
		{
			InputStringHolder* input = InputStringHolder::GetSingleton();
			MenuManager* mm = MenuManager::GetSingleton();

			if (msg->type == UIMessage::kMessage_Message)
			{
				BSUIMessageData* msgData = static_cast<BSUIMessageData*>(msg->data);

				if (msgData->unk0C == input->prevPage || msgData->unk0C == input->leftEquip)
				{
					BookContentUpdater::Queue(false);
				}
				else if (msgData->unk0C == input->nextPage || msgData->unk0C == input->rightEquip)
				{
					BookContentUpdater::Queue(true);
				}
				else if (msgData->unk0C == input->accept || msgData->unk0C == input->cancel)
				{
					bool animationVariable = false;
					this->GetAnimationVariableBool("bPageFlipping", animationVariable);//�Ƿ����ڷ�ҳ�С�
					if (!*(bool*)0x1B3E5D4 || this->disableCloseMsg || animationVariable)
						return 0;
					GlobalBookData* data = GlobalBookData::GetSingleton();
					if (msgData->unk0C == input->accept && data->reference != nullptr)
					{
						g_thePlayer->PickUpItem(data->reference, 1, false, true);
					}
					auto fn = [=]() {
						GFxValue result;
						this->bookView->Invoke("BookMenu.BookMenuInstance.PrepForClose", &result, nullptr, 0);
						mm->CloseMenu(holder->bookMenu);
					};
					CallbackDelegate::Register<CallbackDelegate::kType_Normal>(fn);
				}
				return 1;
			}

			if (msg->type == UIMessage::kMessage_Scaleform)
			{
				if (!*(bool*)0x1B3E5D4)
					return 0;

				BSUIScaleformData* scaleformData = static_cast<BSUIScaleformData*>(msg->data);
				GFxEvent* event = scaleformData->event;

				if (event->type == GFxEvent::KeyDown)
				{
					GFxKeyEvent* key = static_cast<GFxKeyEvent*>(event);
					if (key->keyCode == GFxKey::Code::Left || key->keyCode == GFxKey::Code::Right)
					{
						BookContentUpdater::Queue(key->keyCode == GFxKey::Code::Right);
					}
					else if (this->isNote && (key->keyCode == GFxKey::Code::Up || key->keyCode == GFxKey::Code::Down))
					{
						BookContentUpdater::Queue(key->keyCode == GFxKey::Code::Down);
					}
				}
				return 1;
			}
			return (this->*fnProcessMessage)(msg);
		}
		else
		{
			return (this->*fnProcessMessage)(msg);
		}
	}

	void SetBookText_Hook()
	{
		bool(__cdecl* IsGameRuning)(bool mask) = (bool(__cdecl*)(bool mask))0x006F3570;
		if (!IsGameRuning(true))
		{
			this->SetBookText();

			auto fn = [](){
				UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
				MenuManager* mm = MenuManager::GetSingleton();
				if (mm->IsMenuOpen(stringHolder->bookMenu))
				{
					BookMenu* bookMenu = static_cast<BookMenu*>(mm->GetMenu(stringHolder->bookMenu));
					if (bookMenu != nullptr)
					{
						bookMenu->bookView->SetPause(true);
						if (bookMenu->bookView->GetVariableDouble("BookMenu.BookMenuInstance.iPaginationIndex") != -1.00f)
						{
							GFxValue result;
							bookMenu->bookView->Invoke("BookMenu.BookMenuInstance.CalculatePagination", &result, nullptr, 0);
							bookMenu->bookView->Invoke("BookMenu.BookMenuInstance.CalculatePagination", &result, nullptr, 0);
						}
						if (mm->numPauseGame && (bookMenu->flags & IMenu::kType_PauseGame))
						{
							mm->numPauseGame -= 1;
							bookMenu->flags &= ~IMenu::kType_PauseGame;
							if (!mm->numPauseGame)
							{
								MenuModeChangeEvent event;
								event.unk0 = 0;
								event.menuMode = 0;

								mm->BSTEventSource<MenuModeChangeEvent>::SendEvent(&event);
								typedef void(__fastcall * Fn)(void*, void*);
								Fn ReleaseObject = (Fn)0xA511B0;
								ReleaseObject(&event, nullptr);
							}
						}
					}
				}
			};
			UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
			if (settings.m_menuConfig[stringHolder->bookMenu.c_str()])
			{
				CallbackDelegate::Register<CallbackDelegate::kType_Normal>(fn);
			}
		}
	}

	static void InitHook()
	{
		fnProcessMessage = SafeWrite32(0x010E3AA4 + 0x04 * 4, &ProcessMessage_Hook);

		//fnReceiveEvent = SafeWrite32(0x010E3A98 + 0x01 * 4, &ReceiveEvent_Hook);

		WriteRelCall(0x008464AC, GetFnAddr(&SetBookText_Hook));

		SafeWrite8(0x0084672B, 0x90);
		SafeWrite32(0x0084672C, 0x90909090);
		//Bookmenu depth
		SafeWrite8(0x00845618, 0x3);
	}

	//members:
	UInt32					unk2C;					
	UInt32					unk30;
	UInt32					unk34;
	GFxMovieView*			bookView;				
	NiAVObject*				unk3C;					
	UInt32					unk40;				
	UInt32					unk44;				
	UInt32					unk48;					
	UInt32					unk4C;				
	NiSourceTexture*		unk50;				
	NiTriShape*				unk54;					
	UInt16					unk58;					
	UInt16					refreshTimes;		
	UInt8					disableCloseMsg;		
	bool					isNote;					
	bool					isInit;					
	UInt8					padding;		

	DEFINE_MEMBER_FN(InitBookMenu, void, 0x008451C0);
	DEFINE_MEMBER_FN(CreateBookLayout, void, 0x00845F70);
	DEFINE_MEMBER_FN(SetBookText, void, 0x00845D60);
	DEFINE_MEMBER_FN(ctor, BookMenu*, 0x00845550);
	//Wrapper<Book Menu>::Register()		ctor: 00845C60  845550
};
static_assert(sizeof(BookMenu) == 0x60, "sizeof(BookMenu) != 0x60");
BookMenu::FnProcessMessage	BookMenu::fnProcessMessage = nullptr;
BookMenu::FnReceiveEvent	BookMenu::fnReceiveEvent = nullptr;



class LockpickingMenu : public IMenu
{
public:
	typedef UInt32(LockpickingMenu::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;


	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		return (this->*fnProcessMessage)(msg);
	}

	static void __stdcall Hook_SetLockInfo()
	{
		UIStringHolder* holder = UIStringHolder::GetSingleton();
		MenuManager* manager = MenuManager::GetSingleton();
		if (holder && manager && manager->IsMenuOpen(holder->lockpickingMenu))
		{
			IMenu* lockpickingMenu = manager->GetMenu(holder->lockpickingMenu);
			if (settings.m_menuConfig[holder->lockpickingMenu.c_str()] && (lockpickingMenu->flags & IMenu::kType_PauseGame))
			{
				manager->numPauseGame -= 1;
				lockpickingMenu->flags &= ~IMenu::kType_PauseGame;
				if (!manager->numPauseGame)
				{
					static MenuModeChangeEvent event;
					event.unk0 = 0;
					event.menuMode = 0;
					manager->BSTEventSource<MenuModeChangeEvent>::SendEvent(&event);

					typedef void(__fastcall * Fn)(void*, void*);
					Fn ReleaseObject = (Fn)0xA511B0;
					ReleaseObject(&event, nullptr);
				}
			}
		}
	}

	static void InitHook()
	{
		{
			static const UInt32 kHook_SetLockInfo_Call = 0x0086FE10;

			START_ASM(Hook_SetLockInfo, 0x0087053D, 0x00870549, 1);
				add esp, 0xC
				lea ecx, [esp + 0x38]
				call [kHook_SetLockInfo_Call]
				pushad
				call Hook_SetLockInfo
				popad
			END_ASM(Hook_SetLockInfo);
		}
	}
};
LockpickingMenu::FnProcessMessage	LockpickingMenu::fnProcessMessage = nullptr;



class DialogueMenuEx :public IMenu
{
public:
	typedef UInt32(DialogueMenuEx::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;

	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		if (msg->type == UIMessage::kMessage_Open && MenuOpenCloseEventHandler::unpausedCount != 0)
		{
			this->menuDepth = 0;
			//GFxMovieView* view = this->GetMovieView();
		}
		return (this->*fnProcessMessage)(msg);
	}

	static void InitHook()
	{
		//fnProcessMessage = SafeWrite32(0x010E4C9C + 0x04 * 4, &ProcessMessage_Hook);
		//Dialogue menu depth
		SafeWrite8(0x0085A231, 0x0);
	}
};
DialogueMenuEx::FnProcessMessage	DialogueMenuEx::fnProcessMessage = nullptr;



class InventoryMenuEx : public IMenu
{
public:
	typedef UInt32(InventoryMenuEx::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;

	void Release_Hook()
	{
		if (*(UInt32*)this == 0x010E5B90 && !(this->flags & IMenu::kType_PauseGame))
		{
			if (InterlockedExchangeAdd((volatile long*)((char*)this + 4), -1) == 1)
			{
				void(__fastcall* sub_897160)(void*, void*) = (void(__fastcall*)(void*, void*))0x00897160;//Camera mode.
				sub_897160(*(void**)0x1B2E4C0, nullptr);
				//  sub_897130(dword_1B2E4C0);

				void(__cdecl* sub_861470)(void*) = (void(__cdecl*)(void*))0x861470;  //Enable prompt.
				sub_861470((void*)0x10E3824);

				InputMappingManager* input = InputMappingManager::GetSingleton();
				input->DisableControl(InputMappingManager::ContextType::kContext_Inventory);
				input->DisableControl(InputMappingManager::ContextType::kContext_ItemMenu);
				input->DisableControl(InputMappingManager::ContextType::kContext_MenuMode);

				//A67260  Disable();
				//void(__fastcall* sub_42F240)(void*, void*, bool) = (void(__fastcall*)(void*, void*, bool))0x0042F240;  //???????????
				//sub_42F240(*(void**)0x12E3534, nullptr, 1);

				if (*((UInt32*)this + 0xF)) // Need test.
				{
					void(__fastcall* sub_A49D90)(void*, void*) = (void(__fastcall*)(void*, void*))0x00A49D90;
					sub_A49D90((char*)this + 0x3C, nullptr);
					*((UInt32*)this + 0x11) = 0;
				}
				//Another funciton here.
				GFxValue* root = (GFxValue*)((char*)this + 0x20);
				if ((*(UInt32*)((char*)this + 0x24) >> 6) & 1)
				{
					void(__fastcall* ObjectRelease)(void*, void*, void*, void*) = (void(__fastcall*)(void*, void*, void*, void*))0x922660;
					ObjectRelease(*(void**)root, nullptr, root, *((void**)this + 0xA));
					*(void**)root = nullptr;
				}

				typedef void(__fastcall* Fn)(void*, void*);
				Fn fn = (Fn)0x00A64A10;
				fn(this, nullptr);

				GMemory::Free(this);
			}
			return;
		}
		this->Release();
	}

	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		UInt32 result = (this->*fnProcessMessage)(msg);

		if (msg->type == UIMessage::kMessage_Scaleform)
		{
			BSUIScaleformData* scaleformData = static_cast<BSUIScaleformData*>(msg->data);
			GFxEvent* event = scaleformData->event;
			if (event->type == GFxEvent::KeyDown)
			{
				GFxKeyEvent* key = static_cast<GFxKeyEvent*>(event);
				if (key->keyCode == GFxKey::Code::I && !InputMappingManager::GetSingleton()->unk98)
				{
					GFxValue result;
					GFxMovieView* view = this->GetMovieView();
					if (view != nullptr)
					{
						view->Invoke("_root.Menu_mc.onExitMenuRectClick", &result, nullptr, 0);
					}
				}
			}
		}
		else if (msg->type == UIMessage::kMessage_Open)
		{
			MenuManager* mm = MenuManager::GetSingleton();
			UIStringHolder* holder = UIStringHolder::GetSingleton();
			if (mm->IsMenuOpen(holder->tweenMenu))
			{
				GFxMovieView* view = mm->GetMovieView(holder->tweenMenu);
				view->SetVisible(false);
			}
		}
		else if (msg->type == UIMessage::kMessage_Close)
		{
			UIStringHolder* holder = UIStringHolder::GetSingleton();
			MenuManager* mm = MenuManager::GetSingleton();
			if (mm->IsMenuOpen(holder->tweenMenu))
			{
				PlayerCamera* camera = PlayerCamera::GetSingleton();//kCameraState_Free
				camera->ResetCamera();
				mm->CloseMenu(holder->tweenMenu);
				*((bool*)this + 0x51) = true;
			}
		}
		return result;
	}

	static void InitHook()
	{
		fnProcessMessage = SafeWrite32(0x010E5B90 + 0x04 * 4, &ProcessMessage_Hook);
		WriteRelCall(0x00A5D686, &Release_Hook);
	}
};
InventoryMenuEx::FnProcessMessage	InventoryMenuEx::fnProcessMessage = nullptr;



class MagicMenuEx : public IMenu
{
public:
	typedef UInt32(MagicMenuEx::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;

	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		UInt32 result = (this->*fnProcessMessage)(msg);

		if (msg->type == UIMessage::kMessage_Scaleform)
		{
			BSUIScaleformData* scaleformData = static_cast<BSUIScaleformData*>(msg->data);
			GFxEvent* event = scaleformData->event;
			if (event->type == GFxEvent::KeyDown)
			{
				GFxKeyEvent* key = static_cast<GFxKeyEvent*>(event);
				if (key->keyCode == GFxKey::Code::P && !InputMappingManager::GetSingleton()->unk98)
				{
					GFxValue result;
					GFxMovieView* view = this->GetMovieView();
					if (view != nullptr)
					{
						view->Invoke("_root.Menu_mc.onExitMenuRectClick", &result, nullptr, 0);
					}
				}
			}
		}
		else if (msg->type == UIMessage::kMessage_Open)
		{
			MenuManager* mm = MenuManager::GetSingleton();
			UIStringHolder* holder = UIStringHolder::GetSingleton();
			if (mm->IsMenuOpen(holder->tweenMenu))
			{
				GFxMovieView* view = mm->GetMovieView(holder->tweenMenu);
				view->SetVisible(false);
			}
		}
		else if (msg->type == UIMessage::kMessage_Close)
		{
			MenuManager* mm = MenuManager::GetSingleton();
			UIStringHolder* holder = UIStringHolder::GetSingleton();
			if (mm->IsMenuOpen(holder->tweenMenu))
			{
				PlayerCamera* camera = PlayerCamera::GetSingleton();//kCameraState_Free
				camera->ResetCamera();
				mm->CloseMenu(holder->tweenMenu);
			}
		}
		return result;
	}

	static void InitHook()
	{
		fnProcessMessage = SafeWrite32(0x010E6594 + 0x04 * 4, &ProcessMessage_Hook);

		SafeWrite16(0x00873884, 0x9090);
		SafeWrite32(0x00873886, 0x90909090);
		SafeWrite8(0x0087388A, 0x90);
	}
};
MagicMenuEx::FnProcessMessage	MagicMenuEx::fnProcessMessage = nullptr;



class SleepWaitMenu : public IMenu
{
public:
	typedef UInt32(SleepWaitMenu::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;

	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		if (!(this->flags & IMenu::kType_PauseGame) && msg->type != UIMessage::kMessage_Refresh && msg->type != UIMessage::kMessage_Open && msg->type != UIMessage::kMessage_Close)
		{
			return this->IMenu::ProcessMessage(msg);
		}
		return (this->*fnProcessMessage)(msg);
	}

	static void UICallBack_WaitSleep(FxDelegateArgs* pargs)
	{
		class WaitSleepUpdater : public TaskDelegate
		{
		public:
			TES_FORMHEAP_REDEFINE_NEW();

			void Run() override
			{
				MenuManager* mm = MenuManager::GetSingleton();
				UIStringHolder* stringHoler = UIStringHolder::GetSingleton();
				if (mm->IsMenuOpen(stringHoler->sleepWaitMenu))
				{
					FxDelegateArgs args;
					args.responseID.SetNumber(0);

					SleepWaitMenu* sleepWaitMenu = static_cast<SleepWaitMenu*>(mm->GetMenu(stringHoler->sleepWaitMenu));
					args.pThisMenu = sleepWaitMenu;

					GFxMovieView* view = sleepWaitMenu->GetMovieView();
					args.movie = view;

					GFxValue result;
					void(__fastcall* Invoke)(GFxValue*, void*, const char*, GFxValue*, GFxValue*, UInt32) = (void(__fastcall*)(GFxValue*, void*, const char*, GFxValue*, GFxValue*, UInt32))0x00848930;
					Invoke(&sleepWaitMenu->unk20, nullptr, "getSliderValue", &result, nullptr, 0);

					args.args = &result;
					args.numArgs = 1;

					((void(__cdecl*)(FxDelegateArgs*))0x00887A10)(&args);
				}
			}
			void Dispose() override
			{
				delete this;
			}
			static void Register()
			{
				const SKSEPlugin *plugin = SKSEPlugin::GetSingleton();
				const SKSETaskInterface *task = plugin->GetInterface(SKSETaskInterface::Version_2);
				if (task)
				{
					MenuManager* mm = MenuManager::GetSingleton();
					mm->numPauseGame += 1;
					WaitSleepUpdater *delg = new WaitSleepUpdater();
					task->AddTask(delg);
				}
			}
		};

		SleepWaitMenu* sleepWaitMenu = static_cast<SleepWaitMenu*>(pargs->pThisMenu);
		bool(__cdecl* IsGameRuning)(bool mask) = (bool(__cdecl*)(bool mask))0x006F3570;
		if (!IsGameRuning(true))
		{
			((void(__cdecl*)(FxDelegateArgs*))0x00887A10)(pargs);
		}
		else if(!(sleepWaitMenu->flags & IMenu::kType_PauseGame))
		{
			sleepWaitMenu->flags |= IMenu::kType_PauseGame;
			WaitSleepUpdater::Register();
		}
	}

	static void InitHook()
	{
		fnProcessMessage = SafeWrite32(0x010E7754 + 0x04 * 4, &ProcessMessage_Hook);

		SafeWrite32(0x00887D9C, (UInt32)SleepWaitMenu::UICallBack_WaitSleep);
	}


	UInt32					unk1C;
	GFxValue				unk20;
	bool					unk30;
	bool					unk31;
	UInt16					unk32;
	UInt32					unk34;
};
static_assert(sizeof(SleepWaitMenu) == 0x38, "sizeof(SleepWaitMenu) != 0x38");
SleepWaitMenu::FnProcessMessage	SleepWaitMenu::fnProcessMessage = nullptr;


class MessageBoxMenu : public IMenu
{
public:

	static void UICallback_ButtonPress(FxDelegateArgs* pargs)
	{
#ifdef DEBUG_LOG
		_MESSAGE("type: %d    index: %.2f   num: %d", (UInt32)pargs->args->GetType(), pargs->args->GetNumber(), pargs->numArgs);
#endif
		if (pargs->args->GetType() == GFxValue::ValueType::VT_Number)
		{
			IMenu* msgMenu = static_cast<IMenu*>(pargs->pThisMenu);
			if (!(msgMenu->flags & IMenu::kType_PauseGame))
			{
				MenuManager* mm = MenuManager::GetSingleton();
				double iIndex = pargs->args->GetNumber();

				auto fn = [=]() {
					FxDelegateArgs args;
					args.responseID.SetNumber(0);
					args.pThisMenu = msgMenu;

					GFxMovieView* view = msgMenu->GetMovieView();
					args.movie = view;

					GFxValue arg;
					arg.SetNumber(iIndex);
					args.args = &arg;

					args.numArgs = 1;
#ifdef DEBUG_LOG
					_MESSAGE("iIndex: %.2f    IMenu: %p", iIndex, msgMenu);
#endif
					msgMenu->flags &= ~IMenu::kType_PauseGame;
					mm->numPauseGame -= 1;
					((void(__cdecl*)(FxDelegateArgs*))0x0087B1D0)(&args);
				};

				msgMenu->flags |= IMenu::kType_PauseGame;
				mm->numPauseGame += 1;
				CallbackDelegate::Register<CallbackDelegate::kType_Pause>(fn);
			}
			else
			{
				((void(__cdecl*)(FxDelegateArgs*))0x0087B1D0)(pargs);
			}
		}
	}

	static void InitHook()
	{
		SafeWrite32(0x0087B34C, (UInt32)UICallback_ButtonPress);
	}

};



class ContainerMenuEx : public IMenu
{
public:
	struct InventoryData
	{
		GFxMovieView*					view;				// 00
		GFxValue						categoryListRoot;	// 08
		GFxValue						ui;					// 18   array to store item info.
		BSTArray<StandardItemData*>		items;				// 28
		bool							selected;			// 34
															// 0x34 bool isUpdating? 
		DEFINE_MEMBER_FN(GetSelectedItemData, StandardItemData*, 0x00841D90);
		DEFINE_MEMBER_FN(Update, void, 0x00841E70, TESObjectREFR* owner);
		DEFINE_MEMBER_FN(UpdateUIData, void, 0x0084AFA0, void* visitor);
		DEFINE_MEMBER_FN(CreateStandardItemData, void, 0x008433B0, InventoryEntryData* objDesc, RefHandle& refHandle);
		DEFINE_MEMBER_FN(RemoveStandardItemData, void, 0x008431F0, TESForm*& removedForm);

		//__cdecl GetInventoryEntryData_Begin sub_47E480(a3, &v10); a3: TESObjectRefr v10: InventoryEntryData*
	}; 

	struct CarryWeightData //for follower,to calculate actual item count follower can burden.
	{
		float	pcCurCarryWeight;  //0
		float	pcMaxCarryWeight;  //infinite
		float	refCurCarryWeight;
		float	refMaxCarryWeight;

		DEFINE_MEMBER_FN(Create, CarryWeightData*, 0x008499D0, void*);
		DEFINE_MEMBER_FN(CalculateItemCount, bool, 0x008489A0, TESForm*, UInt32 itemCount, bool direction, UInt32& result);
	};

	struct Data54
	{
		TESForm*		form;
		void*			extraList;
		UInt32			unk08;
		DEFINE_MEMBER_FN(UpdateAmmoInfo, void, 0x008685E0, StandardItemData*);
	};

	typedef UInt32(ContainerMenuEx::*FnProcessMessage)(UIMessage*);

	static FnProcessMessage fnProcessMessage;

	UInt32 ProcessMessage_Hook(UIMessage* msg)
	{
		return (this->*fnProcessMessage)(msg);
	}

	void UpdateItemDataInfo_Hook()
	{
		for (auto form : this->unk3C)
		{
			this->inventoryData->RemoveStandardItemData(form);
		}

		TESObjectREFR* ref = nullptr;
		void(__cdecl* GetContainerOwner)(void*, TESObjectREFR*&) = (void(__cdecl*)(void*, TESObjectREFR*&))0x004A9180;
		GetContainerOwner((void*)0x01B3E764, ref);

		struct Data
		{
			TESObjectREFR*		ref;
			UInt32				lootMode;
			SInt32				detectLevel;
			bool				isNotPlayer;
		};

		Data data;
		data.ref = ref;
		data.lootMode = *(UInt32*)0x01B3E6FC;
		data.detectLevel = 0;

		if (ref != nullptr && ref->Is(FormType::Character))
		{
			Actor* actor = static_cast<Actor*>(ref);
			data.detectLevel = actor->GetDetectionLevel(g_thePlayer, 3);
		}

		InventoryChanges*(__cdecl* GetInventoryChanges)(TESObjectREFR* ref) = (InventoryChanges*(__cdecl*)(TESObjectREFR* actor))0x00476800;

		void(__cdecl* CreateItemDataFromForm)(InventoryData*, InventoryChanges*, TESForm*, Data&) = (void(__cdecl*)(InventoryData*, InventoryChanges*, TESForm*, Data&))0x0084A380;

		for (auto form : this->unk3C)
		{
			InventoryChanges* changes = nullptr;

			data.isNotPlayer = false;
			changes = GetInventoryChanges(g_thePlayer);
			CreateItemDataFromForm(this->inventoryData, changes, form, data);

			data.isNotPlayer = true;
			changes = GetInventoryChanges(ref);
			CreateItemDataFromForm(this->inventoryData, changes, form, data);
		}

		if (ref != nullptr)
		{
			ref->DecRefCount();
		}
	}


	static void UICallback_TransferItem(FxDelegateArgs* pargs)  //sub_84B270
	{
		if (pargs->pThisMenu != nullptr)
		{
			ContainerMenuEx* containerMenu = static_cast<ContainerMenuEx*>(pargs->pThisMenu);
			auto itemData = containerMenu->inventoryData->GetSelectedItemData();
#ifdef DEBUG_LOG
			for (auto & data : containerMenu->inventoryData->items)
			{
				_MESSAGE("NAME: %s   COUNT: %d   VALUE: %d", data->GetName(), data->GetCount(), data->objDesc->GetValue());
			}
			_MESSAGE("");
#endif
			InventoryEntryData* objDesc = (itemData != nullptr) ? itemData->objDesc : nullptr;
			if (objDesc != nullptr)
			{
				UInt32 itemCount = static_cast<UInt32>(pargs->args[0].GetNumber());
				bool direction = pargs->args[1].GetBool();

				CarryWeightData* pCarryWeightData = new (FormHeap_Allocate(sizeof(CarryWeightData))) CarryWeightData();
				pCarryWeightData->Create((void*)0x01B3E764);

				UInt32 actualCount = 0;
				pCarryWeightData->CalculateItemCount(objDesc->baseForm, itemCount, direction, actualCount);
				FormHeap_Free(pCarryWeightData);

				auto processTransfer = [=]() {
					UInt32& lootMode = *(UInt32*)0x01B3E6FC;
					containerMenu->unk3C.clear();

					if (containerMenu->TransferItem(objDesc, actualCount, direction))
					{
						g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
						TESObjectREFR* ref = nullptr;
						void(__cdecl* GetContainerOwner)(void*, TESObjectREFR*&) = (void(__cdecl*)(void*, TESObjectREFR*&))0x004A9180;
						GetContainerOwner((void*)0x01B3E764, ref);
						if (ref != nullptr && ref->Is(FormType::Character))
						{
							Actor* actor = DYNAMIC_CAST<Actor*>(ref);
							if (actor != nullptr && actor->processManager != nullptr)
							{
								((void(__cdecl*)(Actor*))0x00664B90)(actor);
								actor->processManager->UpdateEquipment(actor);
							}
						}
						containerMenu->inventoryData->Update(g_thePlayer);
					}
					else if (lootMode == 2)//steal
					{
						MenuManager* mm = MenuManager::GetSingleton();
						UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
						mm->CloseMenu(stringHolder->containerMenu);
					}
				};
				if (containerMenu->flags & IMenu::kType_PauseGame)
				{
					processTransfer();
				}
				else
				{
					CallbackDelegate::Register<CallbackDelegate::kType_Normal>(processTransfer);
				}
			}
		}
	}

	static void UICallback_EquipItem(FxDelegateArgs* pargs)  //sub_84ADB0
	{
		if (pargs->pThisMenu != nullptr)
		{
			ContainerMenuEx* containerMenu = static_cast<ContainerMenuEx*>(pargs->pThisMenu);
			auto itemData = containerMenu->inventoryData->GetSelectedItemData();
			InventoryEntryData* objDesc = (itemData != nullptr) ? itemData->objDesc : nullptr;
			if (objDesc != nullptr)
			{
				GFxValue* args = pargs->args;
				BGSEquipSlot* slot = nullptr;

				if (args->GetNumber() == 0.0f)
				{
					slot = (containerMenu->unk71) ? GetRightHandSlot() : GetLeftHandSlot();
				}
				else if (args->GetNumber() == 1.0f)
				{
					slot = (containerMenu->unk71) ? GetLeftHandSlot() : GetRightHandSlot();
				}

				UInt32& lootMode = *(UInt32*)0x01B3E6FC;
				UInt32 numArgs = pargs->numArgs;
				UInt32 itemCount = 0;
				if (numArgs > 1 && pargs->args[1].GetType() == GFxValue::ValueType::VT_Number)
				{
					itemCount = static_cast<UInt32>(pargs->args[1].GetNumber());
				}
				auto processer = [=]() 
				{
					containerMenu->unk3C.clear();
					if (numArgs <= 1 || !itemCount)
					{
						containerMenu->EquipItem(slot, objDesc);
						goto UpdateItemInfo;
					}
					else if (objDesc->baseForm->formType == FormType::Book || containerMenu->TransferItem(objDesc, itemCount, true))
					{
						TESForm* form = objDesc->baseForm;
						if (form->formType == FormType::Ammo)
						{
							if (!containerMenu->unk71)
							{
								Data54* pData = &containerMenu->unk54;
								pData->UpdateAmmoInfo(itemData);
							}
							goto UpdateItemInfo;
						}
						else if (form->formType == FormType::Book)
						{
							containerMenu->EquipItem(slot, objDesc);
							goto UpdateItemInfo;
						}
						else
						{
							UInt32 itemCount = itemData->GetCount();
							InventoryEntryData* pEntryData = new (FormHeap_Allocate(sizeof(InventoryEntryData))) InventoryEntryData(form, itemCount);
							containerMenu->EquipItem(slot, pEntryData);
							pEntryData->Release();
							FormHeap_Free(pEntryData);
							goto UpdateItemInfo;
						}
					}
					else if (lootMode == 2 && containerMenu->unk71)
					{
						MenuManager* mm = MenuManager::GetSingleton();
						UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
						mm->CloseMenu(stringHolder->containerMenu);
					}
					return;

				UpdateItemInfo:
					g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
					TESObjectREFR* ref = nullptr;
					void(__cdecl* GetContainerOwner)(void*, TESObjectREFR*&) = (void(__cdecl*)(void*, TESObjectREFR*&))0x004A9180;
					GetContainerOwner((void*)0x01B3E764, ref);
					if (ref != nullptr && ref->Is(FormType::Character))
					{
						Actor* actor = DYNAMIC_CAST<Actor*>(ref);
						if (actor != nullptr && actor->processManager != nullptr)
						{
							((void(__cdecl*)(Actor*))0x00664B90)(actor);
							actor->processManager->UpdateEquipment(actor);
						}
					}
					containerMenu->inventoryData->Update(g_thePlayer);
				};
				if (containerMenu->flags & IMenu::kType_PauseGame)
				{
					processer();
				}
				else
				{
					CallbackDelegate::Register<CallbackDelegate::kType_Normal>(processer);
				}
			}
		}
	}

	static void UICallback_TakeAllItems(FxDelegateArgs* pargs)  //loc_84C0F0
	{
		if (pargs->pThisMenu != nullptr)
		{
			ContainerMenuEx* containerMenu = static_cast<ContainerMenuEx*>(pargs->pThisMenu);
			auto processer = [=]() {
				containerMenu->unk3C.clear();
				containerMenu->TakeAllItems(true);
				TESObjectREFR* ref = nullptr;
				void(__cdecl* GetContainerOwner)(void*, TESObjectREFR*&) = (void(__cdecl*)(void*, TESObjectREFR*&))0x004A9180;
				GetContainerOwner((void*)0x01B3E764, ref);
				if (ref != nullptr && ref->Is(FormType::Character))
				{
					Actor* actor = DYNAMIC_CAST<Actor*>(ref);
					if (actor != nullptr && actor->processManager != nullptr)
					{
						actor->processManager->UpdateEquipment(actor);
					}
				}
			};
			if (containerMenu->flags & IMenu::kType_PauseGame)
			{
				processer();
			}
			else
			{
				CallbackDelegate::Register<CallbackDelegate::kType_Normal>(processer);
			}
		}
	}


	static void InitHook()
	{
		fnProcessMessage = SafeWrite32(0x010E4098 + 0x04 * 4, &ProcessMessage_Hook);
		SafeWrite32(0x0084C22E, (UInt32)UICallback_TransferItem);
		SafeWrite32(0x0084C271, (UInt32)UICallback_EquipItem);
		SafeWrite32(0x0084C2F7, (UInt32)UICallback_TakeAllItems);
		//fix slow speed when handle too many items in item menu,especially in container menu.
		SafeWrite8(0x0084AFCA, 0x90);
		SafeWrite32(0x0084AFCB, 0x90909090);

		//Rebuild item data
		//0084B72F                 call    sub_84A710
		WriteRelCall(0x0084B72F, GetFnAddr(&UpdateItemDataInfo_Hook));

		//EquipItem doesn't send update info.
		SafeWrite16(0x0084A0A5, 0x9090);
		SafeWrite32(0x0084A0A7, 0x90909090);
	}

	UInt32						unk1C;
	GFxValue					root;
	InventoryData*				inventoryData;
	UInt32						unk34;
	UInt32						unk38;
	BSTArray<TESForm*>			unk3C;    //exchangedItems
	BSTArray<void*>				unk48;    //container inventory data?
	Data54						unk54;    //????????????????
	UInt32						unk60;
	bool						unk64;
	UInt32						unk68;
	UInt32						unk6C;
	bool						unk70;
	bool						unk71;
	UInt32						unk74;

	DEFINE_MEMBER_FN(ctor, ContainerMenuEx*, 0x008490C0);
	DEFINE_MEMBER_FN(TransferItem, bool, 0x0084A9B0, InventoryEntryData*, UInt32 itemCount, bool direction);
	DEFINE_MEMBER_FN(EquipItem, void, 0x00849EC0, BGSEquipSlot* slot, InventoryEntryData* objDesc);
	DEFINE_MEMBER_FN(TakeAllItems, void, 0x0084B910, bool unk);
};
static_assert(sizeof(ContainerMenuEx) == 0x78, "sizeof(ContainerMenuEx) != 0x78");
static_assert(offsetof(ContainerMenuEx, unk6C) == 0x6C, "offsetof(ContainerMenuEx, unk6C) != 0x6C");
static_assert(offsetof(ContainerMenuEx, inventoryData) == 0x30, "offsetof(ContainerMenuEx, inventoryData) != 0x30");
ContainerMenuEx::FnProcessMessage	ContainerMenuEx::fnProcessMessage = nullptr;



class GFxMovieEx
{
public:

	class RemapHandler : public BSTEventSink<InputEvent*>
	{
	public:
		virtual EventResult ReceiveEvent(InputEvent ** evns, BSTEventSource<InputEvent*> * source) override
		{
			ButtonEvent * e = (ButtonEvent*)*evns;

			if (!e || e->eventType != InputEvent::kEventType_Button)
				return kEvent_Continue;

			UInt32 deviceType = e->deviceType;
			InputManager* inputManager = static_cast<InputManager*>(source);
			if ((inputManager->IsGamepadEnabled() ^ (deviceType == BSInputDevice::kType_Gamepad)) || !e->IsDown())
				return kEvent_Continue;

			UInt32 keyMask = e->keyMask;
			UInt32 keyCode;

			if (deviceType == BSInputDevice::kType_Mouse)
				keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
			else if (deviceType == BSInputDevice::kType_Gamepad)
				keyCode = InputMap::GamepadMaskToKeycode(keyMask);
			else
				keyCode = keyMask;

			if (keyCode >= InputMap::kMaxMacros)
				keyCode = -1;

			source->RemoveEventSink(this);

			auto fn = [=]() {
				GFxValue arg;
				arg.SetNumber(keyCode);
				GFxValue result;
				scope.Invoke("EndRemapMode", &result, &arg, 1);
				if (scope.IsManagedValue())
				{
					scope.ReleaseManagedValue();
				}
				if (pRemapHandler != nullptr)
				{
					GMemory::Free(pRemapHandler);
					pRemapHandler = nullptr;
				}
			};
			CallbackDelegate::Register<CallbackDelegate::kType_UI>(fn);
			return kEvent_Continue;
		}

		GFxValue scope;
	};

	static void __fastcall StartRemapMode_Hook(void* handler, void* padding, GFxFunctionHandler::Params& params)
	{
		if (!params.ArgCount)
			return;

		InputManager* inputManager = InputManager::GetSingleton();
		if (inputManager != nullptr)
		{
			if (pRemapHandler != nullptr)
			{
				GMemory::Free(pRemapHandler);
				pRemapHandler = nullptr;
			}
			pRemapHandler = new (GMemory::Alloc(sizeof(RemapHandler))) RemapHandler;
			pRemapHandler->scope = params.pArgs[0];

			inputManager->AddEventSink(GFxMovieEx::pRemapHandler);
		}
	}

	typedef void(__thiscall GFxMovieEx::*FnCreateFunction)(GFxValue* pValue, GFxFunctionHandler* pfc, void* puserData);

	static FnCreateFunction fnCreateFunction;

	static RemapHandler*	pRemapHandler;

	void CreateFunction_Hook(GFxValue* pValue, GFxFunctionHandler* pfc, void* puserData)
	{
		char* className = "class SKSEScaleform_StartRemapMode";
		static bool bSkipCheck = false;
		if (!bSkipCheck && strcmp(typeid(*pfc).name(), className) == 0)
		{
			SafeWrite32((UInt32)(*(UInt32**)pfc + 0x1), (UInt32)StartRemapMode_Hook);
			bSkipCheck = true;
		}
		(this->*fnCreateFunction)(pValue, pfc, puserData);
	}

	static void InitHook()
	{
		fnCreateFunction = SafeWrite32(0x011032C8 + 4 * 0xF, &CreateFunction_Hook);
	}
};
GFxMovieEx::FnCreateFunction GFxMovieEx::fnCreateFunction = nullptr;
GFxMovieEx::RemapHandler*	 GFxMovieEx::pRemapHandler = nullptr;




void UICallBack_DropItem(FxDelegateArgs* pargs)
{
	InventoryMenu* inventory = reinterpret_cast<InventoryMenu*>(pargs->pThisMenu);
	if (inventory != nullptr && inventory->inventoryData)
	{
		StandardItemData* itemData = inventory->inventoryData->GetSelectedItemData();
		InventoryEntryData* objDesc = nullptr;
		if (itemData)
			objDesc = itemData->objDesc;
		if (objDesc != nullptr)
		{
			void(__cdecl* sub_8997A0)(void*, char*, bool) = (void(__cdecl*)(void*, char*, bool))0x8997A0;
			if (objDesc->IsQuestItem())
			{
				sub_8997A0(*(void**)0x1B1950C, nullptr, true);  //cdecl
			}
			else
			{
				TESForm* form = objDesc->baseForm;
				if (form->formType == FormType::Key)
				{
					sub_8997A0(*(void**)0x1B1953C, nullptr, true);  //cdecl
				}
				else
				{
					GFxValue* args = pargs->args;
					UInt32 count = static_cast<UInt32>(args->GetNumber());
					BaseExtraList* (__cdecl* GetBaseExtraList)(InventoryEntryData*, UInt32, bool) = (BaseExtraList* (__cdecl*)(InventoryEntryData*, UInt32, bool))0x00868950;
					BaseExtraList* extraList = GetBaseExtraList(objDesc, count, false);
					RefHandle handle;
					g_thePlayer->DropItem(&handle, form, extraList, count, 0, 0);

					UInt32(__fastcall * sub_755420)(void*, void*) = (UInt32(__fastcall *)(void*, void*))0x00755420;
					sub_755420(*(void**)0x12E32E8, nullptr);

					inventory->inventoryData->Update(g_thePlayer);

					//PlayerCamera* camera = PlayerCamera::GetSingleton();
					if (inventory->flags & IMenu::kType_PauseGame/* || camera->cameraState == camera->cameraStates[PlayerCamera::kCameraState_Free]*/)
					{
						g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
					}
					else if (PlayerCamera::GetSingleton()->cameraState == PlayerCamera::GetSingleton()->cameraStates[PlayerCamera::kCameraState_Free])
					{
						PlayerEquipemntUpdater::Register();
					}
				}
			}
		}
	}
}



void UICallBack_CloseTweenMenu(FxDelegateArgs* pargs)
{
	PlayerCamera* camera = PlayerCamera::GetSingleton();//kCameraState_Free
	camera->ResetCamera();

	MenuManager* mm = MenuManager::GetSingleton();
	UIStringHolder* holder = UIStringHolder::GetSingleton();
	mm->CloseMenu(holder->tweenMenu);

	IMenu* inventoryMenu = reinterpret_cast<IMenu*>(pargs->pThisMenu);
	*((bool*)inventoryMenu + 0x51) = 1;
}



void UICallBack_SelectItem(FxDelegateArgs* pargs)
{
	InventoryMenu* inventory = reinterpret_cast<InventoryMenu*>(pargs->pThisMenu);
	GFxValue* args = pargs->args;

	TESForm* form = nullptr;
	BaseExtraList* extraList = nullptr;

	void(__fastcall* EquipItem)(InventoryMenu*, void*, BGSEquipSlot*) = (void(__fastcall*)(InventoryMenu*, void*, BGSEquipSlot*))0x00869DB0;
	if (inventory != nullptr && inventory->bPCControlsReady && pargs->numArgs && args->Type == GFxValue::ValueType::VT_Number)
	{
		if (args->GetNumber() == 0.0f)
		{
			BGSEquipSlot* rightHandSlot = GetRightHandSlot();
			EquipItem(inventory, nullptr, rightHandSlot);
		}
		else if (args->GetNumber() == 1.0f)
		{
			BGSEquipSlot* leftHandSlot = GetLeftHandSlot();
			EquipItem(inventory, nullptr, leftHandSlot);
		}
	}
	else
	{
		EquipItem(inventory, nullptr, nullptr);
	}

	if (inventory->flags & IMenu::kType_PauseGame/* || camera->cameraState == camera->cameraStates[PlayerCamera::kCameraState_Free]*/)
	{
		g_thePlayer->processManager->UpdateEquipment(g_thePlayer);
	}
	else if (PlayerCamera::GetSingleton()->cameraState == PlayerCamera::GetSingleton()->cameraStates[PlayerCamera::kCameraState_Free])
	{
		PlayerEquipemntUpdater::Register();
	}

	class PlayerInfoUpdater : public UIDelegate
	{
	public:
		TES_FORMHEAP_REDEFINE_NEW();

		void Run() override
		{
			UIStringHolder* stringHolder = UIStringHolder::GetSingleton();
			MenuManager* mm = MenuManager::GetSingleton();
			if (mm->IsMenuOpen(stringHolder->inventoryMenu))
			{
				InventoryMenu* inventory = static_cast<InventoryMenu*>(mm->GetMenu(stringHolder->inventoryMenu));
				inventory->UpdatePlayerInfo();
			}
		}
		void Dispose() override
		{
			delete this;
		}
		static void Register()
		{
			const SKSEPlugin *plugin = SKSEPlugin::GetSingleton();
			const SKSETaskInterface *task = plugin->GetInterface(SKSETaskInterface::Version_2);
			if (task)
			{
				PlayerInfoUpdater *delg = new PlayerInfoUpdater();
				task->AddUITask(delg);
			}
		}
	};
	auto fn = [=](UInt32 time)->bool {std::this_thread::sleep_for(std::chrono::milliseconds(time)); PlayerInfoUpdater::Register(); return true; };
	really_async(fn, 100);
}



void UICallBack_SetSaveDisabled(FxDelegateArgs* pargs)
{
	GFxValue* args = pargs->args; //һ���������� GFxVaue[5];

	bool isBool = true;
	if (pargs->numArgs <= 5 || args[5].Type != GFxValue::ValueType::VT_Boolean || !args[5].GetBool())
	{
		isBool = false;
	}
	for (UInt32 i = 0; i < 5 && args[i].GetType() != GFxValue::ValueType::VT_Undefined; ++i)
	{
		GFxValue value(args[i]);
		BGSSaveLoadManager* saveLoadManager = BGSSaveLoadManager::GetSingleton();
		GFxValue arg;
		if (i == 1 || i == 4) //Load Quit
		{
			arg.SetBoolean(false);
		}
		else
		{
			arg.SetBoolean(!saveLoadManager->CanSaveLoadGame(!isBool));
		}
		args[i].pObjectInterface->SetMember(args[i].Value.pData, "disabled", arg, value.GetType() == GFxValue::ValueType::VT_DisplayObject);
		//_MESSAGE("pGFxValue: %p    pObjectInterface: %p    pData:%p", &args[i], args[i].pObjectInterface, args[i].Value.pData);
		if ((arg.GetType() >> 6) & 1)
		{
			arg.pObjectInterface->ObjectRelease(&arg, arg.Value.pData);
		}
		if ((value.GetType() >> 6) & 1)
		{
			value.pObjectInterface->ObjectRelease(&value, value.Value.pData);
		}
	}
}



void UICallBack_ExecuteCommand(FxDelegateArgs* pargs)
{
	class ConsoleCommandUpdater : public TaskDelegate
	{
	public:
		TES_FORMHEAP_REDEFINE_NEW();
		ConsoleCommandUpdater(const char* command)
		{
			m_command = command;
		}

		void Run() override
		{
			MenuManager* mm = MenuManager::GetSingleton();
			UIStringHolder* stringHoler = UIStringHolder::GetSingleton();

			if (mm->IsMenuOpen(stringHoler->console))
			{
				FxDelegateArgs args;
				args.responseID.SetNumber(0);

				IMenu* console = mm->GetMenu(stringHoler->console);
				args.pThisMenu = console;

				GFxMovieView* view = console->GetMovieView();
				args.movie = view;

				GFxValue arg(m_command.c_str());
				args.args = &arg;

				args.numArgs = 1;

				((void(__cdecl*)(FxDelegateArgs*))0x00847080)(&args);

				if (mm->IsMenuOpen(stringHoler->console) && mm->numPauseGame && console->flags & IMenu::kType_PauseGame)
				{
					mm->numPauseGame -= 1;
					console->flags &= ~IMenu::kType_PauseGame;
				}
			}

		}
		void Dispose() override
		{
			delete this;
		}
		static void Register(const char* command)
		{
			UIStringHolder* stringHoler = UIStringHolder::GetSingleton();

			MenuManager* mm = MenuManager::GetSingleton();
			IMenu* console = mm->GetMenu(stringHoler->console);
			if (!(console->flags & IMenu::kType_PauseGame))
			{
				console->flags |= IMenu::kType_PauseGame;
				mm->numPauseGame += 1;
			}
			ConsoleCommandUpdater *delg = new ConsoleCommandUpdater(command);
			PauseTaskInterface::AddTask(delg);
		}
	private:
		std::string					m_command;
	};

	if (pargs->args->GetType() == GFxValue::ValueType::VT_String)
	{
		IMenu* console = static_cast<IMenu*>(pargs->pThisMenu);
		if (!(console->flags & IMenu::kType_PauseGame))
		{
			std::regex tg("^\\s*(ToggleGrass|TG)\\s*$", std::regex::icase);
			std::regex tb("^\\s*(ToggleBorders|TB)\\s*$", std::regex::icase);
			std::regex save("^\\s*(SaveGame|save)\\s+\\w+(\\s+[+-]?\\d+)?\\s*$", std::regex::icase);
			std::regex csb("^\\s*(ClearScreenBlood|csb)\\s*$", std::regex::icase);
			std::regex coe("^\\s*(CenterOnExterior|COE)(\\s+[+-]?\\d+){2}\\s*$", std::regex::icase);
			std::regex cow("^\\s*(CenterOnWorld|COW)\\s+\\w+(\\s+[+-]?\\d+){2}\\s*$", std::regex::icase);
			std::regex coc("^\\s*(CenterOnCell|COC)\\s+\\w+\\s*$", std::regex::icase);
			std::regex jail("^\\s*(ServeTime)\\s*$", std::regex::icase);

			std::string command(pargs->args->GetString());
			if (std::regex_match(command, tg) || std::regex_match(command, tb) || std::regex_match(command, save) || std::regex_match(command, csb) || std::regex_match(command, coe) || std::regex_match(command, cow) || std::regex_match(command, jail))
			{
				ConsoleCommandUpdater::Register(command.c_str());
			}
			else if (std::regex_match(command, coc))  //slow speed,so create a new thread to process it.
			{
				std::vector<std::string> sections;
				std::regex pattern("\\b\\w+\\b", std::regex::icase);
				for (std::sregex_iterator it(command.begin(), command.end(), pattern), end_it; it != end_it; ++it)
				{
					sections.push_back(it->str());
				}
				std::string destination = sections[1];

				auto fn = [=]()->bool {
					void* (*GetInteriorCell)(const char*) = (void* (*)(const char*))0x00451B60;
					void* cell = GetInteriorCell(destination.c_str());
					if (!cell)
					{
						SInt16 x = 0;
						UInt16 y = 0;
						TESDataHandler* pDataHandler = TESDataHandler::GetSingleton();//The next codes are too slow...
						void* worldSpace = pDataHandler->GetSpaceData(destination.c_str(), x, y);
						if (worldSpace != nullptr)
						{
							void* (__fastcall* GetExteriorCell)(void*, void*, SInt16, UInt16) = (void* (__fastcall*)(void*, void*, SInt16, UInt16))0x004F5330;
							cell = GetExteriorCell(worldSpace, nullptr, x, y);
						}
					}
					if (cell != nullptr)
					{
						ConsoleCommandUpdater::Register(command.c_str());
					}
					return true;
				};
				really_async(fn);
			}
			else
			{
				((void(__cdecl*)(FxDelegateArgs*))0x00847080)(pargs);
			}
		}
		else
		{
			((void(__cdecl*)(FxDelegateArgs*))0x00847080)(pargs);
		}
	}
}




void RegisterEventHandler()
{
	MenuManager* mm = MenuManager::GetSingleton();
	if (mm)
	{
		BSTEventSource<MenuOpenCloseEvent>* eventDispatcher = mm->GetMenuOpenCloseEventSource();
		eventDispatcher->AddEventSink(MenuOpenCloseEventHandler::GetSingleton());
	}
}


void Hook_Game_Commit()
{

	FavoritesMenu::InitHook();
	BookMenu::InitHook();
	LockpickingMenu::InitHook();
	InventoryMenuEx::InitHook();

	DialogueMenuEx::InitHook();
	MagicMenuEx::InitHook();
	SleepWaitMenu::InitHook();
	MessageBoxMenu::InitHook();

	ContainerMenuEx::InitHook();

	GFxMovieEx::InitHook();
	//Fix Inventory.
	SafeWrite16(0x0086BF6F, 0x9090);
	SafeWrite32(0x0086BF71, 0x90909090);

	//Fix Container
	SafeWrite16(0x0084C068, 0x9090);
	SafeWrite32(0x0084C06A, 0x90909090);

	SafeWrite16(0x0084AC57, 0x9090);
	SafeWrite32(0x0084AC59, 0x90909090);

	//Drop item in Inventory menu.
	SafeWrite32(0x0086B437, (UInt32)UICallBack_DropItem);
	//Equip item in Inventory Menu
	SafeWrite32(0x0086B3F4, (UInt32)UICallBack_SelectItem);
	//SetSaveDisabled in Journal Menu
	SafeWrite32(0x008A7F73, (UInt32)UICallBack_SetSaveDisabled);

	//UICallBack_ExecuteCommand //0084732B
	SafeWrite32(0x0084732C, (UInt32)UICallBack_ExecuteCommand);

	//Fix camera in Tweenmenu.
	SafeWrite8(0x008951C4, 0x90);
	SafeWrite32(0x008951C5, 0x90909090);

	//Redefine papyrus function: Utility.IsInMenuMode()
	WriteRelJump(0x00918D90, (UInt32)Hook_IsInMenuMode);

	{
		static const UInt32 kHook_GetFavoritesSpell_Jmp = 0x85BA03;

		START_ASM(Hook_GetFavoritesSpell, 0x0085B919, 0x0085B921, 1);
			push eax
			call Hook_GetFavoritesItem
			test eax, eax
			je InvalidSpell
			JMP_ASM(Hook_GetFavoritesSpell)
			InvalidSpell:
			jmp [kHook_GetFavoritesSpell_Jmp]
		END_ASM(Hook_GetFavoritesSpell);
	}

	{
		static const UInt32 kHook_GetFavoritesItem_Jmp = 0x85BB34;

		START_ASM(Hook_GetFavoritesItem, 0x0085BA52, 0x0085BA5A, 1);
			push eax
			call Hook_GetFavoritesItem
			test eax, eax
			je InvalidItem
			JMP_ASM(Hook_GetFavoritesItem)
			InvalidItem:
			jmp [kHook_GetFavoritesItem_Jmp]
		END_ASM(Hook_GetFavoritesItem);
	}

	{
		START_ASM(Hook_KillActor, 0x006AC3A0, 0x006AC3A9, 1);
			push ecx
			push ecx
			call Hook_KillActor
			pop ecx
			push esi
			mov esi, ecx
			mov ecx, [esi + 0x88]
		END_ASM(Hook_KillActor);
	}

	{
		START_ASM(Hook_AddUIMessage, 0x00431B00, 0x00431B0A, 1);
			push ecx
			mov eax, [esp + 0xC]
			push eax
			mov eax, [esp + 0xC]
			push eax
			call Hook_AddUIMessage
			pop ecx
			test al, al
			jnz Continue
			retn 0xC
			Continue:
			push ecx
			push esi
			mov esi, ecx
			mov eax, [esi + 0x1C8]
		END_ASM(Hook_AddUIMessage);
	}

	{
		START_ASM(Hook_RequstAutoSave, 0x00681770, 0x00681776, 1);
			push ecx
			mov eax, MenuOpenCloseEventHandler::unpausedCount
			cmp eax, 0
			jz	ContinueSave
			pop ecx
			retn
			ContinueSave:
			pop ecx
			sub esp, 0x104
		END_ASM(Hook_RequstAutoSave)
	}

	{
		static const UInt32 kHook_DisableTimeUpdate_Jmp = 0x008D41F8;

		START_ASM(Hook_VMUpdateTime, 0x008D41E9, 0x008D41F2, 1);		
			push eax
			mov eax, 0x01B2E85E
			movzx eax, byte ptr [eax]
			cmp eax, 0
			pop eax
			jnz DisableUpdate
			cmp MenuOpenCloseEventHandler::unpausedCount, 0
			jnz DisableUpdate
			JMP_ASM(Hook_VMUpdateTime)
			DisableUpdate:
			jmp [kHook_DisableTimeUpdate_Jmp]	
		END_ASM(Hook_VMUpdateTime)
	}
}
