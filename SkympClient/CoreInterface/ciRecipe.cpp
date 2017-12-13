#include "../stdafx.h"
#include "CoreInterface.h"

namespace ci
{
	struct RecipeEntry
	{
		ci::ItemType *itemType;
		uint32_t count;
	};

	struct Recipe::Impl
	{
		BGSConstructibleObject *recipe = nullptr;
		std::vector<RecipeEntry> entries;
	};

	dlf_mutex recipeM;

	class NativeConstructibleObjectDeleter
	{
	public:
		NativeConstructibleObjectDeleter()
		{
			const uint32_t begin = 0x000DD992,
				end = 0x0010C434 + 1;
			this->deletions.reserve(end - begin);
			for (auto id = begin; id != end; ++id)
			{
				auto form = (BGSConstructibleObject *)LookupFormByID(id);
				if (form && form->formType == FormType::ConstructibleObject)
				{
					static auto invalidKeyword = Utility::FindKeyword("MQKillDragonKeyword");
					form->wbKeyword = invalidKeyword;
					this->deletions.push_back(form);
				}
			}
		}

		using Deletions = std::vector<BGSConstructibleObject *>;

		const Deletions &GetDeletedRecipes() {
			return this->deletions;
		}

	private:
		Deletions deletions;
	};

	Recipe::Recipe(const std::string &workbenchKeyword, ci::ItemType *createdItemType, uint32_t createdItemCount) : pimpl(new Impl)
	{
		std::lock_guard<dlf_mutex> l(recipeM);

		static auto deleter = new NativeConstructibleObjectDeleter;

		if (deleter->GetDeletedRecipes().size() == 0)
		{
			ErrorHandling::SendError("ERROR:Recipe Bad deleter");
			throw ~0;
		}

		auto srcRecipe = deleter->GetDeletedRecipes().front();

		pimpl->recipe = FormHeap_Allocate<BGSConstructibleObject>();
		memcpy(pimpl->recipe, srcRecipe, sizeof BGSConstructibleObject);
		pimpl->recipe->formID = 0;
		pimpl->recipe->SetFormID(Utility::NewFormID(), true);

		pimpl->recipe->createdObject = LookupFormByID(createdItemType->GetFormID());
		pimpl->recipe->quantity = createdItemCount;
		pimpl->recipe->wbKeyword = Utility::FindKeyword(workbenchKeyword);
		pimpl->recipe->unk20 = (int32_t)nullptr; // conditions ?

		pimpl->recipe->container.entries = nullptr;
		pimpl->recipe->container.numEntries = 0;
	}

	Recipe::~Recipe()
	{
		std::lock_guard<dlf_mutex> l(recipeM);

		delete pimpl;
	}

	void Recipe::AddRecipeComponent(ci::ItemType *component, uint32_t count)
	{
		if (component == nullptr || count == 0)
			return;

		std::lock_guard<dlf_mutex> l(recipeM);

		pimpl->entries.push_back({ component, count });

		const size_t maxSize = 1024;
		if (pimpl->recipe->container.entries == nullptr)
		{
			pimpl->recipe->container.entries = new TESContainer::Entry *[maxSize];
			for (auto i = 0; i != maxSize; ++i)
			{
				pimpl->recipe->container.entries[i] = nullptr;
			}
		}

		auto &num = pimpl->recipe->container.numEntries;

		if (num == maxSize - 1)
		{
			ErrorHandling::SendError("ERROR:Recipe Container overflow");
			return;
		}

		auto newEntry = new TESContainer::Entry;
		pimpl->recipe->container.entries[num] = newEntry;
		newEntry->count = count;
		newEntry->form = LookupFormByID(component->GetFormID());
		newEntry->data = nullptr;
		++num;

	}
}