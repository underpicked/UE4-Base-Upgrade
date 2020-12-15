// Gui Win32 + DirectX11 binding
// In this binding, ImTextureID is used to store a 'ID3D11ShaderResourceView*' texture identifier. Read the FAQ about ImTextureID in Gui.cpp.

// You can copy and use unmodified Gui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: Gui_ImplXXXX_Init(), Gui_ImplXXXX_NewFrame(), Gui::Render() and Gui_ImplXXXX_Shutdown().
// If you are new to Gui, see examples/README.txt and documentation at the top of Gui.cpp.
// https://github.com/ocornut/Gui

struct ID3D11Device;
struct ID3D11DeviceContext;

Gui_API bool        Gui_ImplDX11_Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
Gui_API void        Gui_ImplDX11_Shutdown();
Gui_API void        Gui_ImplDX11_NewFrame();

// Use if you want to reset your rendering device without losing Gui state.
Gui_API void        Gui_ImplDX11_InvalidateDeviceObjects();
Gui_API bool        Gui_ImplDX11_CreateDeviceObjects();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.
/*
Gui_API LRESULT   Gui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/
