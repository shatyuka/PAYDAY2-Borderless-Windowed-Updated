#include <superblt_flat.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>

HWND g_hWnd;

#define PAYDAY2_WINDOWED_STYLE (WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX)
#define PAYDAY2_FULLSCREEN_WINDOWED_STYLE (WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS)

void Windowed(int width, int height)
{
	SetWindowLong(g_hWnd, GWL_STYLE, PAYDAY2_WINDOWED_STYLE);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	rect.left = (rect.right - width) / 2;
	rect.top = (rect.bottom - height) / 2;
	rect.right = (rect.right + width) / 2;
	rect.bottom = (rect.bottom + height) / 2;
	AdjustWindowRectEx(&rect, PAYDAY2_WINDOWED_STYLE, FALSE, WS_EX_OVERLAPPEDWINDOW);
	SetWindowPos(g_hWnd, HWND_NOTOPMOST, rect.left >= 0 ? rect.left : 0, rect.top >= 0 ? rect.top : 0,
		rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
}

void FullscreenWindowed()
{
	Sleep(100);
	SetWindowLong(g_hWnd, GWL_STYLE, PAYDAY2_FULLSCREEN_WINDOWED_STYLE);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	SetWindowPos(g_hWnd, 0, 0, 0, rect.right, rect.bottom, SWP_FRAMECHANGED);
}

int ChangeDisplayMode(lua_State* L)
{
	int mode = luaL_checkint(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	switch (mode)
	{
	case 0:
		break;
	case 1:
		std::thread(Windowed, width, height).detach();
		break;
	case 2:
		std::thread(FullscreenWindowed).detach();
		break;
	default:
		PD2HOOK_LOG_ERROR("Invalid parameter");
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
