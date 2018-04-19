#include "../stdafx.h"
#include "Skyrim\TESForms\Gameplay\BGSTextureSet.h"
#include "Skyrim\BSSystem\BSTCreateFactory.h"
#include "SKSE\GameInput.h"
#include "LookData.h"
#include "../CoreInterface/CoreInterface.h"

#include "AnimData.h"

class CIAccess
{
public:
	static void OnRaceMenuExit()
	{
		auto &logic = ci::IClientLogic::clientLogic;
		if (logic)
		{
			ci::IClientLogic::QueueCallback([=] {
				logic->OnRaceMenuExit();
			});
		}
	}
};

namespace LookData_
{
	ci::LookData lookToReapply;
	bool isReapply = false;
	ci::LookData localPlLookData;
	std::function<void()> afterApply;
	clock_t lastApplyTintMasks = 0;
	std::map<TESNPC *, std::vector<ci::LookData::TintMask>> appiedTintMasks;

	void Apply(const ci::LookData &lookData, Actor *actor);

	void QueueNiNodeUpdate(Actor *actor, int32_t numCalls = 0)
	{
		enum {
			Simple,
			VeryFatAndDeprecated
		};

		const int32_t mode = Simple;

		switch (mode)
		{
		case Simple:
			cd::QueueNiNodeUpdate(actor, {});
			return;
		case VeryFatAndDeprecated:
			if (!actor)
				return;
			auto npc = (TESNPC *)actor->baseForm;
			if (actor != g_thePlayer)
			{
				return cd::QueueNiNodeUpdate(actor);
			}

			if (npc->numHeadParts == 0)
			{
				if (numCalls < 3)
				{
					SET_TIMER(50, [=] {
						QueueNiNodeUpdate(g_thePlayer, numCalls + 1);
					});
				}
				else
				{
					isReapply = true;
					Apply(lookToReapply, g_thePlayer);
				}
			}
			else
			{
				const cd::Value<Actor> cdActor = actor;
				cd::QueueNiNodeUpdate(cdActor, [cdActor] {
				});
			}
			return;
		}
	}

#define PREFIX "actors\\character\\character assets\\tintmasks\\"
#define POSTFIX ".dds"
	BSFixedString TintMaskTextures[] = {
		"null",
		PREFIX"argonianbrowridge" POSTFIX, PREFIX"argoniancheeks" POSTFIX, PREFIX"argoniancheeks01" POSTFIX, PREFIX"argoniancheeks02" POSTFIX, PREFIX"argoniancheeks03" POSTFIX,
		PREFIX"argoniancheekslower" POSTFIX, PREFIX"argonianchin" POSTFIX, PREFIX"argoniandirt" POSTFIX, PREFIX"argonianeyeliner" POSTFIX, PREFIX"argonianeyesocketlower" POSTFIX,
		PREFIX"argonianeyesockets" POSTFIX, PREFIX"argonianeyesocketupper" POSTFIX, PREFIX"argonianforehead" POSTFIX, PREFIX"argonianlaughline" POSTFIX, PREFIX"argonianlips" POSTFIX,
		PREFIX"argonianlips01" POSTFIX, PREFIX"argonianlips02" POSTFIX, PREFIX"argonianneck" POSTFIX, PREFIX"argoniannostrils01" POSTFIX, PREFIX"argonianstripes01" POSTFIX,
		PREFIX"argonianstripes02" POSTFIX, PREFIX"argonianstripes03" POSTFIX, PREFIX"argonianstripes04" POSTFIX, PREFIX"argonianstripes05" POSTFIX, PREFIX"argonianstripes06" POSTFIX,
		PREFIX"femalehead_cheeks" POSTFIX, PREFIX"femalehead_cheeks2" POSTFIX, PREFIX"femalehead_frownlines" POSTFIX, PREFIX"femaleheadblackbloodtattoo_01" POSTFIX,
		PREFIX"femaleheadblackbloodtattoo_02" POSTFIX, PREFIX"femaleheadbothiahtattoo_01" POSTFIX, PREFIX"femaleheadbreton_lips" POSTFIX, PREFIX"femaleheadbretonwarpaint_01" POSTFIX,
		PREFIX"femaleheadbretonwarpaint_02" POSTFIX, PREFIX"femaleheaddarkelfwarpaint_01" POSTFIX, PREFIX"femaleheaddarkelfwarpaint_02" POSTFIX, PREFIX"femaleheaddarkelfwarpaint_03" POSTFIX,
		PREFIX"femaleheaddarkelfwarpaint_04" POSTFIX, PREFIX"femaleheaddarkelfwarpaint_05" POSTFIX, PREFIX"femaleheaddarkelfwarpaint_06" POSTFIX, PREFIX"femaleheaddirt_01" POSTFIX,
		PREFIX"femaleheaddirt_02" POSTFIX, PREFIX"femaleheaddirt_03" POSTFIX, PREFIX"femaleheadforsworntattoo_01" POSTFIX, PREFIX"femaleheadforsworntattoo_02" POSTFIX,
		PREFIX"femaleheadforsworntattoo_03" POSTFIX, PREFIX"femaleheadforsworntattoo_04" POSTFIX, PREFIX"femaleheadhighelf_lips" POSTFIX, PREFIX"femaleheadhighelfwarpaint_01" POSTFIX,
		PREFIX"femaleheadhighelfwarpaint_02" POSTFIX, PREFIX"femaleheadhighelfwarpaint_03" POSTFIX, PREFIX"femaleheadhighelfwarpaint_04" POSTFIX, PREFIX"femaleheadhuman_chin" POSTFIX,
		PREFIX"femaleheadhuman_forehead" POSTFIX, PREFIX"femaleheadhuman_neck" POSTFIX, PREFIX"femaleheadhuman_nose" POSTFIX, PREFIX"femaleheadimperial_lips" POSTFIX,
		PREFIX"femaleheadimperialwarpaint_01" POSTFIX, PREFIX"femaleheadimperialwarpaint_02" POSTFIX, PREFIX"femaleheadimperialwarpaint_03" POSTFIX, PREFIX"femaleheadimperialwarpaint_04" POSTFIX,
		PREFIX"femaleheadimperialwarpaint_05" POSTFIX, PREFIX"femaleheadnord_lips" POSTFIX, PREFIX"femaleheadnordwarpaint_01" POSTFIX, PREFIX"femaleheadnordwarpaint_02" POSTFIX,
		PREFIX"femaleheadnordwarpaint_03" POSTFIX, PREFIX"femaleheadnordwarpaint_04" POSTFIX, PREFIX"femaleheadnordwarpaint_05" POSTFIX, PREFIX"femaleheadorcwarpaint_01" POSTFIX,
		PREFIX"femaleheadorcwarpaint_02" POSTFIX, PREFIX"femaleheadorcwarpaint_03" POSTFIX, PREFIX"femaleheadorcwarpaint_04" POSTFIX, PREFIX"femaleheadorcwarpaint_05" POSTFIX,
		PREFIX"femaleheadredguard_lips" POSTFIX, PREFIX"femaleheadredguardwarpaint_01" POSTFIX, PREFIX"femaleheadredguardwarpaint_02" POSTFIX, PREFIX"femaleheadredguardwarpaint_03" POSTFIX,
		PREFIX"femaleheadredguardwarpaint_04" POSTFIX, PREFIX"femaleheadredguardwarpaint_05" POSTFIX, PREFIX"femaleheadwarpaint_01" POSTFIX, PREFIX"femaleheadwarpaint_02" POSTFIX,
		PREFIX"femaleheadwarpaint_03" POSTFIX, PREFIX"femaleheadwarpaint_04" POSTFIX, PREFIX"femaleheadwarpaint_05" POSTFIX, PREFIX"femaleheadwarpaint_06" POSTFIX,
		PREFIX"femaleheadwarpaint_07" POSTFIX, PREFIX"femaleheadwarpaint_08" POSTFIX, PREFIX"femaleheadwarpaint_09" POSTFIX, PREFIX"femaleheadwarpaint_10" POSTFIX,
		PREFIX"femaleheadwoodelfwarpaint_01" POSTFIX, PREFIX"femaleheadwoodelfwarpaint_02" POSTFIX, PREFIX"femaleheadwoodelfwarpaint_03" POSTFIX, PREFIX"femaleheadwoodelfwarpaint_04" POSTFIX,
		PREFIX"femaleheadwoodelfwarpaint_05" POSTFIX, PREFIX"femalelowereyesocket" POSTFIX, PREFIX"femalenordeyelinerstyle_01" POSTFIX, PREFIX"femaleuppereyesocket" POSTFIX,
		PREFIX"khajiitcheekcolor" POSTFIX, PREFIX"khajiitcheekcolorlower" POSTFIX, PREFIX"khajiitchin" POSTFIX, PREFIX"khajiitdirt" POSTFIX, PREFIX"khajiiteyeliner" POSTFIX,
		PREFIX"khajiiteyesocket01" POSTFIX, PREFIX"khajiiteyesocket02" POSTFIX, PREFIX"khajiiteyesocketlower" POSTFIX, PREFIX"khajiiteyesocketupper" POSTFIX,
		PREFIX"khajiitforehead" POSTFIX, PREFIX"khajiitlaughlines" POSTFIX, PREFIX"khajiitlipcolor" POSTFIX, PREFIX"khajiitneck" POSTFIX, PREFIX"khajiitnose01" POSTFIX,
		PREFIX"khajiitpaint01" POSTFIX, PREFIX"khajiitpaint02" POSTFIX, PREFIX"khajiitpaint03" POSTFIX, PREFIX"khajiitpaint04" POSTFIX, PREFIX"khajiitstripes01" POSTFIX,
		PREFIX"khajiitstripes02" POSTFIX, PREFIX"khajiitstripes03" POSTFIX, PREFIX"khajiitstripes04" POSTFIX, PREFIX"malehead_cheeks" POSTFIX, PREFIX"malehead_cheeks2" POSTFIX,
		PREFIX"malehead_frekles_01" POSTFIX, PREFIX"malehead_frownlines" POSTFIX, PREFIX"malehead_nose" POSTFIX, PREFIX"maleheadblackbloodtattoo_01" POSTFIX,
		PREFIX"maleheadblackbloodtattoo_02" POSTFIX, PREFIX"maleheadbothiahtattoo_01" POSTFIX, PREFIX"maleheadbretonwarpaint_01" POSTFIX, PREFIX"maleheadbretonwarpaint_02" POSTFIX,
		PREFIX"maleheadbretonwarpaint_03" POSTFIX, PREFIX"maleheadbretonwarpaint_04" POSTFIX, PREFIX"maleheadbretonwarpaint_05" POSTFIX, PREFIX"maleheaddarkelfwarpaint_01" POSTFIX,
		PREFIX"maleheaddarkelfwarpaint_02" POSTFIX, PREFIX"maleheaddarkelfwarpaint_03" POSTFIX, PREFIX"maleheaddarkelfwarpaint_04" POSTFIX, PREFIX"maleheaddarkelfwarpaint_05" POSTFIX,
		PREFIX"maleheaddarkelfwarpaint_06" POSTFIX, PREFIX"maleheaddirt_01" POSTFIX, PREFIX"maleheaddirt_02" POSTFIX, PREFIX"maleheaddirt_03" POSTFIX, PREFIX"maleheadforsworntattoo_01" POSTFIX,
		PREFIX"maleheadforsworntattoo_02" POSTFIX, PREFIX"maleheadforsworntattoo_03" POSTFIX, PREFIX"maleheadforsworntattoo_04" POSTFIX, PREFIX"maleheadhuman_chin" POSTFIX,
		PREFIX"maleheadhuman_forehead" POSTFIX, PREFIX"maleheadhuman_neck" POSTFIX, PREFIX"maleheadimperialwarpaint_01" POSTFIX, PREFIX"maleheadimperialwarpaint_02" POSTFIX,
		PREFIX"maleheadimperialwarpaint_03" POSTFIX, PREFIX"maleheadimperialwarpaint_04" POSTFIX, PREFIX"maleheadimperialwarpaint_05" POSTFIX, PREFIX"maleheadnord_lips" POSTFIX,
		PREFIX"maleheadnordwarpaint_01" POSTFIX, PREFIX"maleheadnordwarpaint_02" POSTFIX, PREFIX"maleheadnordwarpaint_03" POSTFIX, PREFIX"maleheadnordwarpaint_04" POSTFIX,
		PREFIX"maleheadnordwarpaint_05" POSTFIX, PREFIX"maleheadorcwarpaint_01" POSTFIX, PREFIX"maleheadorcwarpaint_02" POSTFIX, PREFIX"maleheadorcwarpaint_03" POSTFIX,
		PREFIX"maleheadorcwarpaint_04" POSTFIX, PREFIX"maleheadorcwarpaint_05" POSTFIX, PREFIX"maleheadredguard_lips" POSTFIX, PREFIX"maleheadredguardwarpaint_01" POSTFIX,
		PREFIX"maleheadredguardwarpaint_02" POSTFIX, PREFIX"maleheadredguardwarpaint_03" POSTFIX, PREFIX"maleheadredguardwarpaint_04" POSTFIX, PREFIX"maleheadredguardwarpaint_05" POSTFIX,
		PREFIX"maleheadwarpaint_01" POSTFIX, PREFIX"maleheadwarpaint_02" POSTFIX, PREFIX"maleheadwarpaint_03" POSTFIX, PREFIX"maleheadwarpaint_04" POSTFIX, PREFIX"maleheadwarpaint_05" POSTFIX,
		PREFIX"maleheadwarpaint_06" POSTFIX, PREFIX"maleheadwarpaint_07" POSTFIX, PREFIX"maleheadwarpaint_08" POSTFIX, PREFIX"maleheadwarpaint_09" POSTFIX, PREFIX"maleheadwarpaint_10" POSTFIX,
		PREFIX"maleheadwoodelfwarpaint_01" POSTFIX, PREFIX"maleheadwoodelfwarpaint_02" POSTFIX, PREFIX"maleheadwoodelfwarpaint_03" POSTFIX, PREFIX"maleheadwoodelfwarpaint_04" POSTFIX,
		PREFIX"maleheadwoodelfwarpaint_05" POSTFIX, PREFIX"malelowereyesocket" POSTFIX, PREFIX"maleuppereyesocket" POSTFIX, PREFIX"redguardmaleeyelinerstyle_01" POSTFIX, PREFIX"skintone" POSTFIX,
	};
	enum {
		NUM_TINT_MASK_TEXTURES = 190
	};
	STATIC_ASSERT(sizeof TintMaskTextures == NUM_TINT_MASK_TEXTURES * sizeof size_t);

