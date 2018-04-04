#pragma once
#include <string>
#include <ostream>
#include <functional>

namespace ci
{
	namespace Chat
	{
		void Init(int32_t offsetX, int32_t offsetY);
		void SetRussianUser();

		// pass " " to clear the chat
		void AddMessage(const std::wstring &message, bool isNotification = false);
	}

	namespace Dialog
	{
		struct Result
		{
			std::wstring inputText;
			int32_t listItem;
		};

		enum class Style : uint8_t
		{
			Message = 0,
			Input = 1,
			List = 2,
		};

		using Callback = std::function<void(Result)>;

		void Show(const std::wstring &title, Style dialogStyle, const std::wstring &text, int32_t defaultIndex, Callback callback);
		bool Hide();
	}

	class Text3D
	{
	public:
		Text3D(const std::wstring &str, NiPoint3 pos);
		~Text3D();

		void SetText(std::wstring str);
		void SetPos(const NiPoint3 &pos);

		void SetPosSource(std::function<NiPoint3()> fn);
		void SetTextSource(std::function<std::wstring()> fn);

		void SetFontHeight(uint32_t height);

		std::wstring GetText() const;
		NiPoint3 GetPos() const;

		void SetVisible(bool v);
		bool IsVisible() const;

		void SetDrawDistance(float distance);
		float GetDrawDistance() const;

	private:
		Text3D(const Text3D &) = delete;
		Text3D & operator=(const Text3D &) = delete;

		struct Impl;
		Impl *pimpl;
	};
}