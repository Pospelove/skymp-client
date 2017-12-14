#pragma once

namespace ci
{
	class Recipe
	{
	public:
		Recipe(const std::string &workbenchKeyword, ci::ItemType *createdItemType, uint32_t createdItemCount = 1);
		~Recipe();
		void AddRecipeComponent(ci::ItemType *component, uint32_t count);
		void RemoveAllRecipeComponents();
		void SetPlayerKnows(bool knows);

	private:
		struct Impl;
		Impl *const pimpl;
	};
}