	UInt8 GetTintMaskTextureID(const char *path)
	{
		if (path)
			for (size_t i = 0; i != NUM_TINT_MASK_TEXTURES; ++i)
			{
				if (!strcmp(path, TintMaskTextures[i]))
					return i;
			}
		return NULL;
	}

	BSFixedString GetTintMaskTexturePath(UInt8 tintMaskID)
	{
		if (tintMaskID >= NUM_TINT_MASK_TEXTURES)
			tintMaskID = NULL;
		return TintMaskTextures[tintMaskID];
	}

	enum PlayableRace : UInt32
	{
		ArgonianRace = 0x00013740,
		BretonRace,
		DarkElfRace,
		HighElfRace,
		ImperialRace,
		KhajiitRace,
		NordRace,
		OrcRace,
		RedguardRace,
		WoodElfRace,

		RaceMIN = ArgonianRace,
		RaceMAX = WoodElfRace
	};

	bool IsHeadPartValid(BGSHeadPart *headpart, RefHandle raceID)
	{
		if (!headpart)
			return false;
		auto listForm = headpart->validRaces;
		if (!listForm)
			return false;
		using namespace ID_BGSListForm;
		switch (raceID)
		{
		case NULL:
			break;
		case PlayableRace::ArgonianRace:
			switch (listForm->formID)
			{
			case HeadPartsArgonian:
			case HeadPartsArgonianandVampire:
				return true;
			}
			break;
		case PlayableRace::BretonRace:
			switch (listForm->formID)
			{
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsHuman:
			case HeadPartsHumansandVampires:
			case HeadPartsBretsNordsImpsandVampires:
			case HeadPartsBreton:
			case HeadPartsBretonandVampire:
			case HeadPartsHumansOrcsandVampires:
				return true;
			}
			break;
		case PlayableRace::DarkElfRace:
			switch (listForm->formID)
			{
			case HeadPartsHuman://
			case HeadPartsHumansandVampires://
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsElves:
			case HeadPartsElvesandVampires:
			case HeadPartsDarkElf:
			case HeadPartsDarkElfandVampire:
				return true;
			}
			break;
		case PlayableRace::HighElfRace:
			switch (listForm->formID)
			{
			case HeadPartsHuman://
			case HeadPartsHumansandVampires://
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsElves:
			case HeadPartsElvesandVampires:
			case HeadPartsHighElf:
			case HeadPartsHighElfandVampire:
				return true;
			}
			break;
		case PlayableRace::ImperialRace:
			switch (listForm->formID)
			{
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsHuman:
			case HeadPartsHumansandVampires:
			case HeadPartsBretsNordsImpsandVampires:
			case HeadPartsImperial:
			case HeadPartsImperialandVampire:
			case HeadPartsHumansOrcsandVampires:
				return true;
			}
			break;
		case PlayableRace::KhajiitRace:
			switch (listForm->formID)
			{
			case HeadPartsKhajiit:
			case HeadPartsKhajiitandVampire:
				return true;
			}
			break;
		case PlayableRace::NordRace:
			switch (listForm->formID)
			{
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsHuman:
			case HeadPartsHumansandVampires:
			case HeadPartsBretsNordsImpsandVampires:
			case HeadPartsNord:
			case HeadPartsNordandVampire:
			case HeadPartsHumansOrcsandVampires:
				return true;
			}
			break;
		case PlayableRace::OrcRace:
			switch (listForm->formID)
			{
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsOrc:
			case HeadPartsOrcandVampire:
			case HeadPartsHumansOrcsandVampires:
				return true;
			}
			break;
		case PlayableRace::RedguardRace:
			switch (listForm->formID)
			{
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsHuman:
			case HeadPartsHumansandVampires:
			case HeadPartsRedguard:
			case HeadPartsRedguardandVampire:
			case HeadPartsHumansOrcsandVampires:
				return true;
			}
			break;
		case PlayableRace::WoodElfRace:
			switch (listForm->formID)
			{
			case HeadPartsHuman://
			case HeadPartsHumansandVampires://
			case HeadPartsAllRacesMinusBeast:
			case HeadPartsAllRacesMinusBeastVampires:
			case HeadPartsElves:
			case HeadPartsElvesandVampires:
			case HeadPartsWoodElf:
			case HeadPartsWoodElfandVampire:
				return true;
			}
			break;
		}
		return false;
	}

