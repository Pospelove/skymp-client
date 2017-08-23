#include "CoreInterface.h"

namespace ci
{
	bool operator==(const LookData &l, const LookData &r)
	{
		if (&l == &r)
			return true;
		return !memcmp(&l.hairColor, &r.hairColor, sizeof (l.hairColor))
			&& l.headpartIDs == r.headpartIDs
			&& l.headTextureSetID == r.headTextureSetID
			&& l.isFemale == r.isFemale
			&& l.options == r.options
			&& l.presets == r.presets
			&& l.raceID == r.raceID
			&& !memcmp(&l.skinColor, &r.skinColor, sizeof (l.skinColor))
			&& l.tintmasks == r.tintmasks
			&& l.weight == r.weight;
	}

	bool operator!=(const LookData &l, const LookData &r)
	{
		return !(l == r);
	}

	bool operator==(const LookData::TintMask &l, const LookData::TintMask &r)
	{
		if (&l == &r)
			return true;
		return l.alpha == r.alpha
			&& !memcmp(&l.color, &r.color, sizeof r.color)
			&& l.tintMaskTextureID == r.tintMaskTextureID
			&& l.tintType == r.tintType;
	}

	bool operator!=(const LookData::TintMask &l, const LookData::TintMask &r)
	{
		return !(l == r);
	}
}