#include "../stdafx.h"
#include "Werewolf.h"

#include "AnimData.h" // for AnimData_::Register();

namespace Werewolf
{
	void SetWerewolf(uint32_t refID, bool ww, bool skipAnim)
	{
		static dlf_mutex wwMutex;
		std::lock_guard<dlf_mutex> l(wwMutex);

		enum class Stage : int8_t
		{
			None = 0,
			StartingWerewolf = 1,
			Werewolf = 2,
		};

		static std::map<uint32_t, Stage> wwStage;
		static std::set<uint32_t> wwPositive;

		if (ww)
			wwPositive.insert(refID);
		else
			wwPositive.erase(refID);

		if ((wwStage[refID] == Stage::StartingWerewolf))
		{
			if (!ww)
			{
				SET_TIMER_LIGHT(250, [=] {
					size_t count = 0;
					{
						std::lock_guard<dlf_mutex> l(wwMutex);
						count = wwPositive.count(refID);
					}
					if (count == 0)
					{
						SetWerewolf(refID, false, skipAnim);
					}
				});
			}
			return;
		}

		if ((wwStage[refID] == Stage::None && ww) || (wwStage[refID] == Stage::Werewolf && !ww))
		{
			SET_TIMER_LIGHT(0,
				[=] {
				static auto wwSkin = LookupFormByID(0x000F6002);
				static auto wwRace = (TESRace *)LookupFormByID(0x00CDD84);

				auto ac = (Actor *)LookupFormByID(refID);
				if (ac != nullptr)
				{
					static std::map<void *, TESRace *> lastRace;
					if (ww)
					{
						wwStage[refID] = Stage::StartingWerewolf;
						sd::UnequipAll(ac);
						SET_TIMER(skipAnim ? 1 : 4500, [=] {
							auto ac = (Actor *)LookupFormByID(refID);
							if (ac != nullptr)
							{
								sd::SetRace(ac, wwRace);
								wwStage[refID] = Stage::Werewolf;
								if (g_thePlayer->formID == refID)
								{
									SET_TIMER_LIGHT(200, [] { AnimData_::Register(); });
								}
							}
							else
							{
								wwStage.erase(refID);
							}
						});
						if (skipAnim == false)
							cd::SendAnimationEvent(ac, "IdleWerewolfTransformation");
						sd::AddItem(ac, wwSkin, 1, true);
						sd::EquipItem(ac, wwSkin, false, true);
						if (lastRace[ac] == nullptr)
						{
							lastRace[ac] = ac->GetRace();
						}
					}
					else
					{
						sd::RemoveItem(ac, wwSkin, -1, true, nullptr);
						sd::SetRace(ac, lastRace[ac]);
						lastRace.erase(ac);
						wwStage.erase(refID);
						if (g_thePlayer->formID == refID)
						{
							SET_TIMER_LIGHT(1000, [] { AnimData_::Register(); });
						}
					}
				}
			});
		}
	}
}