	__forceinline void Warning(std::string msg)
	{
		/*SET_TIMER(0, [=] {
			char buf[512];
			sprintf_s<sizeof buf>(buf, "Warning: %s", msg.data());
			sd::PrintNote(buf);
		});*/
		//ErrorHandling::SendError("WARN:LookData %s", msg.data());
	}

	ci::LookData GetFrom(TESNPC *pl)
	{
		ci::LookData result;

		result.isFemale = sd::GetSex(pl) == 1;
		result.raceID = 
			(pl == g_thePlayer->baseForm) ? sd::GetRace(g_thePlayer)->formID : pl->race->formID;
		result.weight = UInt8(pl->weight) / 10;
		result.skinColor = { pl->color.red, pl->color.green , pl->color.blue , 255 };
		if (pl->headData && pl->headData->hairColor)
		{
			auto hairColor = pl->headData->hairColor->color;
			result.hairColor = { hairColor.red, hairColor.green, hairColor.blue, 0 };
		}
		else
			result.hairColor = { 57,55,40,0 };
		if (pl->headData && pl->headData->headTexture)
			result.headTextureSetID = pl->headData->headTexture->formID;
		if (pl->headparts)
		{
			for (auto it = pl->headparts; it != pl->headparts + pl->numHeadParts; ++it)
				result.headpartIDs.push_back((*it)->formID);
		}
		if (pl->faceMorph)
		{
			std::copy(pl->faceMorph->option, pl->faceMorph->option + pl->faceMorph->kNumOptions,
				result.options.begin());
			std::copy(pl->faceMorph->presets, pl->faceMorph->presets + pl->faceMorph->kNumPresets,
				result.presets.begin());
		}
		if (pl == g_thePlayer->baseForm)
		{
			for (auto it = g_thePlayer->tintMasks.begin(); it != g_thePlayer->tintMasks.end(); ++it)
			{
				ci::LookData::TintMask m;
				std::string texture_path = (*it)->texture->str.c_str();
				std::transform(texture_path.begin(), texture_path.end(), texture_path.begin(), ::tolower);
				m.alpha = (*it)->alpha;
				m.color = { (*it)->color.red, (*it)->color.green, (*it)->color.blue, (*it)->color.alpha };
				m.tintMaskTextureID = GetTintMaskTextureID(texture_path.data());
				m.tintType = (*it)->tintType;
				result.tintmasks.push_back(m);
			}
		}
		else
			result.tintmasks = appiedTintMasks[pl];

		return result;
	}

