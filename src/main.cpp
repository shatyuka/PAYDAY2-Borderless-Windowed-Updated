#include <superblt_flat.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <mutex>

enum class STATE
{
	HOLD,
	BORDERLESS,
	WINDOWED
};

STATE GetState(HWND hWnd)
{
	LONG lExstyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (lExstyle & WS_EX_TOPMOST)
		return STATE::HOLD;

	RECT c, d;
	GetClientRect(hWnd, &c);
	GetWindowRect(GetDesktopWindow(), &d);

	if (c.right - c.left >= d.right - d.left && c.bottom - c.top >= d.bottom - d.top)
	{
		if (lExstyle & WS_EX_OVERLAPPEDWINDOW)
			return STATE::BORDERLESS;
		else
			return STATE::HOLD;
	}
	else if (lExstyle & WS_EX_OVERLAPPEDWINDOW)
		return STATE::HOLD;
	else
		return STATE::WINDOWED;
}

void SetToNoboarder(HWND hWnd)
{
	while (true)
	{
		STATE state = GetState(hWnd);
		if (state == STATE::BORDERLESS)
		{
			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS);
			SetWindowLong(hWnd, GWL_EXSTYLE, 0);

			RECT rect;
			GetWindowRect(GetDesktopWindow(), &rect);
			SetWindowPos(hWnd, 0, 0, 0, rect.right, rect.bottom, SWP_FRAMECHANGED);
		}
		else if (state == STATE::WINDOWED)
		{
			SetWindowLong(hWnd, GWL_STYLE, WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX);
			SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
		}
		std::this_thread::yield();
	}
}

void Plugin_Init()
{
	PD2HOOK_LOG_LOG("Initializing Borderless Windowed Updated");
	HWND hWnd = FindWindow(L"diesel win32", L"PAYDAY 2");
	if (!hWnd)
	{
		PD2HOOK_LOG_ERROR("Failed to find PAYDAY 2 window.");
		return;
	}
	PD2HOOK_LOG_LOG("Borderless Windowed Updated loaded successfully.");
	
	// Simply modify the (ex)style of window via SetWindowLong will jam the program.
	// However, with my intuition, I invoke the proceduere in a new thread, and it works!
	// I don't know why, it might be something about dead lock. It works, that's all. :D
	std::thread(SetToNoboarder, hWnd).detach();
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
	return 0;
}
