#pragma once

class Tests
{
public:
	static void Run()
	{
		TestCyrillic();
		TestPerkRequirementsList();
	}

private:

	static void TestPerkRequirementsList()
	{
		auto perk = new ci::Perk(0x00079345);
		auto list = perk->GetLocalRequirements();
		std::vector<uint32_t> ids;
		for (auto rp : list)
			ids.push_back(rp->GetFormID());
		if (ids != std::vector<uint32_t>{0xbabe4, 0x79343, 0x79342, 0x79344, 0x79345})
		{
			std::wstringstream wss;
			wss << L"ERROR:ClientLogic Test failed - TestPerkRequirementsList() ";
			for (auto id : ids)
				wss << std::hex << "0x" << id << " ";
			ci::Log(wss.str());
			ExitWithError();
		}
	}

	static void TestCyrillic()
	{
		auto dcd = decodeRu(L"<cy:16><cy:31><cy:15>");
		if (dcd != L"рст")
		{
			ci::Log(L"ERROR:ClientLogic Test failed - decodeRu() " + dcd);
			ExitWithError();
		}

		auto ecd = encodeRu(L"рст");
		if (ecd != "<cy:16><cy:31><cy:15>")
		{
			ci::Log("ERROR:ClientLogic Test failed - encodeRu() " + ecd);
			ExitWithError();
		}
	}

	static void ExitWithError()
	{
		ci::Dialog::Show(L" ", ci::Dialog::Style::Message, L"CLIENT LOGIC\nFATAL: TEST FAILED", -1, [](ci::Dialog::Result) {
			ci::SetTimer(1, [] {
				std::exit(-1); 
			});
		});
	}
};