	ci::LookData GetFromPlayerImpl()
	{
		auto pl = (TESNPC *)sd::GetPlayer()->baseForm;
		return GetFrom(pl);
	}

	void ApplyTintMasks(TESNPC *npc, const std::vector<ci::LookData::TintMask> &result_tintmasks)
	{
		const auto lastApplied = appiedTintMasks[npc];
		appiedTintMasks[npc] = result_tintmasks;
		lastApplyTintMasks = clock();

		const bool isPlayer =
			(npc == g_thePlayer->baseForm);
		const bool isFemale = sd::GetSex(npc) == 1;

		if (result_tintmasks.size() >= 64)
		{
			Warning("Too many TintMasks");
			goto fail;
		}

		if (npc->race && npc->race->chargenData[isFemale])
		{
			auto &chargenData = *npc->race->chargenData[isFemale];
			if (chargenData.tintData)
			{
				auto &tintData = *chargenData.tintData;

				const size_t tintmask_count = result_tintmasks.size();
				g_thePlayer->tintMasks.reserve(tintmask_count);

				bool any_error = false;
				TintMask *skintone = nullptr;
				for (size_t i = 0; i != tintmask_count; ++i)
				{
					auto _new = FormHeap_Allocate<TintMask>();
					_new->color = { 
						result_tintmasks[i].color.r, result_tintmasks[i].color.g, result_tintmasks[i].color.b, result_tintmasks[i].color.a 
					};
					_new->color.alpha = 0;
					_new->alpha = result_tintmasks[i].alpha;
					_new->tintType = result_tintmasks[i].tintType;

					auto texture = (TESTexture *)FormHeap_Allocate(sizeof TESTexture);
					texture->str = GetTintMaskTexturePath(result_tintmasks[i].tintMaskTextureID);
					_new->texture = texture;

					bool illegal_tint_mask = true;
					TESRace::CharGenData::TintOption *tintOption = nullptr;
					for (size_t j = 0; j != tintData.GetSize(); ++j)
					{
						if (!strcmp(tintData[j]->texture.str.c_str(), _new->texture->str.c_str()))
							tintOption = tintData[j];
					}
					if (tintOption)
					{
						if (((_new->color.red == 255 && _new->color.green == 255 && _new->color.blue == 255)
							|| (_new->color.red == 253 && _new->color.green == 253 && _new->color.blue == 253))
							&& _new->alpha == 0.0f)
							illegal_tint_mask = false;
						else
						{
							auto &colors = tintOption->colors;
							auto &alpha = tintOption->alpha;
							for (size_t j = 0; j != colors.GetSize(); ++j)
							{
								if (colors[j]
									&& colors[j]->color.red == _new->color.red
									&& colors[j]->color.green == _new->color.green
									&& colors[j]->color.blue == _new->color.blue)
									illegal_tint_mask = false;
							}
						}
					}
					any_error = any_error || illegal_tint_mask;

					if (!i && !skintone && std::string(_new->texture->str.c_str()).find("skintone") != std::string::npos)
						skintone = _new;
					g_thePlayer->tintMasks[i] = _new;
				}
				if (any_error)
				{
					Warning("Invalid TintMask");
					goto fail;
				}
				else
				{
					if (skintone && skintone->tintType == skintone->kMaskType_SkinTone)
					{
						if (skintone->color.red == npc->color.red
							&& skintone->color.green == npc->color.green
							&& skintone->color.blue == npc->color.blue)
						{
						}
						else if(npc != g_thePlayer->baseForm)
						{
							Warning("Skintone TintMask color conflicts with body color"); 
							// ^ Ложные срабатывания на аргониан, котов без каких-либо видимых последствий
							npc->color.red = skintone->color.red;
							npc->color.green = skintone->color.green;
							npc->color.blue = skintone->color.blue;
							NiColorA colorResult;
							npc->SetSkinFromTint(&colorResult, skintone, 1, 0);
						}
					}
					else
					{
						Warning("Unable to find skintone TintMask");
						goto fail;
					}
				}
				return;
			}
			return Warning("Unable to get TintMask data");
		}
		return Warning("Unable to get any CharGen data");

	fail:
		appiedTintMasks[npc] = lastApplied;
		npc->color = { 167, 134, 122 };
		if (npc->race && npc->race->chargenData[isFemale])
		{
			auto &chargenData = *npc->race->chargenData[isFemale];
			if (chargenData.presets && chargenData.presets->GetSize() && chargenData.presets->begin())
			{
				auto preset = chargenData.presets->front();
				if (preset && preset->formType == FormType::NPC)
				{
					ci::LookData::TintMask tM;
					tM.tintMaskTextureID = GetTintMaskTextureID(PREFIX "skintone" POSTFIX);
					tM.alpha = 1;
					npc->color.red = tM.color.r = preset->color.red;
					npc->color.green = tM.color.g = preset->color.green;
					npc->color.blue = tM.color.b = preset->color.blue;
					tM.color.a = 0;
					tM.tintType = TintMask::kMaskType_SkinTone;

					static UInt8 ApplyTintMasks_Calls = 0;
					if (ApplyTintMasks_Calls++ < 5)
					{
						ApplyTintMasks(npc, { tM });
					}
					else
					{
						Warning("Too many ApplyTintMasks calls");
						ApplyTintMasks_Calls = 0;
					}
				}
			}
		}
	}

