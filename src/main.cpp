//
// Created by PinkySmile on 31/10/2020
//

#include <SokuLib.hpp>

static bool init = false;
static int (SokuLib::BattleManager::*ogBattleMgrOnProcess)();
static void (SokuLib::BattleManager::*ogBattleMgrOnRender)();
static SokuLib::DrawUtils::Sprite text;
static SokuLib::SWRFont font;

int __fastcall CBattleManager_OnRender(SokuLib::BattleManager *This)
{
	(This->*ogBattleMgrOnRender)();
	text.draw();
	return 0;
}

void loadFont()
{
	SokuLib::FontDescription desc;

	// Pink
	desc.r1 = 255;
	desc.g1 = 155;
	desc.b1 = 155;
	// Light green
	desc.r2 = 155;
	desc.g2 = 255;
	desc.b2 = 155;
	desc.height = 24;
	desc.weight = FW_BOLD;
	desc.italic = 0;
	desc.shadow = 4;
	desc.bufferSize = 1000000;
	desc.charSpaceX = 0;
	desc.charSpaceY = 0;
	desc.offsetX = 0;
	desc.offsetY = 0;
	desc.useOffset = 0;
	strcpy(desc.faceName, "MonoSpatialModSWR");
	font.create();
	font.setIndirect(desc);
}

int __fastcall CBattleManager_OnProcess(SokuLib::BattleManager *This)
{
	if (!init) {
		SokuLib::Vector2i realSize;

		loadFont();
		text.texture.createFromText("Hello, world!", font, {300, 300}, &realSize);
		text.setPosition(SokuLib::Vector2i{320 - realSize.x / 2, 240 - realSize.y / 2});
		text.setSize(realSize.to<unsigned>());
		text.rect.width = realSize.x;
		text.rect.height = realSize.y;
		init = true;
	}
	text.setRotation(text.getRotation() + 0.01f);
	return (This->*ogBattleMgrOnProcess)();
}

// We check if the game version is what we target (in our case, Soku 1.10a).
extern "C" __declspec(dllexport) bool CheckVersion(const BYTE hash[16])
{
	return memcmp(hash, SokuLib::targetHash, sizeof(SokuLib::targetHash)) == 0;
}

// Called when the mod loader is ready to initialize this module.
// All hooks should be placed here. It's also a good moment to load settings from the ini.
extern "C" __declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule)
{
	DWORD old;

#ifdef _DEBUG
	FILE *_;

	AllocConsole();
	freopen_s(&_, "CONOUT$", "w", stdout);
	freopen_s(&_, "CONOUT$", "w", stderr);
#endif

	puts("Hello, world!");
	VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, PAGE_EXECUTE_WRITECOPY, &old);
	ogBattleMgrOnRender  = SokuLib::TamperDword(&SokuLib::VTable_BattleManager.onRender,  CBattleManager_OnRender);
	ogBattleMgrOnProcess = SokuLib::TamperDword(&SokuLib::VTable_BattleManager.onProcess, CBattleManager_OnProcess);
	VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, old, &old);

	FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
	return true;
}

extern "C" int APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	return TRUE;
}

// New mod loader functions
// Loading priority. Mods are loaded in order by ascending level of priority (the highest first).
// When 2 mods define the same loading priority the loading order is undefined.
extern "C" __declspec(dllexport) int getPriority()
{
	return 0;
}

// Not yet implemented in the mod loader, subject to change
// SokuModLoader::IValue **getConfig();
// void freeConfig(SokuModLoader::IValue **v);
// bool commitConfig(SokuModLoader::IValue *);
// const char *getFailureReason();
// bool hasChainedHooks();
// void unHook();