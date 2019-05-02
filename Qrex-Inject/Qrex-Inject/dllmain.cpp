#include <Windows.h>
#include "GLua.h"
#include <string>

ILuaShared* LuaShared = NULL;
ILuaInterface* qrex = NULL;

HMODULE module = NULL;


void addHook(std::string name, std::string identifier, CLuaFunction function)
{
	qrex->PushSpecial(SPECIAL_GLOB);
	qrex->GetField(-1, "hook");
	qrex->GetField(-1, "Add");
	qrex->PushString(name.c_str(), name.length());
	qrex->PushString(identifier.c_str(), identifier.length());
	qrex->PushCFunction(function);
	qrex->Call(3, 0);
	qrex->Pop();
	qrex->Pop();
}

void removeHook(std::string name, std::string identifier)
{
	qrex->PushSpecial(SPECIAL_GLOB);
	qrex->GetField(-1, "hook");
	qrex->GetField(-1, "Remove");
	qrex->PushString(name.c_str(), name.length());
	qrex->PushString(identifier.c_str(), identifier.length());
	qrex->Call(2, 0);
	qrex->Pop();
	qrex->Pop();
}

int Main(lua_State * state)
{
	removeHook("Think", "RunLuaMenuInit");
	// qrex->RunString("RunString", "", "print('fuckyou')", true, true);

	// Place code above ^
	return 0;
}

int DllInit()
{
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
	addHook("Think", "RunLuaMenuInit", Main);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		module = hModule;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DllInit, NULL, NULL, NULL);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		qrex = NULL;
		LuaShared = NULL;
		module = NULL;
	}
	return TRUE;
}