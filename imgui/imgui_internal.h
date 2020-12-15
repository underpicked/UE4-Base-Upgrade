// dear Gui, v1.54 WIP
// (internals)

// You may use this file to debug, understand or extend Gui features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define Gui_DEFINE_MATH_OPERATORS
// To implement maths operators for ImVec2 (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)

#pragma once

#ifndef Gui_VERSION
#error Must include Gui.h before Gui_internal.h
#endif

#include <stdio.h>      // FILE*
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251) // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when Gui_API is set to__declspec(dllexport)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"        // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"     // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

struct ImRect;
struct GuiColMod;
struct GuiStyleMod;
struct GuiGroupData;
struct GuiMenuColumns;
struct GuiDrawContext;
struct GuiTextEditState;
struct GuiMouseCursorData;
struct GuiPopupRef;
struct GuiWindow;
struct GuiWindowSettings;

typedef int GuiLayoutType;        // enum: horizontal or vertical             // enum GuiLayoutType_
typedef int GuiButtonFlags;       // flags: for ButtonEx(), ButtonBehavior()  // enum GuiButtonFlags_
typedef int GuiItemFlags;         // flags: for PushItemFlag()                // enum GuiItemFlags_
typedef int GuiSeparatorFlags;    // flags: for Separator() - internal        // enum GuiSeparatorFlags_
typedef int GuiSliderFlags;       // flags: for SliderBehavior()              // enum GuiSliderFlags_

									//-------------------------------------------------------------------------
									// STB libraries
									//-------------------------------------------------------------------------

namespace GuiStb
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             GuiTextEditState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   -1.0f
#include "stb_textedit.h"

} // namespace GuiStb

  //-----------------------------------------------------------------------------
  // Context
  //-----------------------------------------------------------------------------

#ifndef GGui
extern Gui_API GuiContext* GGui;  // Current implicit Gui context pointer
#endif

										//-----------------------------------------------------------------------------
										// Helpers
										//-----------------------------------------------------------------------------

#define IM_PI           3.14159265358979323846f

										// Helpers: UTF-8 <> wchar
Gui_API int           ImTextStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end);      // return output UTF-8 bytes count
Gui_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);          // return input UTF-8 bytes count
Gui_API int           ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
Gui_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                            // return number of UTF-8 code-points (NOT bytes count)
Gui_API int           ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end);                   // return number of bytes to express string as UTF-8 code-points

																														   // Helpers: Misc
Gui_API ImU32         ImHash(const void* data, int data_size, ImU32 seed = 0);    // Pass data_size==0 for zero-terminated strings
Gui_API void*         ImFileLoadToMemory(const char* filename, const char* file_open_mode, int* out_file_size = NULL, int padding_bytes = 0);
Gui_API FILE*         ImFileOpen(const char* filename, const char* file_open_mode);
static inline bool      ImCharIsSpace(int c) { return c == ' ' || c == '\t' || c == 0x3000; }
static inline bool      ImIsPowerOfTwo(int v) { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

// Helpers: Geometry
Gui_API ImVec2        ImLineClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& p);
Gui_API bool          ImTriangleContainsPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
Gui_API ImVec2        ImTriangleClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
Gui_API void          ImTriangleBarycentricCoords(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p, float& out_u, float& out_v, float& out_w);

// Helpers: String
Gui_API int           ImStricmp(const char* str1, const char* str2);
Gui_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
Gui_API void          ImStrncpy(char* dst, const char* src, size_t count);
Gui_API char*         ImStrdup(const char* str);
Gui_API char*         ImStrchrRange(const char* str_begin, const char* str_end, char c);
Gui_API int           ImStrlenW(const ImWchar* str);
Gui_API const ImWchar*ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin); // Find beginning-of-line
Gui_API const char*   ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
Gui_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
Gui_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);

// Helpers: Math
// We are keeping those not leaking to the user by default, in the case the user has implicit cast operators between ImVec2 and its own types (when IM_VEC2_CLASS_EXTRA is defined)
#ifdef Gui_DEFINE_MATH_OPERATORS
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x*rhs, lhs.y*rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x*rhs.x, lhs.y*rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z, lhs.w*rhs.w); }
#endif

static inline int    ImMin(int lhs, int rhs) { return lhs < rhs ? lhs : rhs; }
static inline int    ImMax(int lhs, int rhs) { return lhs >= rhs ? lhs : rhs; }
static inline float  ImMin(float lhs, float rhs) { return lhs < rhs ? lhs : rhs; }
static inline float  ImMax(float lhs, float rhs) { return lhs >= rhs ? lhs : rhs; }
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline int    ImClamp(int v, int mn, int mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }
static inline float  ImClamp(float v, float mn, float mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }
static inline ImVec2 ImClamp(const ImVec2& f, const ImVec2& mn, ImVec2 mx) { return ImVec2(ImClamp(f.x, mn.x, mx.x), ImClamp(f.y, mn.y, mx.y)); }
static inline float  ImSaturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline void   ImSwap(int& a, int& b) { int tmp = a; a = b; b = tmp; }
static inline void   ImSwap(float& a, float& b) { float tmp = a; a = b; b = tmp; }
static inline int    ImLerp(int a, int b, float t) { return (int)(a + (b - a) * t); }
static inline float  ImLerp(float a, float b, float t) { return a + (b - a) * t; }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t) { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t) { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImLengthSqr(const ImVec2& lhs) { return lhs.x*lhs.x + lhs.y*lhs.y; }
static inline float  ImLengthSqr(const ImVec4& lhs) { return lhs.x*lhs.x + lhs.y*lhs.y + lhs.z*lhs.z + lhs.w*lhs.w; }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value) { float d = lhs.x*lhs.x + lhs.y*lhs.y; if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }
static inline float  ImFloor(float f) { return (float)(int)f; }
static inline ImVec2 ImFloor(const ImVec2& v) { return ImVec2((float)(int)v.x, (float)(int)v.y); }
static inline float  ImDot(const ImVec2& a, const ImVec2& b) { return a.x * b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed) { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }

// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a dummy parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
struct ImNewPlacementDummy {};
inline void* operator   new(size_t, ImNewPlacementDummy, void* ptr) { return ptr; }
inline void  operator   delete(void*, ImNewPlacementDummy, void*) {} // This is only required so we can use the symetrical new()
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewPlacementDummy(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewPlacementDummy(), Gui::MemAlloc(sizeof(_TYPE))) _TYPE
template <typename T> void IM_DELETE(T*& p) { if (p) { p->~T(); Gui::MemFree(p); p = NULL; } }

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

enum GuiButtonFlags_
{
	GuiButtonFlags_Repeat = 1 << 0,   // hold to repeat
	GuiButtonFlags_PressedOnClickRelease = 1 << 1,   // return true on click + release on same item [DEFAULT if no PressedOn* flag is set]
	GuiButtonFlags_PressedOnClick = 1 << 2,   // return true on click (default requires click+release)
	GuiButtonFlags_PressedOnRelease = 1 << 3,   // return true on release (default requires click+release)
	GuiButtonFlags_PressedOnDoubleClick = 1 << 4,   // return true on double-click (default requires click+release)
	GuiButtonFlags_FlattenChildren = 1 << 5,   // allow interactions even if a child window is overlapping
	GuiButtonFlags_AllowItemOverlap = 1 << 6,   // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
	GuiButtonFlags_DontClosePopups = 1 << 7,   // disable automatically closing parent popup on press // [UNUSED]
	GuiButtonFlags_Disabled = 1 << 8,   // disable interactions
	GuiButtonFlags_AlignTextBaseLine = 1 << 9,   // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
	GuiButtonFlags_NoKeyModifiers = 1 << 10,  // disable interaction if a key modifier is held
	GuiButtonFlags_NoHoldingActiveID = 1 << 11,  // don't set ActiveId while holding the mouse (GuiButtonFlags_PressedOnClick only)
	GuiButtonFlags_PressedOnDragDropHold = 1 << 12   // press when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
};

enum GuiSliderFlags_
{
	GuiSliderFlags_Vertical = 1 << 0
};

enum GuiColumnsFlags_
{
	// Default: 0
	GuiColumnsFlags_NoBorder = 1 << 0,   // Disable column dividers
	GuiColumnsFlags_NoResize = 1 << 1,   // Disable resizing columns when clicking on the dividers
	GuiColumnsFlags_NoPreserveWidths = 1 << 2,   // Disable column width preservation when adjusting columns
	GuiColumnsFlags_NoForceWithinWindow = 1 << 3,   // Disable forcing columns to fit within window
	GuiColumnsFlags_GrowParentContentsSize = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.
};

enum GuiSelectableFlagsPrivate_
{
	// NB: need to be in sync with last value of GuiSelectableFlags_
	GuiSelectableFlags_Menu = 1 << 3,   // -> PressedOnClick
	GuiSelectableFlags_MenuItem = 1 << 4,   // -> PressedOnRelease
	GuiSelectableFlags_Disabled = 1 << 5,
	GuiSelectableFlags_DrawFillAvailWidth = 1 << 6
};

enum GuiSeparatorFlags_
{
	GuiSeparatorFlags_Horizontal = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
	GuiSeparatorFlags_Vertical = 1 << 1
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
enum GuiLayoutType_
{
	GuiLayoutType_Vertical,
	GuiLayoutType_Horizontal
};

enum GuiAxis
{
	GuiAxis_None = -1,
	GuiAxis_X = 0,
	GuiAxis_Y = 1
};

enum GuiPlotType
{
	GuiPlotType_Lines,
	GuiPlotType_Histogram
};

enum GuiDataType
{
	GuiDataType_Int,
	GuiDataType_Float,
	GuiDataType_Float2
};

enum GuiDir
{
	GuiDir_None = -1,
	GuiDir_Left = 0,
	GuiDir_Right = 1,
	GuiDir_Up = 2,
	GuiDir_Down = 3,
	GuiDir_Count_
};

// 2D axis aligned bounding-box
// NB: we can't rely on ImVec2 math operators being available here
struct Gui_API ImRect
{
	ImVec2      Min;    // Upper-left
	ImVec2      Max;    // Lower-right

	ImRect() : Min(FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX) {}
	ImRect(const ImVec2& min, const ImVec2& max) : Min(min), Max(max) {}
	ImRect(const ImVec4& v) : Min(v.x, v.y), Max(v.z, v.w) {}
	ImRect(float x1, float y1, float x2, float y2) : Min(x1, y1), Max(x2, y2) {}

