#include "Twitter.h"

#include <windows.h>
#include <tchar.h>

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	UBTwitter::MessageManagerWindow form;
	Application::Run(%form);
}

