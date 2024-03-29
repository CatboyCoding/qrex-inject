#include <Windows.h>
#include "GLua.h"
#include <string>

ILuaShared* LuaShared = NULL;
ILuaInterface* qrex = NULL;

HMODULE module = NULL;

int destroyThread()
{
	FreeLibraryAndExitThread(module, 0);
}

int initDestroy()
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)destroyThread, NULL, NULL, NULL);
	return 0;
}

int getClientState() {
	HMODULE LuaShared_modhandle = GetModuleHandle("lua_shared.dll");
	if (LuaShared_modhandle != NULL)
	{
		typedef void* (*CreateInterfaceFn)(const char* name, int* returncode);
		CreateInterfaceFn LuaShared_createinter = (CreateInterfaceFn)GetProcAddress(LuaShared_modhandle, "CreateInterface");
		if (LuaShared_createinter != NULL)
		{
			LuaShared = (ILuaShared*)LuaShared_createinter(LUASHARED_INTERFACE_VERSION, NULL);
		}
	}
	if (LuaShared == NULL)
	{
		MessageBox(NULL, "[Qrextomnia] LuaShared not initialized", NULL, MB_OK);
		return 1;
	}
	qrex = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	if (qrex == NULL) {
		MessageBox(NULL, "[Qrextomnia] Lua State not initialized", NULL, MB_OK);
		return 1;
	}
	return 0;
}

int Main(lua_State * state)
{
	getClientState();
	qrex->RunString("RunString", "", "print('fuckyou')", true, true);
	// Place code above ^

	initDestroy();
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		module = hModule;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Main, NULL, NULL, NULL);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		qrex = NULL;
		LuaShared = NULL;
		module = NULL;
	}
	return TRUE;
}