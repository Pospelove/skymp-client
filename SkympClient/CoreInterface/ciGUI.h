#pragma once
#include <string>
#include <ostream>
#include <functional>

namespace ci
{
	namespace Chat
	{
		void Init();
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

		void SetText(const std::wstring &str);
		void SetPos(const NiPoint3 &pos);

		std::wstring GetText() const;
		NiPoint3 GetPos() const;

	private:
		Text3D(const Text3D &) = delete;
		Text3D & operator=(const Text3D &) = delete;

		struct Impl;
		Impl *pimpl;
	};
}