	void ApplyTintMasksPlayerAsync(const std::vector<ci::LookData::TintMask> &result_tintmasks, bool queueNiNodeUpdate = false, uint32_t delayMs = 0)
	{
		SET_TIMER(delayMs, [=] {
			if (lastApplyTintMasks + 500 < clock())
			{
				auto npc = (TESNPC *)g_thePlayer->baseForm;
				ApplyTintMasks(npc, result_tintmasks);
				if (queueNiNodeUpdate)
					QueueNiNodeUpdate(g_thePlayer);
			}
			else
				ApplyTintMasksPlayerAsync(result_tintmasks, true, 100);
		});
	}

	void ApplyBodyData(TESNPC *npc, Actor *actor, UInt32 result_raceID, UInt8 isFemale, UInt8 weight, TESNPC::Color color)
	{
		if (weight > 10)
		{
			weight = 10;
			Warning("Invalid weight");
		}
		if (isFemale > TRUE)
		{
			isFemale = FALSE;
			Warning("Invalid sex");
		}

		if (result_raceID < PlayableRace::RaceMIN || result_raceID > PlayableRace::RaceMAX)
		{
			result_raceID = PlayableRace::NordRace;
			Warning("Invalid race");
		}
		auto race = (TESRace *)LookupFormByID(result_raceID);
		if (race && race->formType == FormType::Race)
		{
			npc->race = race;
			npc->color = color;
		}
		else
		{
			npc->numHeadParts = 0;
			npc->headparts = nullptr;
		}

		const bool isPlayer =
			(npc == sd::GetPlayer()->baseForm);
		const UInt8 isFemale_old = sd::GetSex(npc) == 1;
		const float original_weight = npc->weight;
		float new_weight = weight * 10;

		npc->TESActorBaseData::flags.female = !!isFemale;

		npc->weight = new_weight;
		npc->sleepOutfit = npc->defaultOutfit = nullptr;
		npc->height = 1;

		if (actor && actor->baseForm == npc)
		{
			const UInt32 refID = actor->formID;
			afterApply = [refID, original_weight, new_weight, isPlayer, isFemale, isFemale_old] {
				SET_TIMER(0, [=] {
					const auto actor = cd::Value<Actor>(refID);
					const auto neck_delta = (original_weight / 100.f) - (new_weight / 100.f);
					if (neck_delta)
						cd::UpdateWeight(actor, neck_delta, [=] {
							QueueNiNodeUpdate(actor);
						});
					else
						QueueNiNodeUpdate(actor);
					if (isPlayer)
					{
						if (isFemale_old != isFemale)
							cd::Resurrect(actor);
					}
				});
			};
		}
	}

