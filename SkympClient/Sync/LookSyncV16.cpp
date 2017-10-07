#include "../stdafx.h"
#include "Skyrim\TESForms\Gameplay\BGSTextureSet.h"
#include "Skyrim\BSSystem\BSTCreateFactory.h"
#include "SKSE\GameInput.h"
#include "LookData.h"
#include "../CoreInterface/CoreInterface.h"

enum HumanRace : UInt32
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

#define PREFIX "actors\\character\\character assets\\tintmasks\\"
#define POSTFIX ".dds"

const char *const TintMaskTextures[] = {
	"null",
	PREFIX"argonianbrowridge" POSTFIX,
	PREFIX"argoniancheeks" POSTFIX,
	PREFIX"argoniancheeks01" POSTFIX,
	PREFIX"argoniancheeks02" POSTFIX,
	PREFIX"argoniancheeks03" POSTFIX,
	PREFIX"argoniancheekslower" POSTFIX,
	PREFIX"argonianchin" POSTFIX,
	PREFIX"argoniandirt" POSTFIX,
	PREFIX"argonianeyeliner" POSTFIX,
	PREFIX"argonianeyesocketlower" POSTFIX,
	PREFIX"argonianeyesockets" POSTFIX,
	PREFIX"argonianeyesocketupper" POSTFIX,
	PREFIX"argonianforehead" POSTFIX,
	PREFIX"argonianlaughline" POSTFIX,
	PREFIX"argonianlips" POSTFIX,
	PREFIX"argonianlips01" POSTFIX,
	PREFIX"argonianlips02" POSTFIX,
	PREFIX"argonianneck" POSTFIX,
	PREFIX"argoniannostrils01" POSTFIX,
	PREFIX"argonianstripes01" POSTFIX,
	PREFIX"argonianstripes02" POSTFIX,
	PREFIX"argonianstripes03" POSTFIX,
	PREFIX"argonianstripes04" POSTFIX,
	PREFIX"argonianstripes05" POSTFIX,
	PREFIX"argonianstripes06" POSTFIX,
	PREFIX"femalehead_cheeks" POSTFIX,
	PREFIX"femalehead_cheeks2" POSTFIX,
	PREFIX"femalehead_frownlines" POSTFIX,
	PREFIX"femaleheadblackbloodtattoo_01" POSTFIX,
	PREFIX"femaleheadblackbloodtattoo_02" POSTFIX,
	PREFIX"femaleheadbothiahtattoo_01" POSTFIX,
	PREFIX"femaleheadbreton_lips" POSTFIX,
	PREFIX"femaleheadbretonwarpaint_01" POSTFIX,
	PREFIX"femaleheadbretonwarpaint_02" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_01" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_02" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_03" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_04" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_05" POSTFIX,
	PREFIX"femaleheaddarkelfwarpaint_06" POSTFIX,
	PREFIX"femaleheaddirt_01" POSTFIX,
	PREFIX"femaleheaddirt_02" POSTFIX,
	PREFIX"femaleheaddirt_03" POSTFIX,
	PREFIX"femaleheadforsworntattoo_01" POSTFIX,
	PREFIX"femaleheadforsworntattoo_02" POSTFIX,
	PREFIX"femaleheadforsworntattoo_03" POSTFIX,
	PREFIX"femaleheadforsworntattoo_04" POSTFIX,
	PREFIX"femaleheadhighelf_lips" POSTFIX,
	PREFIX"femaleheadhighelfwarpaint_01" POSTFIX,
	PREFIX"femaleheadhighelfwarpaint_02" POSTFIX,
	PREFIX"femaleheadhighelfwarpaint_03" POSTFIX,
	PREFIX"femaleheadhighelfwarpaint_04" POSTFIX,
	PREFIX"femaleheadhuman_chin" POSTFIX,
	PREFIX"femaleheadhuman_forehead" POSTFIX,
	PREFIX"femaleheadhuman_neck" POSTFIX,
	PREFIX"femaleheadhuman_nose" POSTFIX,
	PREFIX"femaleheadimperial_lips" POSTFIX,
	PREFIX"femaleheadimperialwarpaint_01" POSTFIX,
	PREFIX"femaleheadimperialwarpaint_02" POSTFIX,
	PREFIX"femaleheadimperialwarpaint_03" POSTFIX,
	PREFIX"femaleheadimperialwarpaint_04" POSTFIX,
	PREFIX"femaleheadimperialwarpaint_05" POSTFIX,
	PREFIX"femaleheadnord_lips" POSTFIX,
	PREFIX"femaleheadnordwarpaint_01" POSTFIX,
	PREFIX"femaleheadnordwarpaint_02" POSTFIX,
	PREFIX"femaleheadnordwarpaint_03" POSTFIX,
	PREFIX"femaleheadnordwarpaint_04" POSTFIX,
	PREFIX"femaleheadnordwarpaint_05" POSTFIX,
	PREFIX"femaleheadorcwarpaint_01" POSTFIX,
	PREFIX"femaleheadorcwarpaint_02" POSTFIX,
	PREFIX"femaleheadorcwarpaint_03" POSTFIX,
	PREFIX"femaleheadorcwarpaint_04" POSTFIX,
	PREFIX"femaleheadorcwarpaint_05" POSTFIX,
	PREFIX"femaleheadredguard_lips" POSTFIX,
	PREFIX"femaleheadredguardwarpaint_01" POSTFIX,
	PREFIX"femaleheadredguardwarpaint_02" POSTFIX,
	PREFIX"femaleheadredguardwarpaint_03" POSTFIX,
	PREFIX"femaleheadredguardwarpaint_04" POSTFIX,
	PREFIX"femaleheadredguardwarpaint_05" POSTFIX,
	PREFIX"femaleheadwarpaint_01" POSTFIX,
	PREFIX"femaleheadwarpaint_02" POSTFIX,
	PREFIX"femaleheadwarpaint_03" POSTFIX,
	PREFIX"femaleheadwarpaint_04" POSTFIX,
	PREFIX"femaleheadwarpaint_05" POSTFIX,
	PREFIX"femaleheadwarpaint_06" POSTFIX,
	PREFIX"femaleheadwarpaint_07" POSTFIX,
	PREFIX"femaleheadwarpaint_08" POSTFIX,
	PREFIX"femaleheadwarpaint_09" POSTFIX,
	PREFIX"femaleheadwarpaint_10" POSTFIX,
	PREFIX"femaleheadwoodelfwarpaint_01" POSTFIX,
	PREFIX"femaleheadwoodelfwarpaint_02" POSTFIX,
	PREFIX"femaleheadwoodelfwarpaint_03" POSTFIX,
	PREFIX"femaleheadwoodelfwarpaint_04" POSTFIX,
	PREFIX"femaleheadwoodelfwarpaint_05" POSTFIX,
	PREFIX"femalelowereyesocket" POSTFIX,
	PREFIX"femalenordeyelinerstyle_01" POSTFIX,
	PREFIX"femaleuppereyesocket" POSTFIX,
	PREFIX"khajiitcheekcolor" POSTFIX,
	PREFIX"khajiitcheekcolorlower" POSTFIX,
	PREFIX"khajiitchin" POSTFIX,
	PREFIX"khajiitdirt" POSTFIX,
	PREFIX"khajiiteyeliner" POSTFIX,
	PREFIX"khajiiteyesocket01" POSTFIX,
	PREFIX"khajiiteyesocket02" POSTFIX,
	PREFIX"khajiiteyesocketlower" POSTFIX,
	PREFIX"khajiiteyesocketupper" POSTFIX,
	PREFIX"khajiitforehead" POSTFIX,
	PREFIX"khajiitlaughlines" POSTFIX,
	PREFIX"khajiitlipcolor" POSTFIX,
	PREFIX"khajiitneck" POSTFIX,
	PREFIX"khajiitnose01" POSTFIX,
	PREFIX"khajiitpaint01" POSTFIX,
	PREFIX"khajiitpaint02" POSTFIX,
	PREFIX"khajiitpaint03" POSTFIX,
	PREFIX"khajiitpaint04" POSTFIX,
	PREFIX"khajiitstripes01" POSTFIX,
	PREFIX"khajiitstripes02" POSTFIX,
	PREFIX"khajiitstripes03" POSTFIX,
	PREFIX"khajiitstripes04" POSTFIX,
	PREFIX"malehead_cheeks" POSTFIX,
	PREFIX"malehead_cheeks2" POSTFIX,
	PREFIX"malehead_frekles_01" POSTFIX,
	PREFIX"malehead_frownlines" POSTFIX,
	PREFIX"malehead_nose" POSTFIX,
	PREFIX"maleheadblackbloodtattoo_01" POSTFIX,
	PREFIX"maleheadblackbloodtattoo_02" POSTFIX,
	PREFIX"maleheadbothiahtattoo_01" POSTFIX,
	PREFIX"maleheadbretonwarpaint_01" POSTFIX,
	PREFIX"maleheadbretonwarpaint_02" POSTFIX,
	PREFIX"maleheadbretonwarpaint_03" POSTFIX,
	PREFIX"maleheadbretonwarpaint_04" POSTFIX,
	PREFIX"maleheadbretonwarpaint_05" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_01" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_02" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_03" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_04" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_05" POSTFIX,
	PREFIX"maleheaddarkelfwarpaint_06" POSTFIX,
	PREFIX"maleheaddirt_01" POSTFIX,
	PREFIX"maleheaddirt_02" POSTFIX,
	PREFIX"maleheaddirt_03" POSTFIX,
	PREFIX"maleheadforsworntattoo_01" POSTFIX,
	PREFIX"maleheadforsworntattoo_02" POSTFIX,
	PREFIX"maleheadforsworntattoo_03" POSTFIX,
	PREFIX"maleheadforsworntattoo_04" POSTFIX,
	PREFIX"maleheadhuman_chin" POSTFIX,
	PREFIX"maleheadhuman_forehead" POSTFIX,
	PREFIX"maleheadhuman_neck" POSTFIX,
	PREFIX"maleheadimperialwarpaint_01" POSTFIX,
	PREFIX"maleheadimperialwarpaint_02" POSTFIX,
	PREFIX"maleheadimperialwarpaint_03" POSTFIX,
	PREFIX"maleheadimperialwarpaint_04" POSTFIX,
	PREFIX"maleheadimperialwarpaint_05" POSTFIX,
	PREFIX"maleheadnord_lips" POSTFIX,
	PREFIX"maleheadnordwarpaint_01" POSTFIX,
	PREFIX"maleheadnordwarpaint_02" POSTFIX,
	PREFIX"maleheadnordwarpaint_03" POSTFIX,
	PREFIX"maleheadnordwarpaint_04" POSTFIX,
	PREFIX"maleheadnordwarpaint_05" POSTFIX,
	PREFIX"maleheadorcwarpaint_01" POSTFIX,
	PREFIX"maleheadorcwarpaint_02" POSTFIX,
	PREFIX"maleheadorcwarpaint_03" POSTFIX,
	PREFIX"maleheadorcwarpaint_04" POSTFIX,
	PREFIX"maleheadorcwarpaint_05" POSTFIX,
	PREFIX"maleheadredguard_lips" POSTFIX,
	PREFIX"maleheadredguardwarpaint_01" POSTFIX,
	PREFIX"maleheadredguardwarpaint_02" POSTFIX,
	PREFIX"maleheadredguardwarpaint_03" POSTFIX,
	PREFIX"maleheadredguardwarpaint_04" POSTFIX,
	PREFIX"maleheadredguardwarpaint_05" POSTFIX,
	PREFIX"maleheadwarpaint_01" POSTFIX,
	PREFIX"maleheadwarpaint_02" POSTFIX,
	PREFIX"maleheadwarpaint_03" POSTFIX,
	PREFIX"maleheadwarpaint_04" POSTFIX,
	PREFIX"maleheadwarpaint_05" POSTFIX,
	PREFIX"maleheadwarpaint_06" POSTFIX,
	PREFIX"maleheadwarpaint_07" POSTFIX,
	PREFIX"maleheadwarpaint_08" POSTFIX,
	PREFIX"maleheadwarpaint_09" POSTFIX,
	PREFIX"maleheadwarpaint_10" POSTFIX,
	PREFIX"maleheadwoodelfwarpaint_01" POSTFIX,
	PREFIX"maleheadwoodelfwarpaint_02" POSTFIX,
	PREFIX"maleheadwoodelfwarpaint_03" POSTFIX,
	PREFIX"maleheadwoodelfwarpaint_04" POSTFIX,
	PREFIX"maleheadwoodelfwarpaint_05" POSTFIX,
	PREFIX"malelowereyesocket" POSTFIX,
	PREFIX"maleuppereyesocket" POSTFIX,
	PREFIX"redguardmaleeyelinerstyle_01" POSTFIX,
	PREFIX"skintone" POSTFIX,
};

