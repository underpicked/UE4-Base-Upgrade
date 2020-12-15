//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
// This file contains compile-time options for Gui.
// Other options (memory allocation overrides, callbacks, etc.) can be set at runtime via the GuiIO structure - Gui::GetIO().
//-----------------------------------------------------------------------------

#pragma once

//---- Define assertion handler. Defaults to calling assert().
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define Gui_API __declspec( dllexport )
//#define Gui_API __declspec( dllimport )

//---- Don't define obsolete functions names. Consider enabling from time to time or when updating to reduce like hood of using already obsolete function/names
//#define Gui_DISABLE_OBSOLETE_FUNCTIONS

//---- Include Gui_user.h at the end of Gui.h
//#define Gui_INCLUDE_Gui_USER_H

//---- Don't implement default handlers for Windows (so as not to link with OpenClipboard() and others Win32 functions)
//#define Gui_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
//#define Gui_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS

//---- Don't implement demo windows functionality (ShowDemoWindow()/ShowStyleEditor()/ShowUserGuide() methods will be empty)
//---- It is very strongly recommended to NOT disable the demo windows. Please read the comment at the top of Gui_demo.cpp to learn why.
//#define Gui_DISABLE_DEMO_WINDOWS

//---- Don't implement ImFormatString(), ImFormatStringV() so you can reimplement them yourself.
//#define Gui_DISABLE_FORMAT_STRING_FUNCTIONS

//---- Pack colors to BGRA instead of RGBA (remove need to post process vertex buffer in back ends)
//#define Gui_USE_BGRA_PACKED_COLOR

//---- Implement STB libraries in a namespace to avoid linkage conflicts
//#define Gui_STB_NAMESPACE     GuiStb

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.
/*
#define IM_VEC2_CLASS_EXTRA                                                 \
ImVec2(const MyVec2& f) { x = f.x; y = f.y; }                       \
operator MyVec2() const { return MyVec2(x,y); }

#define IM_VEC4_CLASS_EXTRA                                                 \
ImVec4(const MyVec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }     \
operator MyVec4() const { return MyVec4(x,y,z,w); }
*/

//---- Use 32-bit vertex indices (instead of default: 16-bit) to allow meshes with more than 64K vertices
//#define ImDrawIdx unsigned int

//---- Tip: You can add extra functions within the Gui:: namespace, here or in your own headers files.
//---- e.g. create variants of the Gui::Value() helper for your low-level math types, or your own widgets/helpers.
/*
namespace Gui
{
void    Value(const char* prefix, const MyMatrix44& v, const char* float_format = NULL);
}
*/