	void ApplyHeadparts(TESNPC *npc, const std::vector<UInt32> &headpart_IDs)
	{
		if (!npc)
			return;
		const auto headpart_count = headpart_IDs.size();
		if (headpart_count >= 6 && headpart_count <= 10)
		{
			const bool isFemale = npc->TESActorBaseData::flags.female;
			bool hasFace = false, hasEyes = false;
			std::vector<BGSHeadPart *> validHeadParts;
			for (auto it = headpart_IDs.begin(); it != headpart_IDs.end(); ++it)
			{
				const UInt32 headpart_id = *it;
				auto headpart = (BGSHeadPart *)LookupFormByID(headpart_id);
				if (headpart && headpart->formType == FormType::HeadPart && headpart->validRaces)
				{
					bool valid = IsHeadPartValid(headpart, npc->race->formID);

					auto isFlagSet = [](BGSHeadPart *h, UInt32 flag) {
						return h && (h->partFlags & flag) == flag;
					};

					if ((isFlagSet(headpart, BGSHeadPart::kFlagMale) && !isFlagSet(headpart, BGSHeadPart::kFlagFemale) && isFemale)
						|| (isFlagSet(headpart, BGSHeadPart::kFlagFemale) && !isFlagSet(headpart, BGSHeadPart::kFlagMale) && !isFemale))
						valid = false;

					if (!valid)
					{
						std::string str = "Invalid HeadPart ";
						using I = uint32_t;
						auto makeHexStr = [](I w, size_t hex_len) {
							static const char* digits = "0123456789ABCDEF";
							std::string rc(hex_len, '0');
							for (size_t i = 0, j = (hex_len - 1) * 4; i<hex_len; ++i, j -= 4)
								rc[i] = digits[(w >> j) & 0x0f];
							return rc;
						};
						str += makeHexStr(headpart_id, sizeof(I) << 1);
						Warning(str.data());
						goto fail;
					}
					else
					{
						for (auto it = validHeadParts.begin(); it != validHeadParts.end(); ++it)
						{
							if (!*it || *it == headpart)
							{
								Warning("HeadPart isn't unique");
								goto fail;
							}
						}
						if (headpart->type == headpart->kTypeFace)
							hasFace = true;
						if (headpart->type == headpart->kTypeEyes)
							hasEyes = true;
						validHeadParts.push_back(headpart);
					}
				}
				else
					Warning("Unknown HeadPart");
			}

			if (validHeadParts.empty())
				goto fail;
			else
			{
				if (hasFace && hasEyes)
				{
					npc->headparts = new BGSHeadPart*[npc->numHeadParts = validHeadParts.size()];
					std::copy(validHeadParts.begin(), validHeadParts.end(), npc->headparts);
				}
				else
				{
					Warning("Invalid HeadParts set");
					goto fail;
				}
			}
			return;
		}
		Warning("Invalid HeadParts count");
	fail:
		npc->numHeadParts = 0;
		npc->headparts = nullptr;
	}

	void ApplyHeadData(TESNPC *npc, decltype(BGSColorForm::color) result_hair_color, UInt32 texture_set_id)
	{
		auto factory = ((IFormFactory **)0x012E57B0)[BGSColorForm::kTypeID];
		if (factory)
		{
			auto hair_color = (BGSColorForm *)factory->Create();
			hair_color->color = result_hair_color;
			{
				if (!npc->headData)
				{
					npc->headData = (TESNPC::HeadData *)FormHeap_Allocate(sizeof TESNPC::HeadData);
					npc->headData->hairColor = nullptr;
				}

				npc->headData->headTexture = nullptr;
				npc->headData->hairColor = hair_color;
				const UInt8 isFemale = sd::GetSex(npc) == 1;
				if (npc->race && npc->race->chargenData[isFemale] && npc->race->chargenData[isFemale]->textureSet && npc->race->chargenData[isFemale]->colors)
				{
					auto &textureSet = *npc->race->chargenData[isFemale]->textureSet;
					auto &colors = *npc->race->chargenData[isFemale]->colors;

					bool success = false;
					for (size_t i = 0; i < colors.GetSize(); ++i)
						if (colors[i]->color.blue == hair_color->color.blue
							&& colors[i]->color.red == hair_color->color.red
							&& colors[i]->color.green == hair_color->color.green)
						{
							success = true;
							break;
						}
					if (!success)
					{
						Warning("Invalid hair color");
						auto defaultColor = npc->race->chargenData[isFemale]->defaultColor;
						if (colors.GetSize() != 0)
							npc->headData->hairColor = defaultColor ? defaultColor : colors[0];
						else if (npc->headData->hairColor)
							*(UInt32 *)&npc->headData->hairColor->color = NULL;
					}

					if (texture_set_id)
					{
						bool exist = false;
						auto resultTextureSet = (BGSTextureSet *)LookupFormByID(texture_set_id);
						if (resultTextureSet && resultTextureSet->formType == FormType::TextureSet)
							for (size_t i = 0; i < textureSet.GetSize(); ++i)
								if (textureSet[i] && textureSet[i]->formType == FormType::TextureSet && textureSet[i]->formID == texture_set_id)
								{
									exist = true;
									break;
								}
						if (exist)
							npc->headData->headTexture = resultTextureSet;
						else
							Warning("Invalid texture set");
					}
				}
				else
					Warning("Unable to get CharGen data for texture sets");
			}
		}
	}

