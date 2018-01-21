#include "../stdafx.h"
#include "CoreInterface.h"

#define CI_IMPL
#include "DataSearchUtil.h"

#include  <iomanip>

class CIAccess
{
public:
	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};

auto getLocation = [](TESObjectREFR *ref) {
	auto interior = sd::GetParentCell(ref);
	if (interior && interior->IsInterior() == false)
		interior = nullptr;
	if (interior != nullptr)
		return interior->formID;
	else
		return ref->GetWorldSpace()->formID;
};

namespace ci
{
	using KeywordBackup = std::map<BGSConstructibleObject *, BGSKeyword *>;
	extern std::shared_ptr<KeywordBackup> keywordBackup;

	namespace DataSearch
	{
		namespace Private
		{
			ci::Signal<void(TeleportDoorsData)> tpdCallbacks;

			class HitEventSinkGlobal : public BSTEventSink<TESHitEvent>
			{
			public:
				HitEventSinkGlobal() {
					g_hitEventSource.AddEventSink(this);
				}

				virtual ~HitEventSinkGlobal() override {
					g_hitEventSource.RemoveEventSink(this);
				}

				std::unique_ptr<ci::DataSearch::TeleportDoorsData::TeleportDoor> tpd1 = nullptr;

				virtual	EventResult	ReceiveEvent(TESHitEvent *evn, BSTEventSource<TESHitEvent> * source) override
				{
					if(false == (bool)tpdCallbacks)
						return EventResult::kEvent_Continue;

					if (evn->caster != g_thePlayer || evn->target == nullptr)
						return EventResult::kEvent_Continue;

					if (evn->target->baseForm->formType != FormType::Door)
						return EventResult::kEvent_Continue;

					ci::DataSearch::TeleportDoorsData::TeleportDoor tpd;
					tpd.baseID = evn->target->baseForm->formID;
					tpd.refID = evn->target->formID;
					tpd.pos = cd::GetPosition(evn->target);
					tpd.rot = cd::GetAngle(evn->target);
					tpd.locationID = getLocation(evn->target);

					auto wc = WorldCleaner::GetSingleton();
					wc->SetFormProtected(tpd.refID, true);
					const auto refID = tpd.refID;
					auto f = [refID] {
						auto doorRef = (TESObjectREFR *)LookupFormByID(refID);
						if (doorRef)
						{
							sd::SetDestroyed(doorRef, false);
							sd::BlockActivation(doorRef, false);
							sd::SetOpen(doorRef, false);
							sd::Lock(doorRef, false, false);
						}
					};
					SET_TIMER_LIGHT(0, f);

					if (!tpd1)
					{
						tpd1.reset(new ci::DataSearch::TeleportDoorsData::TeleportDoor);
						*tpd1 = tpd;

						ci::Chat::AddMessage(L"#BEBEBE" L"Selected door 1");

					}
					else
					{
						if (tpd1->refID != tpd.refID)
						{
							ci::Chat::AddMessage(L"#BEBEBE" L"Selected door 2");

							{
								std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
								ci::DataSearch::TeleportDoorsData result;
								result.doors[0] = *tpd1;
								result.doors[1] = tpd;
								tpdCallbacks(result);
							}
							tpd1.reset(nullptr);
						}
						else
						{
							ci::Chat::AddMessage(L"#BEBEBE" L"Undo select");
							tpd1.reset(nullptr);
						}
					}

					return EventResult::kEvent_Continue;
				}
			};

			void Update()
			{
				//SET_TIMER_LIGHT(1, Update);
			}

			void StartUpdating()
			{
				static bool started = false;
				if (!started)
				{
					//SET_TIMER_LIGHT(1, Update);
					started = true;
					new HitEventSinkGlobal;
				}
			}
		}
	}
}

