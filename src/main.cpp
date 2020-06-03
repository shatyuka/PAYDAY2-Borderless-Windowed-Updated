#include <superblt_flat.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>

HWND g_hWnd;

void Windowed()
{
	SetWindowLong(g_hWnd, GWL_STYLE, WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
}

void FullscreenWindowed()
{
	Sleep(100);
	SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	SetWindowPos(g_hWnd, 0, 0, 0, rect.right, rect.bottom, SWP_FRAMECHANGED);
}

int ChangeDisplayMode(lua_State* L)
{
	int mode = luaL_checkint(L, 1);
	switch(mode)
	{
	case 0:
		break;
	case 1:
		std::thread(Windowed).detach();
		break;
	case 2:
		std::thread(FullscreenWindowed).detach();
		break;
	default:
		PD2HOOK_LOG_ERROR("Parameter error");
	}
	return 0;
}

void Plugin_Init()
{
	PD2HOOK_LOG_LOG("Initializing Borderless Windowed Updated");
	g_hWnd = FindWindow(L"diesel win32", L"PAYDAY 2");
	if (!g_hWnd)
	{
		PD2HOOK_LOG_ERROR("Failed to find PAYDAY 2 window.");
		return;
	}
	PD2HOOK_LOG_LOG("Borderless Windowed Updated loaded successfully.");
}

void Plugin_Update()
{
}

void Plugin_Setup_Lua(lua_State* L)
{
	// Deprecated, see this function's documentation (in superblt_flat.h) for more detail
}

int Plugin_PushLua(lua_State* L)
{
	lua_newtable(L);

	lua_pushcfunction(L, ChangeDisplayMode);
	lua_setfield(L, -2, "change_display_mode");
	
	return 1;
}