	void ApplyFaceMorph(TESNPC *npc,
		const decltype(ci::LookData::presets) &presets,
		const decltype(ci::LookData::options) &options)
	{
		if (!npc->faceMorph)
			npc->faceMorph = new TESNPC::FaceMorphs;
		std::copy(presets.begin(), presets.end(), npc->faceMorph->presets);
		std::copy(options.begin(), options.end(), npc->faceMorph->option);
	}

	bool appliedAny = false;

	ci::LookData playerBackup;

	void ApplyImpl(ci::LookData lookData, TESNPC *npc, Actor *actor = nullptr)
	{
		appliedAny = true;

		if (actor == g_thePlayer)
		{
			playerBackup = lookData;
			static bool fixed = false;
			if (!fixed)
			{
				fixed = true;
				SET_TIMER(0, [] {
					SET_TIMER(0, [] {
						sd::ToggleMenus();
						std::thread([] {
							auto mm = MenuManager::GetSingleton();
							mm->OpenMenu("RaceSex Menu");
							Sleep(100);
							mm->CloseMenu("RaceSex Menu");
							SET_TIMER(0, [] {
								sd::ForceThirdPerson();
								sd::ForceFirstPerson();
								sd::ToggleMenus();
							});
						}).detach();
					});
				});
				return SET_TIMER(300, [=] {
					ApplyImpl(lookData, npc, g_thePlayer);
					std::thread([=] {
						auto mm = MenuManager::GetSingleton();
						while (mm->IsMenuOpen("Main Menu") == false)
							Sleep(500);
						while (mm->IsMenuOpen("Main Menu") == true)
							Sleep(500);
						fixed = false;
						SET_TIMER(0, [=] {
							ApplyImpl(playerBackup, npc, g_thePlayer);
							SET_TIMER(300, [] {
								AnimData_::Register();
							});
						});
					}).detach();
				});
			}
		}

		const bool isThePlayer = npc == g_thePlayer->baseForm;

		static LockTimer applyImplLock(333);
		void *any = nullptr;
		const bool locked = applyImplLock.TryLock(isThePlayer ? g_thePlayer : any);
		if (!locked && isThePlayer)
		{
			SET_TIMER(500, [=] {
				ApplyImpl(lookData, npc, actor);
			});
			return;
		}

		if (!npc)
			return;
		if (actor && actor->baseForm != npc)
			actor = nullptr;

		TESNPC::Color skinColor;
		skinColor.red = lookData.skinColor.r;
		skinColor.green = lookData.skinColor.g;
		skinColor.blue = lookData.skinColor.b;

		BGSColorForm::Color hairColor;
		hairColor.red = lookData.hairColor.r;
		hairColor.green = lookData.hairColor.g;
		hairColor.blue = lookData.hairColor.b;

		/*if (isThePlayer)
		{
			SET_TIMER(750, [=] {
				npc->headparts = nullptr;
				npc->numHeadParts = 0;
			});
		}*/

		ApplyBodyData(npc, actor, lookData.raceID, lookData.isFemale, lookData.weight, skinColor);
		ApplyHeadparts(npc, lookData.headpartIDs);
		ApplyHeadData(npc, hairColor, lookData.headTextureSetID);
		ApplyFaceMorph(npc, lookData.presets, lookData.options);

		npc->TESActorBaseData::flags.dontAffectStealth = true;

		if (isThePlayer)
		{
			//ApplyTintMasks(npc, lookData.tintmasks);
			ApplyTintMasksPlayerAsync(lookData.tintmasks);
		}

		if (afterApply != nullptr)
		{
			afterApply();
			afterApply = nullptr;
		}
	}

	TESNPC *GetSourceNPC(UInt8 isFemale)
	{
		static std::vector<TESNPC *> npcs;
		//sd::PrintNote("NPC Count: %u", npcs.size());
		if (npcs.empty())
		{
			for (UInt32 id = 0x00000EA6; id != 0x0010FEEE; id++)
			{
				auto npc = (TESNPC *)LookupFormByID(id);
				if (npc && npc->formType == FormType::NPC)
				{
					if (npc->npcClass == LookupFormByID(0x1ce17) || npc->npcClass == LookupFormByID(0x15be7) || npc->npcClass == LookupFormByID(0x39d30))//bandit
						if (npc->race->formID >= PlayableRace::RaceMIN && npc->race->formID <= PlayableRace::RaceMAX && npc->TESActorBaseData::flags.female == !!isFemale && npc->TESActorBaseData::flags.ghost == false)
							npcs.push_back(npc);
				}
			}
		}
		static auto it = npcs.begin();
		auto result = *it;
		++it;
		if (it == npcs.end())
			it = npcs.begin();
		result->TESActorBaseData::flags.oppositeGenderAnims = false;
		return result;
	}

