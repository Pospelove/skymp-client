#pragma once

namespace ci
{
	class Recipe
	{
	public:
		Recipe(const std::string &workbenchKeyword, ci::ItemType *createdItemType, uint32_t createdItemCount = 1);
		~Recipe();
		void AddRecipeComponent(ci::ItemType *component, uint32_t count);

	private:
		struct Impl;
		Impl *const pimpl;

		void PrepareContainer();
	};
}