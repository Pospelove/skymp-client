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

	dlf_mutex recipeM{"ci_recipe"};

	using KeywordBackup = std::map<BGSConstructibleObject *, BGSKeyword *>;
	std::shared_ptr<KeywordBackup> keywordBackup;

	class NativeConstructibleObjectDeleter
	{
	public:
		NativeConstructibleObjectDeleter()
		{
			auto kb = new KeywordBackup;
			const uint32_t begin = 0x000DD992,
				end = 0x0010C434 + 1 +(0x01000000);
			this->deletions.reserve(end - begin);
			for (auto id = begin; id != end; ++id)
			{
				auto form = (BGSConstructibleObject *)LookupFormByID(id);
				if (form && form->formType == FormType::ConstructibleObject)
				{
					static auto invalidKeyword = Utility::FindKeyword("MQKillDragonKeyword");
					(*kb)[form] = form->wbKeyword;
					form->wbKeyword = invalidKeyword;
					this->deletions.push_back(form);
				}
			}
			ci::keywordBackup.reset(kb);
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

		LABEL_TRY_AGAIN:
		static int32_t offset = 0;
		TESForm *srcRecipe;
		try {
			srcRecipe = deleter->GetDeletedRecipes().at(offset++);
		}
		catch (...) {
			srcRecipe = nullptr;
		}

		if (!srcRecipe)
		{
			ErrorHandling::SendError("ERROR:Recipe Source recipes overflow");
			offset = 0;
			goto LABEL_TRY_AGAIN;
		}

		/*pimpl->recipe = FormHeap_Allocate<BGSConstructibleObject>();
		memcpy(pimpl->recipe, srcRecipe, sizeof BGSConstructibleObject);
		pimpl->recipe->formID = 0;
		pimpl->recipe->SetFormID(Utility::NewFormID(), true);*/
		pimpl->recipe = (BGSConstructibleObject *)srcRecipe;

		if (createdItemType != nullptr)
			pimpl->recipe->createdObject = LookupFormByID(createdItemType->GetFormID());
		pimpl->recipe->quantity = createdItemCount;
		if (nullptr == (pimpl->recipe->wbKeyword = Utility::FindKeyword(workbenchKeyword)))
			if ("VI" != workbenchKeyword)
				ErrorHandling::SendError("ERROR:Recipe Bad keyword %s", workbenchKeyword.data());
		pimpl->recipe->unk20 = (int32_t)nullptr; // conditions ?

		pimpl->recipe->container.entries = nullptr;
		pimpl->recipe->container.numEntries = 0;
	}

	Recipe::~Recipe()
	{
		std::lock_guard<dlf_mutex> l(recipeM);

		static auto invalidKeyword = Utility::FindKeyword("MQKillDragonKeyword");
		pimpl->recipe->wbKeyword = invalidKeyword;

		this->RemoveAllRecipeComponents();

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

	void Recipe::RemoveAllRecipeComponents()
	{
		std::lock_guard<dlf_mutex> l(recipeM);
		if (pimpl->recipe->container.entries != nullptr)
		{
			delete[] pimpl->recipe->container.entries;
			pimpl->recipe->container.entries = nullptr;
			pimpl->recipe->container.numEntries = 0;
		}
	}

	void Recipe::SetPlayerKnows(bool knows)
	{
		std::lock_guard<dlf_mutex> l(recipeM);

		static std::map<Recipe *, BGSKeyword *> keywordBackup;
		static std::set<Recipe *> unknownRecipes;

		const bool knowsWas = unknownRecipes.count(this) == 0;

		if (knowsWas != knows)
		{
			if (knows)
			{
				pimpl->recipe->wbKeyword = keywordBackup[this];
				keywordBackup.erase(this);
				unknownRecipes.erase(this);
			}
			else
			{
				keywordBackup.insert({ this, pimpl->recipe->wbKeyword });
				unknownRecipes.insert(this);
				static auto invalidKeyword = Utility::FindKeyword("MQKillDragonKeyword");
				pimpl->recipe->wbKeyword = invalidKeyword;
			}
		}
	}
}