void ci::DataSearch::RequestNavMesh(std::function<void(NavMeshData)> callback)
{
	Private::StartUpdating();

	SET_TIMER_LIGHT(15000, [=] {
		RequestNavMesh(callback);
	});

	std::thread([=] {
		for (auto i = 0ui32; i != (2 * 0x0010DFF9) + 1; ++i)
		{
			auto form = (NavMesh *)LookupFormByID(i);
			if (form)
			{
				if (form->formType == FormType::NAVM)
				{
					NavMeshData res;

					for (size_t i = 0; i != form->triangles.size(); ++i)
					{
						NavMeshData::Triangle tr;
						for (int32_t ver = 0; ver != 3; ++ver)
							tr.verticeIDs[ver] = form->triangles[i].vertices[ver];
						res.triangles.push_back(tr);
					}

					for (size_t i = 0; i != form->vertices.size(); ++i)
						res.vertices.push_back(form->vertices[i]);

					for (size_t i = 0; i != form->externalConnections.size(); ++i)
						res.externalConnections.push_back(form->externalConnections[i].navmeshID);

					res.formID = form->formID;

					{
						std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
						callback(res);
					}
					Sleep(50);
				}
			}
		}
	}).detach();
}

void ci::DataSearch::RequestTeleportDoorsManual(std::function<void(TeleportDoorsData)> callback)
{
	Private::StartUpdating();
	Private::tpdCallbacks.Add(callback);
}

void ci::DataSearch::RequestContainers(std::function<void(ContainerData)> callback)
{
	Private::StartUpdating();
	WorldCleaner::GetSingleton()->SetCallback(FormType::Container, [=](TESObjectREFR *ref) {
		ContainerData c;
		c.baseID = ref->baseForm->formID;
		c.locationID = getLocation(ref);
		c.pos = cd::GetPosition(ref);
		c.rot = { sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };
		c.refID = ref->GetFormID();
		callback(c);
	});
}

void ci::DataSearch::RequestDoors(std::function<void(DoorData)> callback)
{
	Private::StartUpdating();
	WorldCleaner::GetSingleton()->SetCallback(FormType::Door, [=](TESObjectREFR *ref) {
		DoorData c;
		c.baseID = ref->baseForm->formID;
		c.locationID = getLocation(ref);
		c.pos = cd::GetPosition(ref);
		c.rot = { sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };
		c.refID = ref->GetFormID();
		callback(c);
	});
}

void ci::DataSearch::RequestItems(std::function<void(ItemData)> callback)
{
	Private::StartUpdating();

	WorldCleaner::GetSingleton()->SetCallback(FormType::Misc, [=](TESObjectREFR *ref) {
		if (ref->formID > 0xFF000000)
			return;
		if (ref->baseForm->IsLockpick())
			return;
		if (ref->baseForm->IsGold())
			return;
		ItemData c;
		c.baseID = ref->baseForm->formID;
		c.locationID = getLocation(ref);
		c.pos = cd::GetPosition(ref);
		c.rot = { sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };
		c.refID = ref->GetFormID();
		c.value = ref->baseForm->GetGoldValue();
		c.damage = 0;
		c.cl = ci::ItemType::Class::Misc;
		c.subCl = ci::ItemType::Subclass::MISC_Misc;
		callback(c);
	});
}

void ci::DataSearch::RequestActors(std::function<void(ActorData)> callback)
{
	Private::StartUpdating();

	WorldCleaner::GetSingleton()->SetCallback(FormType::NPC, [=](TESObjectREFR *ref) {
		if (ref->IsActivationBlocked() && ref->formID > 0xFF000000)
			return;
		if (sd::IsDisabled(ref) == true)
			return;
		auto npc = (TESNPC *)ref->baseForm;
		if (npc->TESActorBaseData::flags.unique == true)
			return;
		ActorData c;
		c.baseID = ref->baseForm->formID;
		c.locationID = getLocation(ref);
		c.pos = cd::GetPosition(ref);
		c.rot = { sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };
		c.refID = ref->GetFormID();
		c.race = ((Actor *)ref)->GetRace()->formID;
		callback(c);
	});
}

