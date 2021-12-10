#include <superblt_flat.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <vector>

HWND g_hWnd;
std::vector<HMONITOR> g_hMonitors;

#define PAYDAY2_WINDOWED_STYLE (WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX)
#define PAYDAY2_FULLSCREEN_WINDOWED_STYLE (WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

RECT GetMonitorRect(int adapter)
{
	if (adapter < g_hMonitors.size())
	{
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo(g_hMonitors[adapter], &info))
			return info.rcMonitor;
	}
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	return rect;
}

void Windowed(int width, int height, int adapter)
{
	SetWindowLong(g_hWnd, GWL_STYLE, PAYDAY2_WINDOWED_STYLE);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
	RECT rect{ 0, 0, width, height };
	AdjustWindowRectEx(&rect, PAYDAY2_WINDOWED_STYLE, FALSE, WS_EX_OVERLAPPEDWINDOW);
	const int window_width = rect.right - rect.left;
	const int window_height = rect.bottom - rect.top;
	rect = GetMonitorRect(adapter);
	const int screen_width = rect.right - rect.left;
	const int screen_height = rect.bottom - rect.top;
	if (screen_width >= window_width)
		rect.left = (screen_width - window_width) / 2;
	if (screen_height >= window_height)
		rect.top = (screen_height - window_height) / 2;
	SetWindowPos(g_hWnd, HWND_NOTOPMOST, rect.left, rect.top, window_width, window_height, SWP_FRAMECHANGED);
}

void FullscreenWindowed(int adapter)
{
	Sleep(100);
	SetWindowLong(g_hWnd, GWL_STYLE, PAYDAY2_FULLSCREEN_WINDOWED_STYLE);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
	RECT rect = GetMonitorRect(adapter);
	SetWindowPos(g_hWnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
}

int ChangeDisplayMode(lua_State* L)
{
	int mode = luaL_checkint(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	int adapter = luaL_checkint(L, 4);
	switch (mode)
	{
	case 0:
		break;
	case 1:
		std::thread(Windowed, width, height, adapter).detach();
		break;
	case 2:
		std::thread(FullscreenWindowed, adapter).detach();
		break;
	default:
		PD2HOOK_LOG_ERROR("Invalid parameter");
	}
	return 0;
}

BOOL CALLBACK MonitorEnumProcCallback(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM dwData)
{
	g_hMonitors.push_back(hMonitor);
	return TRUE;
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
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProcCallback, NULL);
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
