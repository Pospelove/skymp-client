#pragma once

inline void Serialize(RakNet::BitStream &bsOut, const ci::MovementData &d)
{
	bsOut.Write(d.aimingAngle);
	bsOut.Write(d.attackState);
	bsOut.Write(d.pos.x);
	bsOut.Write(d.pos.y);
	bsOut.Write(d.pos.z);
	bsOut.Write(d.angleZ);
	bsOut.Write(d.direction);
	bsOut.Write(d.speedSampled);
	bsOut.Write(d.runMode);
	bsOut.Write(d.jumpStage);
	bsOut.Write(d.isInJumpState);
	bsOut.Write(d.isJumping);
	bsOut.Write(d.isSprinting);
	bsOut.Write(d.isSneaking);
	bsOut.Write(d.isWeapDrawn);
	bsOut.Write(d.isBlocking);
	bsOut.Write(d.isFirstPerson);
	bsOut.Write(d.isSwimming);
}

inline void Deserialize(RakNet::BitStream &bsIn, ci::MovementData &d)
{
	bsIn.Read(d.aimingAngle);
	bsIn.Read(d.attackState);
	bsIn.Read(d.pos.x);
	bsIn.Read(d.pos.y);
	bsIn.Read(d.pos.z);
	bsIn.Read(d.angleZ);
	bsIn.Read(d.direction);
	bsIn.Read(d.speedSampled);
	bsIn.Read(d.runMode);
	bsIn.Read(d.jumpStage);
	bsIn.Read(d.isInJumpState);
	bsIn.Read(d.isJumping);
	bsIn.Read(d.isSprinting);
	bsIn.Read(d.isSneaking);
	bsIn.Read(d.isWeapDrawn);
	bsIn.Read(d.isBlocking);
	bsIn.Read(d.isFirstPerson);
	bsIn.Read(d.isSwimming);
}

inline void Serialize(RakNet::BitStream &bsOut, const ci::LookData &d)
{
	bsOut.Write(d.isFemale);
	bsOut.Write(d.raceID);
	bsOut.Write(d.weight);
	bsOut.Write(d.skinColor.r);
	bsOut.Write(d.skinColor.g);
	bsOut.Write(d.skinColor.b);
	bsOut.Write(d.skinColor.a);
	bsOut.Write(d.hairColor.r);
	bsOut.Write(d.hairColor.g);
	bsOut.Write(d.hairColor.b);
	bsOut.Write(d.hairColor.a);
	
	bsOut.Write((uint16_t)d.headpartIDs.size());
	for (auto it = d.headpartIDs.begin(); it != d.headpartIDs.end(); ++it)
		bsOut.Write(*it);

	bsOut.Write((uint16_t)d.tintmasks.size());
	for (auto it = d.tintmasks.begin(); it != d.tintmasks.end(); ++it)
	{
		bsOut.Write(it->alpha);
		bsOut.Write(it->color.r);
		bsOut.Write(it->color.g);
		bsOut.Write(it->color.b);
		bsOut.Write(it->color.a);
		bsOut.Write(it->tintMaskTextureID);
		bsOut.Write(it->tintType);
	}

	for (int32_t i = 0; i != d.options.size(); ++i)
		bsOut.Write(d.options[i]);

	for (int32_t i = 0; i != d.presets.size(); ++i)
		bsOut.Write(d.presets[i]);

	bsOut.Write(d.headTextureSetID);
}


inline void Deserialize(RakNet::BitStream &bsIn, ci::LookData &d)
{
	bsIn.Read(d.isFemale);
	bsIn.Read(d.raceID);
	bsIn.Read(d.weight);
	bsIn.Read(d.skinColor.r);
	bsIn.Read(d.skinColor.g);
	bsIn.Read(d.skinColor.b);
	bsIn.Read(d.skinColor.a);
	bsIn.Read(d.hairColor.r);
	bsIn.Read(d.hairColor.g);
	bsIn.Read(d.hairColor.b);
	bsIn.Read(d.hairColor.a);

	uint16_t headpartIDsSize;
	bsIn.Read(headpartIDsSize);
	if (headpartIDsSize > 64)
		headpartIDsSize = 64;

	d.headpartIDs = {};
	for (size_t i = 0; i != headpartIDsSize; ++i)
	{
		uint32_t headpartID;
		bsIn.Read(headpartID);
		d.headpartIDs.push_back(headpartID);
	}

	uint16_t tintmasksSize;
	bsIn.Read(tintmasksSize);
	if (tintmasksSize > 64)
		tintmasksSize = 64;

	d.tintmasks = {};
	for (size_t i = 0; i != tintmasksSize; ++i)
	{
		ci::LookData::TintMask tm;
		bsIn.Read(tm.alpha);
		bsIn.Read(tm.color.r);
		bsIn.Read(tm.color.g);
		bsIn.Read(tm.color.b);
		bsIn.Read(tm.color.a);
		bsIn.Read(tm.tintMaskTextureID);
		bsIn.Read(tm.tintType);
		d.tintmasks.push_back(tm);
	}

	for (int32_t i = 0; i != d.options.size(); ++i)
		bsIn.Read(d.options[i]);

	for (int32_t i = 0; i != d.presets.size(); ++i)
		bsIn.Read(d.presets[i]);

	bsIn.Read(d.headTextureSetID);

}