	ImVec2      GetCenter() const { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
	ImVec2      GetSize() const { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
	float       GetWidth() const { return Max.x - Min.x; }
	float       GetHeight() const { return Max.y - Min.y; }
	ImVec2      GetTL() const { return Min; }                   // Top-left
	ImVec2      GetTR() const { return ImVec2(Max.x, Min.y); }  // Top-right
	ImVec2      GetBL() const { return ImVec2(Min.x, Max.y); }  // Bottom-left
	ImVec2      GetBR() const { return Max; }                   // Bottom-right
	bool        Contains(const ImVec2& p) const { return p.x >= Min.x && p.y >= Min.y && p.x     < Max.x && p.y     < Max.y; }
	bool        Contains(const ImRect& r) const { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x < Max.x && r.Max.y < Max.y; }
	bool        Overlaps(const ImRect& r) const { return r.Min.y <  Max.y && r.Max.y >  Min.y && r.Min.x < Max.x && r.Max.x > Min.x; }
	void        Add(const ImVec2& p) { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
	void        Add(const ImRect& r) { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
	void        Expand(const float amount) { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
	void        Expand(const ImVec2& amount) { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
	void        Translate(const ImVec2& v) { Min.x += v.x; Min.y += v.y; Max.x += v.x; Max.y += v.y; }
	void        ClipWith(const ImRect& r) { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
	void        ClipWithFull(const ImRect& r) { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
	void        Floor() { Min.x = (float)(int)Min.x; Min.y = (float)(int)Min.y; Max.x = (float)(int)Max.x; Max.y = (float)(int)Max.y; }
	void        FixInverted() { if (Min.x > Max.x) ImSwap(Min.x, Max.x); if (Min.y > Max.y) ImSwap(Min.y, Max.y); }
	bool        IsFinite() const { return Min.x != FLT_MAX; }
};

// Stacked color modifier, backup of modified data so we can restore it
struct GuiColMod
{
	GuiCol    Col;
	ImVec4      BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct GuiStyleMod
{
	GuiStyleVar   VarIdx;
	union { int BackupInt[2]; float BackupFloat[2]; };
	GuiStyleMod(GuiStyleVar idx, int v) { VarIdx = idx; BackupInt[0] = v; }
	GuiStyleMod(GuiStyleVar idx, float v) { VarIdx = idx; BackupFloat[0] = v; }
	GuiStyleMod(GuiStyleVar idx, ImVec2 v) { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Stacked data for BeginGroup()/EndGroup()
struct GuiGroupData
{
	ImVec2      BackupCursorPos;
	ImVec2      BackupCursorMaxPos;
	float       BackupIndentX;
	float       BackupGroupOffsetX;
	float       BackupCurrentLineHeight;
	float       BackupCurrentLineTextBaseOffset;
	float       BackupLogLinePosY;
	bool        BackupActiveIdIsAlive;
	bool        AdvanceCursor;
};

// Simple column measurement currently used for MenuItem() only. This is very short-sighted/throw-away code and NOT a generic helper.
struct Gui_API GuiMenuColumns
{
	int         Count;
	float       Spacing;
	float       Width, NextWidth;
	float       Pos[4], NextWidths[4];

	GuiMenuColumns();
	void        Update(int count, float spacing, bool clear);
	float       DeclColumns(float w0, float w1, float w2);
	float       CalcExtraSpace(float avail_w);
};

// Internal state of the currently focused/edited text input box
struct Gui_API GuiTextEditState
{
	GuiID             Id;                         // widget id owning the text state
	ImVector<ImWchar>   Text;                       // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
	ImVector<char>      InitialText;                // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
	ImVector<char>      TempTextBuffer;
	int                 CurLenA, CurLenW;           // we need to maintain our buffer length in both UTF-8 and wchar format.
	int                 BufSizeA;                   // end-user buffer size
	float               ScrollX;
	GuiStb::STB_TexteditState   StbState;
	float               CursorAnim;
	bool                CursorFollow;
	bool                SelectedAllMouseLock;

	GuiTextEditState() { memset(this, 0, sizeof(*this)); }
	void                CursorAnimReset() { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
	void                CursorClamp() { StbState.cursor = ImMin(StbState.cursor, CurLenW); StbState.select_start = ImMin(StbState.select_start, CurLenW); StbState.select_end = ImMin(StbState.select_end, CurLenW); }
	bool                HasSelection() const { return StbState.select_start != StbState.select_end; }
	void                ClearSelection() { StbState.select_start = StbState.select_end = StbState.cursor; }
	void                SelectAll() { StbState.select_start = 0; StbState.cursor = StbState.select_end = CurLenW; StbState.has_preferred_x = false; }
	void                OnKeyPressed(int key);
};

// Data saved in Gui.ini file
struct GuiWindowSettings
{
	char*       Name;
	GuiID     Id;
	ImVec2      Pos;
	ImVec2      Size;
	bool        Collapsed;

	GuiWindowSettings() { Name = NULL; Id = 0; Pos = Size = ImVec2(0, 0); Collapsed = false; }
};

struct GuiSettingsHandler
{
	const char* TypeName;   // Short description stored in .ini file. Disallowed characters: '[' ']'  
	GuiID     TypeHash;   // == ImHash(TypeName, 0, 0)
	void*       (*ReadOpenFn)(GuiContext* ctx, GuiSettingsHandler* handler, const char* name);
	void(*ReadLineFn)(GuiContext* ctx, GuiSettingsHandler* handler, void* entry, const char* line);
	void(*WriteAllFn)(GuiContext* ctx, GuiSettingsHandler* handler, GuiTextBuffer* out_buf);
	void*       UserData;

	GuiSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

// Mouse cursor data (used when io.MouseDrawCursor is set)
struct GuiMouseCursorData
{
	GuiMouseCursor    Type;
	ImVec2              HotOffset;
	ImVec2              Size;
	ImVec2              TexUvMin[2];
	ImVec2              TexUvMax[2];
};

// Storage for current popup stack
struct GuiPopupRef
{
	GuiID             PopupId;        // Set on OpenPopup()
	GuiWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
	GuiWindow*        ParentWindow;   // Set on OpenPopup()
	int                 OpenFrameCount; // Set on OpenPopup()
	GuiID             OpenParentId;   // Set on OpenPopup(), we need this to differenciate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
	ImVec2              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
	ImVec2              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup
};

struct GuiColumnData
{
	float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
	float               OffsetNormBeforeResize;
	GuiColumnsFlags   Flags;              // Not exposed
	ImRect              ClipRect;

	GuiColumnData() { OffsetNorm = OffsetNormBeforeResize = 0.0f; Flags = 0; }
};

struct GuiColumnsSet
{
	GuiID             ID;
	GuiColumnsFlags   Flags;
	bool                IsFirstFrame;
	bool                IsBeingResized;
	int                 Current;
	int                 Count;
	float               MinX, MaxX;
	float               StartPosY;
	float               StartMaxPosX;       // Backup of CursorMaxPos
	float               CellMinY, CellMaxY;
	ImVector<GuiColumnData> Columns;

	GuiColumnsSet() { Clear(); }
	void Clear()
	{
		ID = 0;
		Flags = 0;
		IsFirstFrame = false;
		IsBeingResized = false;
		Current = 0;
		Count = 1;
		MinX = MaxX = 0.0f;
		StartPosY = 0.0f;
		StartMaxPosX = 0.0f;
		CellMinY = CellMaxY = 0.0f;
		Columns.clear();
	}
};

struct Gui_API ImDrawListSharedData
{
	ImVec2          TexUvWhitePixel;            // UV of white pixel in the atlas
	ImFont*         Font;                       // Current/default font (optional, for simplified AddText overload)
	float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
	float           CurveTessellationTol;
	ImVec4          ClipRectFullscreen;         // Value for PushClipRectFullscreen()

												// Const data
												// FIXME: Bake rounded corners fill/borders in atlas
	ImVec2          CircleVtx12[12];

	ImDrawListSharedData();
};

struct ImDrawDataBuilder
{
	ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

	void Clear() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
	void ClearFreeMemory() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
	Gui_API void FlattenIntoSingleLayer();
};

// Storage for SetNexWindow** functions
struct GuiNextWindowData
{
	GuiCond               PosCond;
	GuiCond               SizeCond;
	GuiCond               ContentSizeCond;
	GuiCond               CollapsedCond;
	GuiCond               SizeConstraintCond;
	GuiCond               FocusCond;
	ImVec2                  PosVal;
	ImVec2                  PosPivotVal;
	ImVec2                  SizeVal;
	ImVec2                  ContentSizeVal;
	bool                    CollapsedVal;
	ImRect                  SizeConstraintRect;                 // Valid if 'SetNextWindowSizeConstraint' is true
	GuiSizeCallback       SizeCallback;
	void*                   SizeCallbackUserData;

	GuiNextWindowData()
	{
		PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = 0;
		PosVal = PosPivotVal = SizeVal = ImVec2(0.0f, 0.0f);
		ContentSizeVal = ImVec2(0.0f, 0.0f);
		CollapsedVal = false;
		SizeConstraintRect = ImRect();
		SizeCallback = NULL;
		SizeCallbackUserData = NULL;
	}

	void    Clear()
	{
		PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = 0;
	}
};

// Main state for Gui
struct GuiContext
{
	bool                    Initialized;
	GuiIO                 IO;
	GuiStyle              Style;
	ImFont*                 Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
	float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
	float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
	ImDrawListSharedData    DrawListSharedData;

	float                   Time;
	int                     FrameCount;
	int                     FrameCountEnded;
	int                     FrameCountRendered;
	ImVector<GuiWindow*>  Windows;
	ImVector<GuiWindow*>  WindowsSortBuffer;
	ImVector<GuiWindow*>  CurrentWindowStack;
	GuiStorage            WindowsById;
	int                     WindowsActiveCount;
	GuiWindow*            CurrentWindow;                      // Being drawn into
	GuiWindow*            NavWindow;                          // Nav/focused window for navigation
	GuiWindow*            HoveredWindow;                      // Will catch mouse inputs
	GuiWindow*            HoveredRootWindow;                  // Will catch mouse inputs (for focus/move only)
	GuiID                 HoveredId;                          // Hovered widget
	bool                    HoveredIdAllowOverlap;
	GuiID                 HoveredIdPreviousFrame;
	float                   HoveredIdTimer;
	GuiID                 ActiveId;                           // Active widget
	GuiID                 ActiveIdPreviousFrame;
	float                   ActiveIdTimer;
	bool                    ActiveIdIsAlive;                    // Active widget has been seen this frame
	bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
	bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
	ImVec2                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
	GuiWindow*            ActiveIdWindow;
	GuiWindow*            MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actually window that is moved is generally MovingWindow->RootWindow.
	GuiID                 MovingWindowMoveId;                 // == MovingWindow->MoveId
	ImVector<GuiColMod>   ColorModifiers;                     // Stack for PushStyleColor()/PopStyleColor()
	ImVector<GuiStyleMod> StyleModifiers;                     // Stack for PushStyleVar()/PopStyleVar()
	ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont()
	ImVector<GuiPopupRef> OpenPopupStack;                     // Which popups are open (persistent)
	ImVector<GuiPopupRef> CurrentPopupStack;                  // Which level of BeginPopup() we are in (reset every frame)
	GuiNextWindowData     NextWindowData;                     // Storage for SetNextWindow** functions
	bool                    NextTreeNodeOpenVal;                // Storage for SetNextTreeNode** functions
	GuiCond               NextTreeNodeOpenCond;

	// Render
	ImDrawData              DrawData;                           // Main ImDrawData instance to pass render information to the user
	ImDrawDataBuilder       DrawDataBuilder;
	float                   ModalWindowDarkeningRatio;
	ImDrawList              OverlayDrawList;                    // Optional software render of mouse cursors, if io.MouseDrawCursor is set + a few debug overlays
	GuiMouseCursor        MouseCursor;
	GuiMouseCursorData    MouseCursorData[GuiMouseCursor_Count_];

	// Drag and Drop
	bool                    DragDropActive;
	GuiDragDropFlags      DragDropSourceFlags;
	int                     DragDropMouseButton;
	GuiPayload            DragDropPayload;
	ImRect                  DragDropTargetRect;
	GuiID                 DragDropTargetId;
	float                   DragDropAcceptIdCurrRectSurface;
	GuiID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
	GuiID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
	int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
	ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly
	unsigned char           DragDropPayloadBufLocal[8];

	// Widget state
	GuiTextEditState      InputTextState;
	ImFont                  InputTextPasswordFont;
	GuiID                 ScalarAsInputTextId;                // Temporary text input when CTRL+clicking on a slider, etc.
	GuiColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
	ImVec4                  ColorPickerRef;
	float                   DragCurrentValue;                   // Currently dragged value, always float, not rounded by end-user precision settings
	ImVec2                  DragLastMouseDelta;
	float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
	float                   DragSpeedScaleSlow;
	float                   DragSpeedScaleFast;
	ImVec2                  ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use storage?
	int                     TooltipOverrideCount;
	ImVector<char>          PrivateClipboard;                   // If no custom clipboard handler is defined
	ImVec2                  OsImePosRequest, OsImePosSet;       // Cursor position request & last passed to the OS Input Method Editor

																// Settings
	float                          SettingsDirtyTimer;          // Save .ini Settings on disk when time reaches zero
	ImVector<GuiWindowSettings>  SettingsWindows;             // .ini settings for GuiWindow
	ImVector<GuiSettingsHandler> SettingsHandlers;            // List of .ini settings handlers

																// Logging
	bool                    LogEnabled;
	FILE*                   LogFile;                            // If != NULL log to stdout/ file
	GuiTextBuffer*        LogClipboard;                       // Else log to clipboard. This is pointer so our GGui static constructor doesn't call heap allocators.
	int                     LogStartDepth;
	int                     LogAutoExpandMaxDepth;

	// Misc
	float                   FramerateSecPerFrame[120];          // calculate estimate of framerate for user
	int                     FramerateSecPerFrameIdx;
	float                   FramerateSecPerFrameAccum;
	int                     WantCaptureMouseNextFrame;          // explicit capture via CaptureInputs() sets those flags
	int                     WantCaptureKeyboardNextFrame;
	int                     WantTextInputNextFrame;
	char                    TempBuffer[1024 * 3 + 1];               // temporary text buffer

	GuiContext() : OverlayDrawList(NULL)
	{
		Initialized = false;
		Font = NULL;
		FontSize = FontBaseSize = 0.0f;

		Time = 0.0f;
		FrameCount = 0;
		FrameCountEnded = FrameCountRendered = -1;
		WindowsActiveCount = 0;
		CurrentWindow = NULL;
		NavWindow = NULL;
		HoveredWindow = NULL;
		HoveredRootWindow = NULL;
		HoveredId = 0;
		HoveredIdAllowOverlap = false;
		HoveredIdPreviousFrame = 0;
		HoveredIdTimer = 0.0f;
		ActiveId = 0;
		ActiveIdPreviousFrame = 0;
		ActiveIdTimer = 0.0f;
		ActiveIdIsAlive = false;
		ActiveIdIsJustActivated = false;
		ActiveIdAllowOverlap = false;
		ActiveIdClickOffset = ImVec2(-1, -1);
		ActiveIdWindow = NULL;
		MovingWindow = NULL;
		MovingWindowMoveId = 0;
		NextTreeNodeOpenVal = false;
		NextTreeNodeOpenCond = 0;

		ModalWindowDarkeningRatio = 0.0f;
		OverlayDrawList._Data = &DrawListSharedData;
		OverlayDrawList._OwnerName = "##Overlay"; // Give it a name for debugging
		MouseCursor = GuiMouseCursor_Arrow;
		memset(MouseCursorData, 0, sizeof(MouseCursorData));

		DragDropActive = false;
		DragDropSourceFlags = 0;
		DragDropMouseButton = -1;
		DragDropTargetId = 0;
		DragDropAcceptIdCurrRectSurface = 0.0f;
		DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
		DragDropAcceptFrameCount = -1;
		memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

		ScalarAsInputTextId = 0;
		ColorEditOptions = GuiColorEditFlags__OptionsDefault;
		DragCurrentValue = 0.0f;
		DragLastMouseDelta = ImVec2(0.0f, 0.0f);
		DragSpeedDefaultRatio = 1.0f / 100.0f;
		DragSpeedScaleSlow = 1.0f / 100.0f;
		DragSpeedScaleFast = 10.0f;
		ScrollbarClickDeltaToGrabCenter = ImVec2(0.0f, 0.0f);
		TooltipOverrideCount = 0;
		OsImePosRequest = OsImePosSet = ImVec2(-1.0f, -1.0f);

		SettingsDirtyTimer = 0.0f;

		LogEnabled = false;
		LogFile = NULL;
		LogClipboard = NULL;
		LogStartDepth = 0;
		LogAutoExpandMaxDepth = 2;

		memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
		FramerateSecPerFrameIdx = 0;
		FramerateSecPerFrameAccum = 0.0f;
		WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
		memset(TempBuffer, 0, sizeof(TempBuffer));
	}
};

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
enum GuiItemFlags_
{
	GuiItemFlags_AllowKeyboardFocus = 1 << 0,  // true
	GuiItemFlags_ButtonRepeat = 1 << 1,  // false    // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
	GuiItemFlags_Disabled = 1 << 2,  // false    // FIXME-WIP: Disable interactions but doesn't affect visuals. Should be: grey out and disable interactions with widgets that affect data + view widgets (WIP) 
									   //GuiItemFlags_NoNav                      = 1 << 3,  // false
									   //GuiItemFlags_NoNavDefaultFocus          = 1 << 4,  // false
									   GuiItemFlags_SelectableDontClosePopup = 1 << 5,  // false    // MenuItem/Selectable() automatically closes current Popup window
									   GuiItemFlags_Default_ = GuiItemFlags_AllowKeyboardFocus
};

// Transient per-window data, reset at the beginning of the frame
// FIXME: That's theory, in practice the delimitation between GuiWindow and GuiDrawContext is quite tenuous and could be reconsidered.
struct Gui_API GuiDrawContext
{
	ImVec2                  CursorPos;
	ImVec2                  CursorPosPrevLine;
	ImVec2                  CursorStartPos;
	ImVec2                  CursorMaxPos;           // Used to implicitly calculate the size of our contents, always growing during the frame. Turned into window->SizeContents at the beginning of next frame
	float                   CurrentLineHeight;
	float                   CurrentLineTextBaseOffset;
	float                   PrevLineHeight;
	float                   PrevLineTextBaseOffset;
	float                   LogLinePosY;
	int                     TreeDepth;
	GuiID                 LastItemId;
	ImRect                  LastItemRect;
	bool                    LastItemRectHoveredRect;
	bool                    MenuBarAppending;
	float                   MenuBarOffsetX;
	ImVector<GuiWindow*>  ChildWindows;
	GuiStorage*           StateStorage;
	GuiLayoutType         LayoutType;

	// We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
	GuiItemFlags          ItemFlags;              // == ItemFlagsStack.back() [empty == GuiItemFlags_Default]
	float                   ItemWidth;              // == ItemWidthStack.back(). 0.0: default, >0.0: width in pixels, <0.0: align xx pixels to the right of window
	float                   TextWrapPos;            // == TextWrapPosStack.back() [empty == -1.0f]
	ImVector<GuiItemFlags>ItemFlagsStack;
	ImVector<float>         ItemWidthStack;
	ImVector<float>         TextWrapPosStack;
	ImVector<GuiGroupData>GroupStack;
	int                     StackSizesBackup[6];    // Store size of various stacks for asserting

	float                   IndentX;                // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
	float                   GroupOffsetX;
	float                   ColumnsOffsetX;         // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
	GuiColumnsSet*        ColumnsSet;             // Current columns set

	GuiDrawContext()
	{
		CursorPos = CursorPosPrevLine = CursorStartPos = CursorMaxPos = ImVec2(0.0f, 0.0f);
		CurrentLineHeight = PrevLineHeight = 0.0f;
		CurrentLineTextBaseOffset = PrevLineTextBaseOffset = 0.0f;
		LogLinePosY = -1.0f;
		TreeDepth = 0;
		LastItemId = 0;
		LastItemRect = ImRect();
		LastItemRectHoveredRect = false;
		MenuBarAppending = false;
		MenuBarOffsetX = 0.0f;
		StateStorage = NULL;
		LayoutType = GuiLayoutType_Vertical;
		ItemWidth = 0.0f;
		ItemFlags = GuiItemFlags_Default_;
		TextWrapPos = -1.0f;
		memset(StackSizesBackup, 0, sizeof(StackSizesBackup));

		IndentX = 0.0f;
		GroupOffsetX = 0.0f;
		ColumnsOffsetX = 0.0f;
		ColumnsSet = NULL;
	}
};

// Windows data
struct Gui_API GuiWindow
{
	char*                   Name;
	GuiID                 ID;                                 // == ImHash(Name)
	GuiWindowFlags        Flags;                              // See enum GuiWindowFlags_
	ImVec2                  PosFloat;
	ImVec2                  Pos;                                // Position rounded-up to nearest pixel
	ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
	ImVec2                  SizeFull;                           // Size when non collapsed
	ImVec2                  SizeFullAtLastBegin;                // Copy of SizeFull at the end of Begin. This is the reference value we'll use on the next frame to decide if we need scrollbars.
	ImVec2                  SizeContents;                       // Size of contents (== extents reach of the drawing cursor) from previous frame. Include decoration, window title, border, menu, etc.
	ImVec2                  SizeContentsExplicit;               // Size of contents explicitly set by the user via SetNextWindowContentSize()
	ImRect                  ContentsRegionRect;                 // Maximum visible content position in window coordinates. ~~ (SizeContentsExplicit ? SizeContentsExplicit : Size - ScrollbarSizes) - CursorStartPos, per axis
	ImVec2                  WindowPadding;                      // Window padding at the time of begin.
	float                   WindowRounding;                     // Window rounding at the time of begin.
	float                   WindowBorderSize;                   // Window border size at the time of begin.
	GuiID                 MoveId;                             // == window->GetID("#MOVE")
	ImVec2                  Scroll;
	ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
	ImVec2                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
	bool                    ScrollbarX, ScrollbarY;
	ImVec2                  ScrollbarSizes;
	bool                    Active;                             // Set to true on Begin(), unless Collapsed
	bool                    WasActive;
	bool                    WriteAccessed;                      // Set to true when any widget access the current window
	bool                    Collapsed;                          // Set when collapsing window to become only title-bar
	bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
	bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
	bool                    CloseButton;                        // Set when the window has a close button (p_open != NULL)
	int                     BeginOrderWithinParent;             // Order within immediate parent window, if we are a child window. Otherwise 0.
	int                     BeginOrderWithinContext;            // Order within entire Gui context. This is mostly used for debugging submission order related issues.
	int                     BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
	GuiID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
	int                     AutoFitFramesX, AutoFitFramesY;
	bool                    AutoFitOnlyGrows;
	int                     AutoFitChildAxises;
	GuiDir                AutoPosLastDirection;
	int                     HiddenFrames;
	GuiCond               SetWindowPosAllowFlags;             // store condition flags for next SetWindowPos() call.
	GuiCond               SetWindowSizeAllowFlags;            // store condition flags for next SetWindowSize() call.
	GuiCond               SetWindowCollapsedAllowFlags;       // store condition flags for next SetWindowCollapsed() call.
	ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
	ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning. ImVec2(0,0) when positioning from top-left corner; ImVec2(0.5f,0.5f) for centering; ImVec2(1,1) for bottom right.

	GuiDrawContext        DC;                                 // Temporary per-window data, reset at the beginning of the frame
	ImVector<GuiID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack
	ImRect                  ClipRect;                           // = DrawList->clip_rect_stack.back(). Scissoring / clipping rectangle. x1, y1, x2, y2.
	ImRect                  WindowRectClipped;                  // = WindowRect just after setup in Begin(). == window->Rect() for root window.
	ImRect                  InnerRect;
	int                     LastFrameActive;
	float                   ItemWidthDefault;
	GuiMenuColumns        MenuColumns;                        // Simplified columns storage for menu items
	GuiStorage            StateStorage;
	ImVector<GuiColumnsSet> ColumnsStorage;
	float                   FontWindowScale;                    // Scale multiplier per-window
	ImDrawList*             DrawList;
	GuiWindow*            ParentWindow;                       // If we are a child _or_ popup window, this is pointing to our parent. Otherwise NULL.
	GuiWindow*            RootWindow;                         // Generally point to ourself. If we are a child window, this is pointing to the first non-child parent window.
	GuiWindow*            RootNonPopupWindow;                 // Generally point to ourself. Used to display TitleBgActive color and for selecting which window to use for NavWindowing

																// Navigation / Focus
	int                     FocusIdxAllCounter;                 // Start at -1 and increase as assigned via FocusItemRegister()
	int                     FocusIdxTabCounter;                 // (same, but only count widgets which you can Tab through)
	int                     FocusIdxAllRequestCurrent;          // Item being requested for focus
	int                     FocusIdxTabRequestCurrent;          // Tab-able item being requested for focus
	int                     FocusIdxAllRequestNext;             // Item being requested for focus, for next update (relies on layout to be stable between the frame pressing TAB and the next frame)
	int                     FocusIdxTabRequestNext;             // "

public:
	GuiWindow(GuiContext* context, const char* name);
	~GuiWindow();

	GuiID     GetID(const char* str, const char* str_end = NULL);
	GuiID     GetID(const void* ptr);
	GuiID     GetIDNoKeepAlive(const char* str, const char* str_end = NULL);
	GuiID     GetIDFromRectangle(const ImRect& r_abs);

	// We don't use g.FontSize because the window may be != g.CurrentWidow.
	ImRect      Rect() const { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
	float       CalcFontSize() const { return GGui->FontBaseSize * FontWindowScale; }
	float       TitleBarHeight() const { return (Flags & GuiWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + GGui->Style.FramePadding.y * 2.0f; }
	ImRect      TitleBarRect() const { return ImRect(Pos, ImVec2(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
	float       MenuBarHeight() const { return (Flags & GuiWindowFlags_MenuBar) ? CalcFontSize() + GGui->Style.FramePadding.y * 2.0f : 0.0f; }
	ImRect      MenuBarRect() const { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

// Backup and restore just enough data to be able to use IsItemHovered() on item A after another B in the same window has overwritten the data.  
struct GuiItemHoveredDataBackup
{
	GuiID     LastItemId;
	ImRect      LastItemRect;
	bool        LastItemRectHoveredRect;

	GuiItemHoveredDataBackup() { Backup(); }
	void Backup() { GuiWindow* window = GGui->CurrentWindow; LastItemId = window->DC.LastItemId; LastItemRect = window->DC.LastItemRect; LastItemRectHoveredRect = window->DC.LastItemRectHoveredRect; }
	void Restore() const { GuiWindow* window = GGui->CurrentWindow; window->DC.LastItemId = LastItemId; window->DC.LastItemRect = LastItemRect; window->DC.LastItemRectHoveredRect = LastItemRectHoveredRect; }
};

//-----------------------------------------------------------------------------
// Internal API
// No guarantee of forward compatibility here.
//-----------------------------------------------------------------------------

namespace Gui
{
	// We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
	// If this ever crash because g.CurrentWindow is NULL it means that either
	// - Gui::NewFrame() has never been called, which is illegal.
	// - You are calling Gui functions after Gui::Render() and before the next Gui::NewFrame(), which is also illegal.
	inline    GuiWindow*  GetCurrentWindowRead() { GuiContext& g = *GGui; return g.CurrentWindow; }
	inline    GuiWindow*  GetCurrentWindow() { GuiContext& g = *GGui; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
	Gui_API GuiWindow*  FindWindowByName(const char* name);
	Gui_API void          FocusWindow(GuiWindow* window);
	Gui_API void          BringWindowToFront(GuiWindow* window);
	Gui_API void          BringWindowToBack(GuiWindow* window);
	Gui_API bool          IsWindowChildOf(GuiWindow* window, GuiWindow* potential_parent);

	Gui_API void          Initialize();

	Gui_API void                  MarkIniSettingsDirty();
	Gui_API GuiSettingsHandler* FindSettingsHandler(const char* type_name);
	Gui_API GuiWindowSettings*  FindWindowSettings(GuiID id);

	Gui_API void          SetActiveID(GuiID id, GuiWindow* window);
	Gui_API void          ClearActiveID();
	Gui_API void          SetHoveredID(GuiID id);
	Gui_API GuiID       GetHoveredID();
	Gui_API void          KeepAliveID(GuiID id);

	Gui_API void          ItemSize(const ImVec2& size, float text_offset_y = 0.0f);
	Gui_API void          ItemSize(const ImRect& bb, float text_offset_y = 0.0f);
	Gui_API bool          ItemAdd(const ImRect& bb, GuiID id);
	Gui_API bool          ItemHoverable(const ImRect& bb, GuiID id);
	Gui_API bool          IsClippedEx(const ImRect& bb, GuiID id, bool clip_even_when_logged);
	Gui_API bool          FocusableItemRegister(GuiWindow* window, GuiID id, bool tab_stop = true);      // Return true if focus is requested
	Gui_API void          FocusableItemUnregister(GuiWindow* window);
	Gui_API ImVec2        CalcItemSize(ImVec2 size, float default_x, float default_y);
	Gui_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
	Gui_API void          PushMultiItemsWidths(int components, float width_full = 0.0f);
	Gui_API void          PushItemFlag(GuiItemFlags option, bool enabled);
	Gui_API void          PopItemFlag();

	Gui_API void          OpenPopupEx(GuiID id);
	Gui_API void          ClosePopup(GuiID id);
	Gui_API bool          IsPopupOpen(GuiID id);
	Gui_API bool          BeginPopupEx(GuiID id, GuiWindowFlags extra_flags);
	Gui_API void          BeginTooltipEx(GuiWindowFlags extra_flags, bool override_previous_tooltip = true);

	Gui_API int           CalcTypematicPressedRepeatAmount(float t, float t_prev, float repeat_delay, float repeat_rate);

	Gui_API void          Scrollbar(GuiLayoutType direction);
	Gui_API void          VerticalSeparator();        // Vertical separator, for menu bars (use current line height). not exposed because it is misleading what it doesn't have an effect on regular layout.
	Gui_API bool          SplitterBehavior(GuiID id, const ImRect& bb, GuiAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f);

	Gui_API bool          BeginDragDropTargetCustom(const ImRect& bb, GuiID id);
	Gui_API void          ClearDragDrop();
	Gui_API bool          IsDragDropPayloadBeingAccepted();

	// FIXME-WIP: New Columns API
	Gui_API void          BeginColumns(const char* str_id, int count, GuiColumnsFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
	Gui_API void          EndColumns();                                                             // close columns
	Gui_API void          PushColumnClipRect(int column_index = -1);

	// NB: All position are in absolute pixels coordinates (never using window coordinates internally)
	// AVOID USING OUTSIDE OF Gui.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
	Gui_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
	Gui_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
	Gui_API void          RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
	Gui_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
	Gui_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
	Gui_API void          RenderColorRectWithAlphaCheckerboard(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, int rounding_corners_flags = ~0);
	Gui_API void          RenderTriangle(ImVec2 pos, GuiDir dir, float scale = 1.0f);
	Gui_API void          RenderBullet(ImVec2 pos);
	Gui_API void          RenderCheckMark(ImVec2 pos, ImU32 col, float sz);
	Gui_API void          RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
	Gui_API const char*   FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.

	Gui_API bool          ButtonBehavior(const ImRect& bb, GuiID id, bool* out_hovered, bool* out_held, GuiButtonFlags flags = 0);
	Gui_API bool          ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), GuiButtonFlags flags = 0);
	Gui_API bool          CloseButton(GuiID id, const ImVec2& pos, float radius);
	Gui_API bool          ArrowButton(GuiID id, GuiDir dir, ImVec2 padding, GuiButtonFlags flags = 0);

	Gui_API bool          SliderBehavior(const ImRect& frame_bb, GuiID id, float* v, float v_min, float v_max, float power, int decimal_precision, GuiSliderFlags flags = 0);
	Gui_API bool          SliderFloatN(const char* label, float* v, int components, float v_min, float v_max, const char* display_format, float power);
	Gui_API bool          SliderIntN(const char* label, int* v, int components, int v_min, int v_max, const char* display_format);

	Gui_API bool          DragBehavior(const ImRect& frame_bb, GuiID id, float* v, float v_speed, float v_min, float v_max, int decimal_precision, float power);
	Gui_API bool          DragFloatN(const char* label, float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power);
	Gui_API bool          DragIntN(const char* label, int* v, int components, float v_speed, int v_min, int v_max, const char* display_format);

	Gui_API bool          InputTextEx(const char* label, char* buf, int buf_size, const ImVec2& size_arg, GuiInputTextFlags flags, GuiTextEditCallback callback = NULL, void* user_data = NULL);
	Gui_API bool          InputFloatN(const char* label, float* v, int components, int decimal_precision, GuiInputTextFlags extra_flags);
	Gui_API bool          InputIntN(const char* label, int* v, int components, GuiInputTextFlags extra_flags);
	Gui_API bool          InputScalarEx(const char* label, GuiDataType data_type, void* data_ptr, void* step_ptr, void* step_fast_ptr, const char* scalar_format, GuiInputTextFlags extra_flags);
	Gui_API bool          InputScalarAsWidgetReplacement(const ImRect& aabb, const char* label, GuiDataType data_type, void* data_ptr, GuiID id, int decimal_precision);

	Gui_API void          ColorTooltip(const char* text, const float* col, GuiColorEditFlags flags);
	Gui_API void          ColorEditOptionsPopup(const float* col, GuiColorEditFlags flags);

	Gui_API bool          TreeNodeBehavior(GuiID id, GuiTreeNodeFlags flags, const char* label, const char* label_end = NULL);
	Gui_API bool          TreeNodeBehaviorIsOpen(GuiID id, GuiTreeNodeFlags flags = 0);                     // Consume previous SetNextTreeNodeOpened() data, if any. May return true when logging
	Gui_API void          TreePushRawID(GuiID id);

	Gui_API void          PlotEx(GuiPlotType plot_type, const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size);

	Gui_API int           ParseFormatPrecision(const char* fmt, int default_value);
	Gui_API float         RoundScalar(float value, int decimal_precision);

	// Shade functions
	Gui_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawVert* vert_start, ImDrawVert* vert_end, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
	Gui_API void          ShadeVertsLinearAlphaGradientForLeftToRightText(ImDrawVert* vert_start, ImDrawVert* vert_end, float gradient_p0_x, float gradient_p1_x);
	Gui_API void          ShadeVertsLinearUV(ImDrawVert* vert_start, ImDrawVert* vert_end, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp);

} // namespace Gui

  // ImFontAtlas internals
Gui_API bool              ImFontAtlasBuildWithStbTruetype(ImFontAtlas* atlas);
Gui_API void              ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas* atlas);
Gui_API void              ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent);
Gui_API void              ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* spc);
Gui_API void              ImFontAtlasBuildFinish(ImFontAtlas* atlas);
Gui_API void              ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
Gui_API void              ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif
