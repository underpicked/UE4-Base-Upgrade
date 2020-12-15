#include "stdafx.h"
#include "xor.hpp"
#include <cinttypes>
#include <math.h>
#include "d3dx9.h"
#include <d3d9types.h>

#define M_PI 3.14159265358979323846264338327950288419716939937510

//#define OFFSET_UWORLD 0xEX4MPL3 //Signature of our offset: 48 8b 0d ? ? ? ? 48 85 c9 74 30 e8 ? ? ? ? 48 8b f8
bool menu = true;
bool Esp = false;
bool Aimbot = false;
bool Box = false; 
// Width, height.
int width;
int height;

//JMP RBX (roblox :roflanebalo:)
PVOID jmp_rbx_0 = NULL;
// uh oh trash above ;(
HWND hWnd;
DWORD_PTR UworldAddress;
uint64_t base_address, uworld, game_instance, player_controller, localplayer, actors, pawn;
ID3D11Device *device = nullptr;
ID3D11DeviceContext *immediateContext = nullptr;
ID3D11RenderTargetView *renderTargetView = nullptr;
DWORD_PTR w2sAddress;
// Gui thing's ;3
HRESULT(*PresentOriginal)(IDXGISwapChain *swapChain, UINT syncInterval, UINT flags) = nullptr;
HRESULT(*ResizeOriginal)(IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;
WNDPROC oriWndProc = NULL;
extern LRESULT Gui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Gui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && menu == true)
	{
		return true;
	}
	return CallWindowProc(oriWndProc, hWnd, msg, wParam, lParam);
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};
struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
es:
	DWORD_PTR bonearray = ReadMemory<DWORD_PTR>(mesh + 0x420);

	if (IsBadReadPtr((DWORD_PTR*)bonearray, 8))
		goto es;

	return ReadMemory<FTransform>(bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = ReadMemory<FTransform>(mesh + 0x1C0);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

// Render will be above
void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	Gui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Gui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}
void WorldToScreen(Vector3 world, Vector3* out) {
	if (jmp_rbx_0 == NULL)
	{
		jmp_rbx_0 = (PVOID)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\xFF\x23").c_str(), _xor_("xx").c_str(), 0);
	}
	auto W2S = reinterpret_cast<char(__fastcall*)(uint64_t controller, Vector3 world, Vector3 * out, char)>(w2sAddress);

	(jmp_rbx_0, W2S, player_controller, world, out, (char)0);
}
void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	Gui::GetOverlayDrawList()->AddText(ImVec2(x, y), Gui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

//Here's the menu.
void Menu()
{
	GuiStyle& styled = Gui::GetStyle();
	GuiStyle& style = Gui::GetStyle();
	style.Alpha = 1.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(4, 2);
	style.ItemSpacing = ImVec2(6, 3);
	style.ItemInnerSpacing = ImVec2(6, 4);
	style.WindowRounding = 5.0f;
	style.FrameRounding = 4.0f;
	style.ColumnsMinSpacing = 50.0f;
	style.GrabMinSize = 14.0f;
	style.GrabRounding = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 30.0f;
	style.AntiAliasedLines = true;
	style.WindowRounding = 0.f;
	style.FramePadding = ImVec2(0, 0);
	style.ScrollbarSize = 6.f;
	style.ScrollbarRounding = 0.f;
	style.GrabMinSize = 6.f;

	style.Colors[GuiCol_WindowBg] = ImColor(12, 12, 12, 211);

	static bool pressed = false;

	if (GetKeyState(VK_INSERT) & 0x8000) //change menu key here
		pressed = true;
	else {
		pressed = false;
	}

	GuiIO& io = Gui::GetIO();
	(void)io;

	if (menu)
	{
		io.MouseDrawCursor = true;
	}
	else
	{
		io.MouseDrawCursor = false;
	}

	if (menu)
	{
		Gui::Begin(_xor_("Fortnite").c_str(), &menu, ImVec2(420, 420), 1.0f, GuiWindowFlags_NoResize | GuiWindowFlags_NoCollapse); //edit menu here

		ImVec2 size = Gui::GetItemRectSize();

		if (Gui::Button("AIMBOT", ImVec2((size.x - Gui::GetStyle().ItemSpacing.x - 0.02f) * 0.33f, size.y))) {

			Gui::Checkbox("Write Angles Aimbot", &Aimbot);

		}

		Gui::SameLine();
		if (Gui::Button("ESP", ImVec2((size.x - Gui::GetStyle().ItemSpacing.x - 0.02f) * 0.33f, size.y))) {

			//Features here

		}

		Gui::SameLine();
		if (Gui::Button("MISC", ImVec2((size.x - Gui::GetStyle().ItemSpacing.x - 0.02f) * 0.33f, size.y))) {

			//Features here

		}

	}
}
void SigScanning() {

	auto btAddrUWorld = FindPattern((PBYTE)base_address, 0x10000000, (PBYTE)_xor_("\x48\x8b\x0d\x00\x00\x00\x00\x48\x85\xc9\x74\x30\xe8\x00\x00\x00\x00\x48\x8b\xf8").c_str(), _xor_("xxx????xxxxxx????xxx").c_str(), 0);
	auto btOffUWorld = *reinterpret_cast<uint32_t*>(btAddrUWorld + 3);
	UworldAddress = reinterpret_cast<DWORD_PTR>(btAddrUWorld + 7 + btOffUWorld);
	//std::printf(_xor_("UWorld Offset: %p.\n").c_str(), UworldAddress - base_address);

}

void WriteAngles(float TargetX, float TargetY) //Not sure this is undetected lul.
{
	float x = TargetX / 6.666666666666667f;
	float y = TargetY / 6.666666666666667f;
	y = -(y);

	DWORD_PTR angle = player_controller + 0x420;
	*(float*)angle = y;

	DWORD_PTR anglex = player_controller + 0x420 + 0x4;
	*(float*)anglex = x;
}

__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	if (!device) {
		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);

		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();

		HWND targetWindow;
		EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
			DWORD pid = 0;
			GetWindowThreadProcessId(hWnd, &pid);
			if (pid == GetCurrentProcessId()) {
				*reinterpret_cast<HWND*>(lParam) = hWnd;
				return FALSE;
			}

			return true;
			}, reinterpret_cast<LPARAM>(&targetWindow));

		Gui_ImplDX11_Init(targetWindow, device, immediateContext);
		Gui_ImplDX11_CreateDeviceObjects();
	}

	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	auto& window = BeginScene();

	//if base address null return null
	if (base_address == NULL)
	{
		base_address = (uint64_t)GetModuleHandleA(NULL);
	}
	uint64_t CurrentActor = ReadMemory<uint64_t>(actors * 0x8);
	Vector3 Aimpos;
	uint64_t currentactormesh = ReadMemory<uint64_t>(CurrentActor + 0x280);
	Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
	WorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 10), &Aimpos);
	Vector3 Aimpos;
	float headX = Aimpos.x - width / 2;
	float headY = Aimpos.y - height / 2;
	if (Aimbot)
	{
		if (GetAsyncKeyState(VK_XBUTTON1)) {
			WriteAngles(headX / 3.5f, headY / 3.5f);
		}

		// check if fortnite/whatever game u want to use base on is launched
		hWnd = FindWindow(0, _xor_(L"Fortnite  ").c_str());

		RECT rect;
		if (GetWindowRect(hWnd, &rect))
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}

		//how to read memory part 1:
		uworld = ReadMemory<uint64_t>(UworldAddress);

		if (IsBadReadPtr((uint64_t*)uworld, 8))
			return;
		else
		{
			MessageBox(0, L"Found offset: UWORLD", L"UWORLD FOUND", MB_OK | MB_ICONINFORMATION);
		}

		game_instance = ReadMemory<uint64_t>(uworld + 0x180); /*put offset here, since mine can be outdated :/*/

		if (IsBadReadPtr((uint64_t*)game_instance, 8))
			return;
		else
		{
			MessageBox(0, L"Found offset: Game instance", L"GAME INSTANCE FOUND", MB_OK | MB_ICONINFORMATION);
		}

		localplayer = ReadMemory<uint64_t>(localplayer + 0x0);

		if (IsBadReadPtr((uint64_t*)localplayer, 8))
			return;
		else
		{
			MessageBox(0, L"Found offset: Local Player", L"LOCAL PLAYER FOUND", MB_OK | MB_ICONINFORMATION);
		}
		player_controller = ReadMemory<uint64_t>(localplayer + 0x30);

		if (IsBadReadPtr((uint64_t*)player_controller, 8))
			return;
		else
		{
			MessageBox(0, L"Found offset: Player Controller", L"Player Controller FOUND", MB_OK | MB_ICONINFORMATION);
		}

		Menu(); //Initialize menu.

		EndScene(window);

		return PresentOriginal(swapChain, syncInterval, flags);
	}
}
__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	Gui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

GuiWindow &BeginScene() {
	Gui_ImplDX11_NewFrame();
	Gui::PushStyleVar(GuiStyleVar_WindowBorderSize, 0);
	Gui::PushStyleVar(GuiStyleVar_WindowPadding, ImVec2(0, 0));
	Gui::PushStyleColor(GuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	Gui::Begin("##scene", nullptr, GuiWindowFlags_NoInputs | GuiWindowFlags_NoTitleBar);

	auto &io = Gui::GetIO();
	Gui::SetWindowPos(ImVec2(0, 0), GuiCond_Always);
	Gui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), GuiCond_Always);

	return *Gui::GetCurrentWindow();
}

VOID EndScene(GuiWindow &window) {
	window.DrawList->PushClipRectFullScreen();
	Gui::End();
	Gui::PopStyleColor();
	Gui::PopStyleVar(2);
	Gui::Render();
}

VOID Main() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	HWND window = FindWindow(0, _xor_(L"Fortnite  ").c_str());

	IDXGISwapChain      *swapChain    = nullptr;
	ID3D11Device        *device       = nullptr;
	ID3D11DeviceContext *context      = nullptr;
	auto                 featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd           = { 0 };
	sd.BufferCount                    = 1;
	sd.BufferDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage                    = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags                          = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow					  = window;
	sd.SampleDesc.Count               = 1;
	sd.Windowed                       = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, _xor_(L"error device").c_str(), _xor_(L"error").c_str(), MB_ICONERROR);
		return;
	}

	auto table = *reinterpret_cast<PVOID **>(swapChain);
	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	SigScanning();

	MH_Initialize();

	MH_CreateHook(present, PresentHook, reinterpret_cast<PVOID *>(&PresentOriginal));
	MH_EnableHook(present);

	MH_CreateHook(resize, ResizeHook, reinterpret_cast<PVOID *>(&ResizeOriginal));
	MH_EnableHook(resize);

	oriWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		Main();
	}

	return TRUE;
}