enum { NUM_TINT_MASK_TEXTURES = 190 };
STATIC_ASSERT(sizeof TintMaskTextures == NUM_TINT_MASK_TEXTURES * sizeof size_t);

UInt8 GetTintMaskTextureID(const char *path)
{
	if (path)
		for (size_t i = 0; i != NUM_TINT_MASK_TEXTURES; ++i)
		{
			if (!strcmp(path, TintMaskTextures[i]))
				return i;
		}
	return 0;
}

const char *GetTintMaskTexturePath(UInt8 tintMaskID)
{
	if (tintMaskID >= NUM_TINT_MASK_TEXTURES)
		tintMaskID = NULL;
	return TintMaskTextures[tintMaskID];
}

bool IsHeadPartValid(BGSHeadPart *headpart, TESRace *race)
{
	if (!headpart)
		return false;
	auto listForm = headpart->validRaces;
	if (!listForm)
		return false;
	using namespace ID_BGSListForm;
	switch (race ? race->formID : NULL)
	{
	case NULL:
		break;
	case HumanRace::ArgonianRace:
		switch (listForm->formID)
		{
		case HeadPartsArgonian:
		case HeadPartsArgonianandVampire:
			return true;
		}
		break;
	case HumanRace::BretonRace:
		switch (listForm->formID)
		{
		case HeadPartsAllRacesMinusBeast:
		case HeadPartsAllRacesMinusBeastVampires:
		case HeadPartsHuman:
		case HeadPartsHumansandVampires:
		case HeadPartsBretsNordsImpsandVampires:
		case HeadPartsBreton:
		case HeadPartsBretonandVampire:
			return true;
		}
		break;
	case HumanRace::DarkElfRace:
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
	case HumanRace::HighElfRace:
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
	case HumanRace::ImperialRace:
		switch (listForm->formID)
		{
		case HeadPartsAllRacesMinusBeast:
		case HeadPartsAllRacesMinusBeastVampires:
		case HeadPartsHuman:
		case HeadPartsHumansandVampires:
		case HeadPartsBretsNordsImpsandVampires:
		case HeadPartsImperial:
		case HeadPartsImperialandVampire:
			return true;
		}
		break;
	case HumanRace::KhajiitRace:
		switch (listForm->formID)
		{
		case HeadPartsKhajiit:
		case HeadPartsKhajiitandVampire:
			return true;
		}
		break;
	case HumanRace::NordRace:
		switch (listForm->formID)
		{
		case HeadPartsAllRacesMinusBeast:
		case HeadPartsAllRacesMinusBeastVampires:
		case HeadPartsHuman:
		case HeadPartsHumansandVampires:
		case HeadPartsBretsNordsImpsandVampires:
		case HeadPartsNord:
		case HeadPartsNordandVampire:
			return true;
		}
		break;
	case HumanRace::OrcRace:
		switch (listForm->formID)
		{
		case HeadPartsAllRacesMinusBeast:
		case HeadPartsAllRacesMinusBeastVampires:
		case HeadPartsOrc:
		case HeadPartsOrcandVampire:
			return true;
		}
		break;
	case HumanRace::RedguardRace:
		switch (listForm->formID)
		{
		case HeadPartsAllRacesMinusBeast:
		case HeadPartsAllRacesMinusBeastVampires:
		case HeadPartsHuman:
		case HeadPartsHumansandVampires:
		case HeadPartsRedguard:
		case HeadPartsRedguardandVampire:
			return true;
		}
		break;
	case HumanRace::WoodElfRace:
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
	sd::PrintNote("%u", listForm->formID);
	return false;
}

__forceinline void Warning(const char *msg)
{
	/*char buf[144];
	sprintf_s<sizeof buf>(buf, "WARNING: %s", msg);
	sd::PrintNote(buf);*/
}

void ClearPCTintMasks()
{
	auto pc = g_thePlayer;
	pc->tintMasks.clear();
}

using LookData = ci::LookData;

const LookData GetFromPlayerImpl()
{
	LookData result;

	auto pl = (TESNPC *)sd::GetPlayer()->baseForm;
	result.isFemale = sd::GetSex(pl) == 1;
	result.raceID = pl->race->formID;
	result.weight = UInt8(pl->weight) / 10;
	result.skinColor = { pl->color.red, pl->color.green, pl->color.blue, 0 };
	if (pl->headData && pl->headData->hairColor)
		result.hairColor = { pl->headData->hairColor->color.red, pl->headData->hairColor->color.green, pl->headData->hairColor->color.blue, pl->headData->hairColor->color.alpha };
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
	PlayerCharacter *pc = g_thePlayer;
	const UInt32 count = pc->tintMasks.size();
	for(auto it = pc->tintMasks.begin(); it != pc->tintMasks.end(); ++it)
	{
		LookData::TintMask m;
		std::string texture_path = (*it)->texture->str.c_str();
		std::transform(texture_path.begin(), texture_path.end(), texture_path.begin(), ::tolower);
		m.alpha = (*it)->alpha;
		m.color = { (*it)->color.red, (*it)->color.green, (*it)->color.blue, (*it)->color.alpha };
		m.tintMaskTextureID = GetTintMaskTextureID(texture_path.data());
		m.tintType = (*it)->tintType;
		result.tintmasks.push_back(m);
	}

	//result.skin_color.green = 255;
	return result;
}

LookData g_pcLookData;

const LookData GetFromPlayer()
{
	if (g_pcLookData.isEmpty())
		g_pcLookData = GetFromPlayerImpl();
	return g_pcLookData;
}

void ApplyHeadparts(TESNPC *npc, const std::vector<UInt32> &headpart_IDs)
{
	if (!npc)
		return;
	const auto headpart_count = headpart_IDs.size();
	if (headpart_count >= 6 && headpart_count <= 10)
	{
		const bool isFemale = sd::GetSex(npc) == 1;
		bool hasFace = false, hasEyes = false;
		std::vector<BGSHeadPart *> validHeadParts;
		for (auto it = headpart_IDs.begin(); it != headpart_IDs.end(); ++it)
		{
			const UInt32 headpart_id = *it;
			auto headpart = (BGSHeadPart *)LookupFormByID(headpart_id);
			if (headpart && headpart->formType == (FormType)BGSHeadPart::kTypeID && headpart->validRaces)
			{
				bool valid = IsHeadPartValid(headpart, npc->race);

				auto isFlagSet = [](BGSHeadPart *h, UInt32 flag) {
					return h && (h->partFlags & flag) == flag;
				};

				if ((isFlagSet(headpart, BGSHeadPart::kFlagMale) && !isFlagSet(headpart, BGSHeadPart::kFlagFemale) && isFemale)
					|| (isFlagSet(headpart, BGSHeadPart::kFlagFemale) && !isFlagSet(headpart, BGSHeadPart::kFlagMale) && !isFemale))
					valid = false;

				if (!valid)
				{
					Warning("Invalid HeadPart");
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
	auto factory = IFormFactory::GetFactoryForType((FormType)BGSColorForm::kTypeID);
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
			if (npc->race && npc->race->chargenData && npc->race->chargenData[isFemale] && npc->race->chargenData[isFemale]->textureSet && npc->race->chargenData[isFemale]->colors)
			{
				auto &textureSet = *npc->race->chargenData[isFemale]->textureSet;
				auto &colors = *npc->race->chargenData[isFemale]->colors;

				bool success = false;
				for (size_t i = 0; i < colors.size(); ++i)
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
					if (colors.size())
						npc->headData->hairColor = defaultColor ? defaultColor : colors[0];
					else if (npc->headData->hairColor)
						npc->headData->hairColor->color = { 0,0,0,0 };
				}

				if (texture_set_id)
				{
					bool exist = false;
					auto resultTextureSet = (BGSTextureSet *)LookupFormByID(texture_set_id);
					if (resultTextureSet && resultTextureSet->formType == (FormType)BGSTextureSet::kTypeID)
						for (size_t i = 0; i < textureSet.size(); ++i)
							if (textureSet[i] && textureSet[i]->formType == (FormType)BGSTextureSet::kTypeID && textureSet[i]->formID == texture_set_id)
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

void ApplyFaceMorph(TESNPC *npc, decltype(LookData::presets) presets, decltype(LookData::options) options)
{
	if (!npc->faceMorph)
		npc->faceMorph = new TESNPC::FaceMorphs;
	std::copy(presets.begin(), presets.end(), npc->faceMorph->presets);
	std::copy(options.begin(), options.end(), npc->faceMorph->option);
}

void ApplyTintMasks(TESNPC *npc, const std::vector<LookData::TintMask> &result_tintmasks)
{
	static SInt64 current_timers = 0;

	const auto pc = g_thePlayer;
	const bool isPlayer =
		(npc == pc->baseForm);
	const bool isFemale = sd::GetSex(npc) == 1;

	if (result_tintmasks.size() >= 64)
	{
		Warning("Too many TintMasks");
		goto fail;
	}

	if (npc->race && npc->race->chargenData && npc->race->chargenData[isFemale])
	{
		auto &chargenData = *npc->race->chargenData[isFemale];
		if (chargenData.tintData)
		{
			auto &tintData = *chargenData.tintData;

			const size_t tintmask_count = result_tintmasks.size();
			pc->tintMasks.reserve(tintmask_count);
			{
				bool any_error = false;
				TintMask *skintone = nullptr;
				for (size_t i = 0; i != tintmask_count; ++i)
				{
					auto _new = new TintMask;
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
					for (size_t j = 0; j != tintData.size(); ++j)
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
							for (size_t j = 0; j != colors.size(); ++j)
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

					if (!i && !skintone && std::string(_new->texture->str.c_str()).find("SkinTone") != std::string::npos)
						skintone = _new;
					pc->tintMasks[i] = _new;
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
						else
						{
							//Warning("Skintone TintMask color conflicts with body color"); 
							// ^ Ложные срабатывания на аргониан, котов без каких-либо видимых последствий
							npc->color.red = skintone->color.red;
							npc->color.green = skintone->color.green;
							npc->color.blue = skintone->color.blue;
							NiColorA colorResult;
							//CALL_MEMBER_FN(npc, SetSkinFromTint)(&colorResult, skintone, 1, 0);
							npc->SetSkinFromTint(&colorResult, skintone, NULL, NULL);
						}
						if (!isPlayer)
						{
							const UInt32 real_formID = npc->formID;
							///npc->formID = sd::GetPlayer()->baseForm->formID;
							current_timers++;
							SET_TIMER(350, [=] {
								//npc->formID = real_formID;
								current_timers--;
							});
						}
					}
					else
					{
						Warning("Unable to find skintone TintMask");
						goto fail;
					}
				}
			}
			return;
		}
		return Warning("Unable to get TintMask data");
	}
	return Warning("Unable to get any CharGen data");

fail:
	if (npc->race && npc->race->chargenData && npc->race->chargenData[isFemale])
	{
		auto &chargenData = *npc->race->chargenData[isFemale];
		if (chargenData.presets && chargenData.presets->size())
		{
			auto preset = chargenData.presets->operator[](0);
			if (preset && preset->formType == (FormType)TESNPC::kTypeID)
			{
				LookData::TintMask tM;
				tM.tintMaskTextureID = GetTintMaskTextureID(PREFIX "skintone" POSTFIX);
				tM.alpha = 1;
				npc->color.red = tM.color.r = preset->color.red;
				npc->color.green = tM.color.g = preset->color.green;
				npc->color.blue = tM.color.b = preset->color.blue;
				tM.color.a = 0;
				tM.tintType = TintMask::kMaskType_SkinTone;

				static UInt8 ApplyTintMasks_Calls = 0;
				if (ApplyTintMasks_Calls++ < 3)
				{
					ClearPCTintMasks();
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

std::function<void(void)> after_apply;

void ApplyBodyData(TESNPC *npc, Actor *actor, UInt32 result_raceID, UInt8 isFemale, UInt8 weight, TESNPC::Color color)
{
	if (weight > 10 || weight < 0)
	{
		weight = 10;
		Warning("Invalid weight");
	}
	if (isFemale > TRUE)
	{
		isFemale = FALSE;
		Warning("Invalid sex");
	}

	if (result_raceID < HumanRace::RaceMIN || result_raceID > HumanRace::RaceMAX)
	{
		result_raceID = HumanRace::NordRace;
		Warning("Invalid race");
	}
	auto race = (TESRace *)LookupFormByID(result_raceID);
	if (race && race->formType == (FormType)TESRace::kTypeID)
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

	*((uint32_t *)&npc->TESActorBaseData::flags) = 48u + !!isFemale;
	/*std::bitset<32> flags = ((TESNPC *)sd::GetPlayer()->baseForm)->actorData.flags;
	flags[2] = false; // is ghost
	flags[31] = !!isFemale;
	npc->actorData.flags = flags.to_ullong();*/

	npc->weight = new_weight;
	npc->sleepOutfit = npc->defaultOutfit = nullptr;
	npc->height = 1;

	if (actor && actor->baseForm == npc)
	{
		const UInt32 refID = actor->formID;
		//Timers::GetSingleton()->SetTimer(0, 
		after_apply =
			[refID, original_weight, new_weight, isPlayer, isFemale, isFemale_old] {
			auto actor = (Actor *)LookupFormByID(refID);
			if (actor && actor->formType == (FormType)Actor::kTypeID)
			{
				const auto neck_delta = (original_weight / 100.f) - (new_weight / 100.f);
				if (neck_delta)
					cd::UpdateWeight(actor, neck_delta);
				else
					cd::QueueNiNodeUpdate(actor);
				if (isPlayer)
				{
					if (isFemale_old != isFemale)
						sd::Resurrect(actor);
				}
			}
		};
		//);
	}
}

const void ApplyImpl(LookData result, TESNPC *npc, Actor *actor = nullptr)
{
	if (actor && actor->baseForm != npc)
		actor = nullptr;
	const bool isPlayer =
		(npc == sd::GetPlayer()->baseForm);

	TESNPC::Color skinColor;
	skinColor.red = result.skinColor.r;
	skinColor.green = result.skinColor.g;
	skinColor.blue = result.skinColor.b;

	BGSColorForm::Color hairColor;
	hairColor.red = result.hairColor.r;
	hairColor.green = result.hairColor.g;
	hairColor.blue = result.hairColor.b;

	ApplyBodyData(npc, actor, result.raceID, result.isFemale, result.weight, skinColor);
	ApplyHeadparts(npc, result.headpartIDs);
	ApplyHeadData(npc, hairColor, result.headTextureSetID);
	ApplyFaceMorph(npc, result.presets, result.options);
	ApplyTintMasks(npc, result.tintmasks);

	if (after_apply)
	{
		auto _after_apply = after_apply;
		SET_TIMER(1000 , [=] {
			_after_apply();
		});
		after_apply = decltype(after_apply)();
	}
}

void ShowRaceMenuSafe()
{
	sd::Wait(50);
	ClearPCTintMasks();
	sd::ShowRaceMenu();

	std::thread([] {
		auto mm = MenuManager::GetSingleton();
		bool was_open = false;
		while (1)
		{
			bool open = mm->IsMenuOpen("RaceSex Menu");
			if (open)
				was_open = true;
			if (!open && was_open)
				break;
			Sleep(1);
		}
		return SET_TIMER(0, [] {
			g_pcLookData = GetFromPlayerImpl();
			return SET_TIMER(500, [] {
				auto pc = sd::GetPlayer();
				cd::SendAnimationEvent(pc, "Skymp_Register");
			});
		});
	}).detach();
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
					if (npc->race->formID >= RaceMIN && npc->race->formID <= RaceMAX && npc->TESActorBaseData::flags.female == !!isFemale && npc->TESActorBaseData::flags.ghost == false)
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

void Apply(Actor *ac, LookData r)
{
	auto pc = sd::GetPlayer();
	if (ac == pc)
	{
		g_pcLookData = r;
	}
	ApplyImpl(r, (TESNPC *)ac->baseForm, ac);
}

ILookSynchronizer *ILookSynchronizer::GetV16()
{
	class V16 : public ILookSynchronizer
	{
	public:
		TESNPC *Apply(const ci::LookData &lookData) override {
			return ::Apply(lookData);
		}
		void Apply(const ci::LookData &lookData, Actor *actor) override {
			return ::Apply(actor, lookData);
		}
		void ApplyTintMasks(TESNPC *npc, const std::vector<ci::LookData::TintMask> &result_tintmasks) override {
			return ::ApplyTintMasks(npc, result_tintmasks);
		}
		ci::LookData GetFromPlayer(bool noCache = false) override {
			return ::GetFromPlayer();
		}
		ci::LookData GetFromNPC(TESNPC *npc) override {
			return {};
		}
		void ShowRaceMenu() override {
			return ::ShowRaceMenuSafe();
		}
	};

	static auto synchronizer = new V16;
	return synchronizer;
}