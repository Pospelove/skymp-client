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
	static bool started = false;
	if (!started)
	{
		started = true;
		std::thread([] {

			using Range = std::pair<uint32_t, uint32_t>;
			static std::map<FormType, Range> ranges = {
				{ FormType::EffectSetting,{ 0x0000014A, 0x0010FF0C } }
			};

			static auto prepareSS = [](std::stringstream &ss) {
				ss << std::hex << std::showbase << std::setfill('0') << std::setw(8);
			};

			static auto getIdentifier = [](TESForm *form) {
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
				return iden;
			};

			std::stringstream ss;
			prepareSS(ss);
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

					static auto quote = "\"";

					ss << "{ ";
					ss << quote << iden << quote << ", ";
					ss << quote << archetype << quote << ", ";
					ss << formID << ", ";
					ss << quote << castingType << quote << ", ";
					ss << quote << deliveryType << quote << ", ";
					ss << quote << actorValues[0] << quote << ", ";
					ss << quote << actorValues[1] << quote;
					ss << " },";
				}
			}

			ss << "}" << std::endl;

			std::ofstream file("AAA_DATASEARCH_EFFECTS.lua");
			file << ss.str();
			file.close();

		}).detach();
	}
}