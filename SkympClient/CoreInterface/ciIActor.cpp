#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "CoreInterface.h"

void ci::IActor::SetWerewolf(bool isWerewolf, bool skipAnim)
{
	const auto refID = this->GetRefID();
	if (refID == 0)
		return;
	Werewolf::SetWerewolf(refID, isWerewolf, skipAnim);
}