void ci::DataSearch::LuaCodegenStart()
{
	// –аньше это было в логике клиента в OnWorldInit() исключительно дл€ теста конструктора и деструктора ci::Recipe
	// —ейчас же создание рецепта до запуска DataSearch гарантирует, что Keywords рецептов будут обработаны нормально + не будет состо€ни€ гонки (переменна€ keywordsBackup)
	// 'VI', потому что иначе будет ошибка невалидного Keyword-а, а при использовании 'VI' ошибки будут проигнорированы и не попадут в логи
	delete new ci::Recipe("VI", nullptr, 0);

	static bool started = false;
	if (!started)
	{
		started = true;
		std::thread([] {

			using Range = std::pair<uint32_t, uint32_t>;

			static std::map<FormType, Range> ranges = {
				{ FormType::EffectSetting,{ 0x0000014A, 0x0010FF0C } },
				{ FormType::Spell,{ 0x00000E52, 0x0010FF0B } },
				{ FormType::Enchantment,{ 0x000170F2, 0x0010FCF6 } },
				{ FormType::Ammo, {0x0001397D, 0x0010EC8C}},
				{ FormType::Armor, {0x00000D64, 0x0010FC28}},
				{ FormType::Book, {0x00015475, 0x0010FD60}},
				{ FormType::Ingredient, {0x000134AA, 0x00106E1C}},
				{ FormType::Key, {0x0006564E, 0x0010E7E6}},
				{ FormType::Misc, {0x0000000A, 0x0010E44B}},
				{ FormType::SoulGem, {0x0002E422, 0x00094E40} },
				{ FormType::Weapon, {0x000001F4, 0x0010FD5E}},
				{ FormType::Potion, {0x0001895F, 0x0010D666}},
				{ FormType::ConstructibleObject, {0x00000DD2, 0x0010FDF6}}
			};

			static auto quote = "\"";

			static auto prepareSS = [](std::stringstream &ss) {
				ss << std::hex << std::setfill('0') << std::setw(8);
			};

			static auto getIdentifier = [](TESForm *form) {
				if (form == nullptr)
					return std::string{};
				std::string iden = form->GetName();
				if (iden.empty())
				{
					auto tesFullName = DYNAMIC_CAST<TESFullName *, TESForm *>(form);
					if (tesFullName != nullptr)
						iden = tesFullName->GetFullName();
				}
				if (iden.empty())
				{
					auto refr = DYNAMIC_CAST<TESObjectREFR *, TESForm *>(form);
					if (refr != nullptr)
					{
						auto xTextData = refr->extraData.GetByType<ExtraTextDisplayData>();
						if (xTextData != nullptr)
							iden = xTextData->name.c_str();
					}
				}
				if (iden.empty())
				{
					std::stringstream idss;
					prepareSS(idss);
					idss << form->GetFormID();
					iden = idss.str();
				}
				for (size_t i = 0; i != iden.size(); ++i)
				{
					if (iden[i] == *"\"")
						iden[i] = *"'";
				}
				return iden;
			};

			static auto getWeight = [](TESForm *form) {
				auto weightForm = DYNAMIC_CAST<TESWeightForm *, TESForm *>(form);
				return weightForm != nullptr ? weightForm->weight : 0.0f;
			};

			static auto getGoldValue = [](TESForm *form) {
				auto valueForm = DYNAMIC_CAST<TESValueForm *, TESForm *>(form);
				return valueForm != nullptr ? valueForm->value : 0;
			};

			static auto getDamageArmorPoints = [](TESForm *form) {
				float res = 0.0f;
				switch (form->formType)
				{
				case FormType::Weapon:
				{
					auto weap = (TESObjectWEAP *)form;
					res = weap->attackDamage;
					break;
				}
				case FormType::Armor:
				{
					auto armor = (TESObjectARMO *)form;
					res = armor->armorValTimes100 / 100;
					break;
				}
				default:
					break;
				}
				return res;
			};

			static auto getSkill = [](TESForm *form) {
				std::string res;
				switch (form->formType)
				{
				case FormType::Weapon:
				{
					auto weap = (TESObjectWEAP *)form;
					switch (weap->gameData.type)
					{
					case TESObjectWEAP::GameData::kType_TwoHandSword:
					case TESObjectWEAP::GameData::kType_TwoHandAxe:
						res = "TwoHanded";
						break;
					case TESObjectWEAP::GameData::kType_Bow:
					case TESObjectWEAP::GameData::kType_CrossBow:
					case TESObjectWEAP::GameData::kType_Bow2:
					case TESObjectWEAP::GameData::kType_CBow:
						res = "Marksman";
						break;
					case TESObjectWEAP::GameData::kType_Staff:
					case TESObjectWEAP::GameData::kType_Staff2:
						res = ""; // Not implemented in 0.15
						break;
					default:
						res = "OneHanded";
						break;
					}
					break;
				}
				case FormType::Armor:
				{
					auto armor = (TESObjectARMO *)form;
					res = armor->IsLightArmor() ? "LightArmor" : "HeavyArmor";
					break;
				}
				default:
					break;
				}
				return res;
			};
			
			static auto getArmorSubcl = [](TESForm *form)->std::string {
				auto armor = (TESObjectARMO *)form;
				if (armor != nullptr && armor->formType == FormType::Armor)
				{
					const auto slotMask = armor->GetSlotMask();
					if (slotMask & TESObjectARMO::PartFlag::kPart_Body)
						return "Armor";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Amulet)
						return "Amulet";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Ring)
						return "Ring";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Shield)
						return "Shield";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Hands || slotMask & TESObjectARMO::PartFlag::kPart_Forearms)
						return "Gauntlets";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Head)
						return "Helmet";
					if (slotMask & TESObjectARMO::PartFlag::kPart_Feet)
						return "Boots";
				}
				return "";
			};

			static auto getWeapSubcl = [](TESForm *form)->std::string {
				auto weap = (TESObjectWEAP *)form;
				if (weap != nullptr && weap->formType == FormType::Weapon)
				{
					enum { kType_TwoHandWarhammer = -1 }; // Where is Warhammer ?!

					switch (weap->gameData.type)
					{
					case TESObjectWEAP::GameData::kType_Staff:
					case TESObjectWEAP::GameData::kType_Staff2:
						return "Staff"; // Not implemented in 0.15
					case TESObjectWEAP::GameData::kType_Bow:
					case TESObjectWEAP::GameData::kType_Bow2:
						return "Bow";
					case TESObjectWEAP::GameData::kType_CrossBow:
					case TESObjectWEAP::GameData::kType_CBow:
						return "CrossBow"; // Not implemented in 0.15 (and will not be added before 2.0 release :P)
					case TESObjectWEAP::GameData::kType_OneHandMace:
						return "Mace";
					case TESObjectWEAP::GameData::kType_OneHandSword:
						return "Sword";
					case TESObjectWEAP::GameData::kType_OneHandAxe:
						return "WarAxe";
					case TESObjectWEAP::GameData::kType_OneHandDagger:
						return "Dagger";
					case TESObjectWEAP::GameData::kType_TwoHandSword:
						return "Greatsword";
					case TESObjectWEAP::GameData::kType_TwoHandAxe:
						return "Battleaxe";
					case kType_TwoHandWarhammer: // ???
						return "Warhammer";
					default:
						break;
					}
				}
				return "";
			};

			static std::map<uint32_t, std::string> formIdents;

			static auto getEnchantmentIden = [](TESForm *form)->std::string {
				auto enchForm = DYNAMIC_CAST<TESEnchantableForm *, TESForm *>(form);
				if (enchForm != nullptr && enchForm->enchantment != nullptr)
				{
					return formIdents[enchForm->enchantment->GetFormID()];
				}
				return "";
			};

			struct EffectItem
			{
				std::string mgefIden;
				float mag, dur, area;
			};
			static auto getEffectItemList = [](TESForm *form) {
				std::list<EffectItem> effectItemList;
				auto sp = DYNAMIC_CAST<MagicItem *, TESForm *>(form);
				if (sp != nullptr)
				{
					for (auto &efItemRaw : sp->effectItemList)
					{
						effectItemList.push_back({
							formIdents[efItemRaw->mgef->GetFormID()],
							efItemRaw->magnitude,
							efItemRaw->duration,
							efItemRaw->area
						});
					}
				}
				return effectItemList;
			};

			std::stringstream ss;
			prepareSS(ss);

			// Effects:

			ss << "local effects = {" << std::endl;

			ss.clear();
			const auto &effectsRange = ranges[FormType::EffectSetting];
			for (uint32_t id = effectsRange.first; id <= effectsRange.second; ++id)
			{
				auto mgef = (EffectSetting *)LookupFormByID(id);
				if (mgef != nullptr && mgef->formType == FormType::EffectSetting)
				{
					const std::string iden = getIdentifier(mgef);
					const std::string archetype = ::EffectArchetype(mgef->properties.archetype).GetName();
					const uint32_t formID = id;

					std::string castingType;
					enum CastingType
					{
						ConstantEffect = 0,
						FireAndForget = 1,
						Concentration = 2,
					};
					switch (mgef->properties.castType)
					{
					case ConstantEffect:
						castingType = "ConstantEffect";
						break;
					case FireAndForget:
						castingType = "FireAndForget";
						break;
					case Concentration:
						castingType = "Concentration";
						break;
					}

					std::string deliveryType;
					enum Delivery
					{
						Self = 0,
						Contact = 1,
						Aimed = 2,
						TargetActor = 3,
						TargetLocation = 4,
					};
					switch (mgef->properties.deliveryType)
					{
					case Self:
						deliveryType = "Self";
						break;
					case Contact:
						deliveryType = "Contact";
						break;
					case Aimed:
						deliveryType = "Aimed";
						break;
					case TargetActor:
						deliveryType = "TargetActor";
						break; 
					case TargetLocation:
						deliveryType = "TargetLocation";
						break;
					}

					const std::string actorValues[2] = {
						::GetEffectAVName(mgef->properties.primaryValue),
						::GetEffectAVName(mgef->properties.secondaryValue)
					};

					ss << "{ ";
					ss << quote << iden << quote << ", ";
					ss << quote << archetype << quote << ", ";
					ss << "0x" << formID << ", ";
					ss << quote << castingType << quote << ", ";
					ss << quote << deliveryType << quote << ", ";
					ss << quote << actorValues[0] << quote << ", ";
					ss << quote << actorValues[1] << quote;
					ss << " }," << std::endl;

					formIdents[formID] = iden;
				}
			}
			ss << "nil }" << std::endl;

			// Magic:

			ss << std::endl;
			ss << "local magic = {" << std::endl;
			const auto &spellsRange = ranges[FormType::Spell];
			const auto &enchsRange = ranges[FormType::Enchantment];
			for (uint32_t id = min(spellsRange.first, enchsRange.first); id <= max(spellsRange.second, enchsRange.second); ++id)
			{
				auto sp = (MagicItem *)LookupFormByID(id);
				if (sp != nullptr && (sp->formType == FormType::Spell || sp->formType == FormType::Enchantment))
				{
					const auto magicType = sp->formType == FormType::Enchantment ? "Enchantment" : "Spell";
					const std::string iden = getIdentifier(sp);
					const uint32_t formID = sp->GetFormID();
					const float cost = sp->formType == FormType::Spell ? ((SpellItem *)sp)->GetMagickaCost() : 0.0f;
					const auto effectItemList = getEffectItemList(sp);

					ss << "{ ";
					ss << quote << magicType << quote << ", ";
					ss << quote << iden << quote << ", ";
					ss << "0x" << formID << ", ";
					ss << cost;
					for (const auto &effectItem : effectItemList)
						ss << ", " << "{ " << quote << effectItem.mgefIden << quote << ", " << effectItem.mag << ", " << effectItem.dur << ", " << effectItem.area << "} ";
					ss << " }," << std::endl;
				}
			}
			ss << "nil }" << std::endl;

			// Items:

			ss << std::endl;
			ss << "local itemTypes = {" << std::endl;

			const std::set<FormType> itemFormTypes = {
				FormType::Ammo, FormType::Armor, FormType::Book, FormType::Ingredient, FormType::Key, FormType::Misc, FormType::SoulGem, FormType::Weapon, FormType::Potion
			};

			Range itemsRange = { 0xFF000000, 0x00000000 };
			for (auto formType : itemFormTypes)
			{
				const auto &range = ranges[formType];
				if (itemsRange.first > range.first)
					itemsRange.first = range.first;
				if (itemsRange.second < range.second)
					itemsRange.second = range.second;
			}
			for (auto id = itemsRange.first; id <= itemsRange.second; ++id)
			{
				auto form = LookupFormByID(id);
				if (form == nullptr)
					continue;

				uint8_t soulSize = 0, gemSize = 0;
				std::string cl, subCl;

				switch (form->formType)
				{
				case FormType::Ammo:
					cl = "Ammo";
					subCl = "";
					break;
				case FormType::Armor:
					cl = "Armor";
					subCl = getArmorSubcl(form);
					break;
				case FormType::Book:
					cl = "Book"; // not implemented in 0.15
					subCl = "";
					break;
				case FormType::Ingredient:
					cl = "Ingredient";
					subCl = "";
					break;
				case FormType::Key:
					cl = "Key";
					subCl = "";
					break;
				case FormType::Misc:
					cl = "Misc";
					if (form->IsGold())
						subCl = "Gold";
					else if (form->IsLockpick())
						subCl = "Lockpick";
					else
						subCl = "Misc";
					break;
				case FormType::SoulGem:
					soulSize = ((TESSoulGem *)form)->soulSize;
					gemSize = ((TESSoulGem *)form)->gemSize;
					cl = "SoulGem";
					subCl = "";
					break;
				case FormType::Weapon:
					cl = "Weapon";
					subCl = getWeapSubcl(form);
					break;
				case FormType::Potion:
					cl = "Potion";
					if (((AlchemyItem *)form)->IsPoison())
						subCl = "Poison";
					else if (((AlchemyItem *)form)->IsFood())
						subCl = "Food";
					else
						subCl = cl;
					break;
				default:
					continue;
				}

				const std::string iden = getIdentifier(form);
				const uint32_t formID = form->GetFormID();
				const float weight = getWeight(form);
				const uint32_t goldValue = getGoldValue(form);
				const float damageArmorPoints = getDamageArmorPoints(form);
				const std::string skill = getSkill(form);
				const std::string enchIden = getEnchantmentIden(form);
				const std::list<EffectItem> effectItemList = getEffectItemList(form);

				const auto clSubclResultStr = subCl.empty() ? (cl) : (cl + '.' + subCl);

				ss << "{ ";
				ss << quote << iden << quote << ", ";
				ss << quote << clSubclResultStr << quote << ", ";
				ss << "0x" << formID << ", ";
				ss << weight << ", ";
				ss << std::to_string(goldValue) << ", ";
				ss << damageArmorPoints << ", ";
				ss << quote << skill << quote << ", ";
				ss << quote << enchIden << quote << ", ";
				ss << std::to_string(soulSize) << ", ";
				ss << std::to_string(gemSize);
				for (const auto &effectItem : effectItemList)
					ss << ", " << "{ " << quote << effectItem.mgefIden << quote << ", " << effectItem.mag << ", " << effectItem.dur << ", " << effectItem.area << "} ";
				ss << " }," << std::endl;
			}
			ss << "nil }" << std::endl;

			// Recipes:

			ss << std::endl;
			ss << "local recipes = {" << std::endl;
			const auto &recipesRange = ranges[FormType::ConstructibleObject];
			for (auto id = recipesRange.first; id <= recipesRange.second; ++id)
			{
				auto form = LookupFormByID(id);
				if (form == nullptr)
					continue;
				auto recipe = (BGSConstructibleObject *)form;
				if (recipe->formType != FormType::ConstructibleObject)
					continue;

				const auto keyword =
					((keywordBackup != nullptr && (*keywordBackup)[recipe] != nullptr )? (*keywordBackup)[recipe]->keyword : recipe->wbKeyword->keyword).operator const char *();
				const auto createdObjectIden = getIdentifier(recipe->createdObject);
				const auto numCreatedObjects = recipe->quantity;

				ss << "{ ";
				ss << quote << keyword << quote << ", ";
				ss << quote << createdObjectIden << quote;
				ss << quote << numCreatedObjects << ", ";

				ss << ", " << "{ ";
				bool isBegin = true;
				for (auto it = recipe->container.entries; it != recipe->container.entries + recipe->container.numEntries; ++it)
				{
					auto entry = *it;
					if (!isBegin)
						ss << ", ";
					ss << "[" << quote << getIdentifier(entry->form) << quote << "]" << " = " << std::to_string(entry->count);
					isBegin = false;
				}
				ss << " }";

				ss << " }," << std::endl;
			}
			ss << "nil }" << std::endl;

			ss << "local dsres = {}" << std::endl;
			ss << "dsres.effects = effects" << std::endl;
			ss << "dsres.magic = magic" << std::endl;
			ss << "dsres.itemTypes = itemTypes" << std::endl;
			ss << "dsres.recipes = recipes" << std::endl;
			ss << "return dsres" << std::endl;

			{
				std::ofstream file("AAA_DATASEARCH_LOCAL_RESULTS.lua");
				file << ss.str();
				file.close();
			}

		}).detach();
	}
}