	TESNPC *Apply(const ci::LookData &lookData)
	{
		const auto srcNpc =
			GetSourceNPC(lookData.isFemale);

		auto newNpc = FormHeap_Allocate<TESNPC>();
		memcpy(newNpc, srcNpc, sizeof TESNPC);

		newNpc->voiceType = (BGSVoiceType *)LookupFormByID(0x0002F7C3);
		newNpc->formID = 0;
		newNpc->SetFormID(Utility::NewFormID(), false);
		newNpc->TESAIForm::flags = ((TESNPC *)g_thePlayer->baseForm)->TESAIForm::flags;
		newNpc->TESAIForm::unk10.unk0 = 0;
		newNpc->TESAIForm::unk10.next = nullptr;
		newNpc->TESContainer::entries = nullptr;
		newNpc->TESContainer::numEntries = 0;
		newNpc->faction = nullptr;
		newNpc->nextTemplate = nullptr;
		newNpc->TESActorBaseData::flags.pcLevelMult = false;
		newNpc->TESSpellList::unk04 = nullptr;
		newNpc->TESActorBaseData::flags.unique = true;
		newNpc->TESActorBaseData::flags.simpleActor = true;
		ApplyImpl(lookData, newNpc);
		return newNpc;
	}

	void Apply(const ci::LookData &lookData, Actor *actor)
	{
		if (actor == g_thePlayer)
			lookToReapply = lookData;
		if (!actor)
			return;
		auto formID = actor->formID;
		ApplyImpl(lookData, (TESNPC *)actor->baseForm, actor);

		if (actor == g_thePlayer)
		{
			if (!isReapply)
			{
				SET_TIMER(400, [=] {
					auto actor = (Actor *)LookupFormByID(formID);
					if (actor)
						ApplyImpl(lookData, (TESNPC *)actor->baseForm, actor);
				});
			}
			isReapply = false;
		}
	}

	ci::LookData GetFromPlayer(bool noCach)
	{
		if (localPlLookData.isEmpty() || noCach)
			localPlLookData = GetFromPlayerImpl();
		return localPlLookData;
	}

	ci::LookData GetFromNPC(TESNPC *npc)
	{
		return npc != nullptr ? GetFrom(npc) : ci::LookData();
	}

	void ShowRaceMenu()
	{
		const auto name = ci::LocalPlayer::GetSingleton()->GetName();
		sd::Wait(50);

		g_thePlayer->GetActorBase()->numHeadParts = 0;
		g_thePlayer->GetActorBase()->race = (TESRace *)LookupFormByID(NordRace);

		static auto mod = (TESImageSpaceModifier *)sd::GetFormById(0x000434BB);

		sd::Apply(mod, 33.0f);

		sd::ShowRaceMenu();
		std::thread([name] {
			auto mm = MenuManager::GetSingleton();
			bool wasOpen = false;
			while (1)
			{
				bool open = mm->IsMenuOpen("RaceSex Menu");
				if (open)
				{
					if (!wasOpen)
					{
						//if (appliedAny)
						{
							Sleep(600);
							sd::ImageSpaceModifier::Remove(mod);
							const bool isDarkElf = ((TESNPC *)g_thePlayer->baseForm)->race->formID == PlayableRace::DarkElfRace;
							auto keys =
								isDarkElf ? std::pair<SInt32, SInt32>{ DIK_W, DIK_S } : std::pair<SInt32, SInt32>{ DIK_S, DIK_W };

							keybd_event(VK_LSHIFT, keys.first, NULL, NULL);
							keybd_event(VK_LSHIFT, keys.first, KEYEVENTF_KEYUP, NULL);
							keybd_event(VK_LCONTROL, keys.second, NULL, NULL);
							keybd_event(VK_LCONTROL, keys.second, KEYEVENTF_KEYUP, NULL);
						}

						wasOpen = true;
					}

					if (sd::GetKeyPressed('R') || sd::GetKeyPressed('T') || sd::GetKeyPressed(VK_F6))
					{
						keybd_event(VK_LSHIFT, DIK_ESCAPE, NULL, NULL);
						keybd_event(VK_LSHIFT, DIK_ESCAPE, KEYEVENTF_KEYUP, NULL);
					}

					if (*(UInt8 *)((*(UInt32 *)0x012E7458) + 0x98)) //allowTextInput
					{
						keybd_event(VK_LSHIFT, DIK_W, NULL, NULL);
						keybd_event(VK_LSHIFT, DIK_W, KEYEVENTF_KEYUP, NULL);
						keybd_event(VK_LSHIFT, DIK_RETURN, NULL, NULL);
						keybd_event(VK_LSHIFT, DIK_RETURN, KEYEVENTF_KEYUP, NULL);
					}
				}
				if (!open && wasOpen)
					break;
				Sleep(1);
			}
			return SET_TIMER(0, [name] {
				localPlLookData = GetFromPlayerImpl();
				playerBackup = localPlLookData;
				CIAccess::OnRaceMenuExit();
				ci::LocalPlayer::GetSingleton()->SetName(name);
				AnimData_::Register();
				SET_TIMER(100, [] {
					sd::Resurrect(g_thePlayer);
				});
			});
		}).detach();
	}

	void PreventCrash(TESNPC *npc)
	{
		if (npc)
		{
			npc->headparts = nullptr;
			npc->numHeadParts = 0;
		}
	}
}

ILookSynchronizer *ILookSynchronizer::GetV17()
{
	class V17 : public ILookSynchronizer
	{
	public:
		TESNPC *Apply(const ci::LookData &lookData) override {
			return LookData_::Apply(lookData);
		}
		void Apply(const ci::LookData &lookData, Actor *actor) override {
			return LookData_::Apply(lookData, actor);
		}
		void ApplyTintMasks(TESNPC *npc, const std::vector<ci::LookData::TintMask> &result_tintmasks) override  {
			return LookData_::ApplyTintMasks(npc, result_tintmasks);
		}
		ci::LookData GetFromPlayer(bool noCache = false) override {
			return LookData_::GetFromPlayer(noCache);
		}
		ci::LookData GetFromNPC(TESNPC *npc) override {
			return LookData_::GetFromNPC(npc);
		}
		void ShowRaceMenu() override {
			return LookData_::ShowRaceMenu();
		}
	};

	static auto synchronizer = new V17;
	return synchronizer;
}