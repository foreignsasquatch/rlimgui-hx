/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   rlImGui * basic ImGui integration
*
*   LICENSE: ZLIB
*
*   Copyright (c) 2020 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/
#include "rlImGui.h"

#include "imgui.h"
#include "raylib.h"
#include "rlgl.h"

#ifdef PLATFORM_DESKTOP
#include <GLFW/glfw3.h>
#endif

#include <math.h>

#include "fontawesome.h"

static Texture2D FontTexture;

static ImGuiMouseCursor CurrentMouseCursor = ImGuiMouseCursor_COUNT;
static MouseCursor MouseCursorMap[ImGuiMouseCursor_COUNT];

static const char* rlImGuiGetClipText(void*)
{
    return GetClipboardText();
}

static void rlImGuiSetClipText(void*, const char* text)
{
    SetClipboardText(text);
}

static void rlImGuiNewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    if (IsWindowFullscreen())
    {
        int monitor = GetCurrentMonitor();
        io.DisplaySize.x = float(GetMonitorWidth(monitor));
        io.DisplaySize.y = float(GetMonitorHeight(monitor));
    }
    else
    {
        io.DisplaySize.x = float(GetScreenWidth());
        io.DisplaySize.y = float(GetScreenHeight());
    }

    int width = int(io.DisplaySize.x), height = int(io.DisplaySize.y);
#ifdef PLATFORM_DESKTOP
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
#endif
    if (width > 0 && height > 0) {
        io.DisplayFramebufferScale = ImVec2(width / io.DisplaySize.x, height / io.DisplaySize.y);
    }
    else {
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    io.DeltaTime = GetFrameTime();

    io.KeyCtrl = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    io.KeyShift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
    io.KeyAlt = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
    io.KeySuper = IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);

    if (io.WantSetMousePos)
    {
        SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y);
    }
    else
    {
        io.MousePos.x = (float)GetMouseX();
        io.MousePos.y = (float)GetMouseY();
    }

    io.MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

	{
		Vector2 mouseWheel = GetMouseWheelMoveV();
		io.MouseWheel += mouseWheel.y;
		io.MouseWheelH += mouseWheel.x;
	}

    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
    {
        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor != CurrentMouseCursor || io.MouseDrawCursor)
        {
            CurrentMouseCursor = imgui_cursor;
            if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
            {
                HideCursor();
            }
            else
            {
                ShowCursor();

                if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
                {
		    SetMouseCursor((imgui_cursor > -1 && imgui_cursor < ImGuiMouseCursor_COUNT) ? MouseCursorMap[imgui_cursor] : MOUSE_CURSOR_DEFAULT);
                }
            }
        }
    }
}

#define FOR_ALL_KEYS(X) \
    do { \
    X(KEY_APOSTROPHE); \
    X(KEY_COMMA); \
    X(KEY_MINUS); \
    X(KEY_PERIOD); \
    X(KEY_SLASH); \
    X(KEY_ZERO); \
    X(KEY_ONE); \
    X(KEY_TWO); \
    X(KEY_THREE); \
    X(KEY_FOUR); \
    X(KEY_FIVE); \
    X(KEY_SIX); \
    X(KEY_SEVEN); \
    X(KEY_EIGHT); \
    X(KEY_NINE); \
    X(KEY_SEMICOLON); \
    X(KEY_EQUAL); \
    X(KEY_A); \
    X(KEY_B); \
    X(KEY_C); \
    X(KEY_D); \
    X(KEY_E); \
    X(KEY_F); \
    X(KEY_G); \
    X(KEY_H); \
    X(KEY_I); \
    X(KEY_J); \
    X(KEY_K); \
    X(KEY_L); \
    X(KEY_M); \
    X(KEY_N); \
    X(KEY_O); \
    X(KEY_P); \
    X(KEY_Q); \
    X(KEY_R); \
    X(KEY_S); \
    X(KEY_T); \
    X(KEY_U); \
    X(KEY_V); \
    X(KEY_W); \
    X(KEY_X); \
    X(KEY_Y); \
    X(KEY_Z); \
    X(KEY_SPACE); \
    X(KEY_ESCAPE); \
    X(KEY_ENTER); \
    X(KEY_TAB); \
    X(KEY_BACKSPACE); \
    X(KEY_INSERT); \
    X(KEY_DELETE); \
    X(KEY_RIGHT); \
    X(KEY_LEFT); \
    X(KEY_DOWN); \
    X(KEY_UP); \
    X(KEY_PAGE_UP); \
    X(KEY_PAGE_DOWN); \
    X(KEY_HOME); \
    X(KEY_END); \
    X(KEY_CAPS_LOCK); \
    X(KEY_SCROLL_LOCK); \
    X(KEY_NUM_LOCK); \
    X(KEY_PRINT_SCREEN); \
    X(KEY_PAUSE); \
    X(KEY_F1); \
    X(KEY_F2); \
    X(KEY_F3); \
    X(KEY_F4); \
    X(KEY_F5); \
    X(KEY_F6); \
    X(KEY_F7); \
    X(KEY_F8); \
    X(KEY_F9); \
    X(KEY_F10); \
    X(KEY_F11); \
    X(KEY_F12); \
    X(KEY_LEFT_SHIFT); \
    X(KEY_LEFT_CONTROL); \
    X(KEY_LEFT_ALT); \
    X(KEY_LEFT_SUPER); \
    X(KEY_RIGHT_SHIFT); \
    X(KEY_RIGHT_CONTROL); \
    X(KEY_RIGHT_ALT); \
    X(KEY_RIGHT_SUPER); \
    X(KEY_KB_MENU); \
    X(KEY_LEFT_BRACKET); \
    X(KEY_BACKSLASH); \
    X(KEY_RIGHT_BRACKET); \
    X(KEY_GRAVE); \
    X(KEY_KP_0); \
    X(KEY_KP_1); \
    X(KEY_KP_2); \
    X(KEY_KP_3); \
    X(KEY_KP_4); \
    X(KEY_KP_5); \
    X(KEY_KP_6); \
    X(KEY_KP_7); \
    X(KEY_KP_8); \
    X(KEY_KP_9); \
    X(KEY_KP_DECIMAL); \
    X(KEY_KP_DIVIDE); \
    X(KEY_KP_MULTIPLY); \
    X(KEY_KP_SUBTRACT); \
    X(KEY_KP_ADD); \
    X(KEY_KP_ENTER); \
    X(KEY_KP_EQUAL); \
    } while(0)

#define SET_KEY_DOWN(KEY) io.KeysDown[KEY] = IsKeyDown(KEY)


static void rlImGuiEvents()
{
    ImGuiIO& io = ImGui::GetIO();
    FOR_ALL_KEYS(SET_KEY_DOWN);

    unsigned int pressed = GetCharPressed();
    if (pressed != 0)
        io.AddInputCharacter(pressed);
}

static void rlImGuiTriangleVert(ImDrawVert& idx_vert)
{
    Color* c;
    c = (Color*)&idx_vert.col;
    rlColor4ub(c->r, c->g, c->b, c->a);
    rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
    rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
}

static void rlImGuiRenderTriangles(unsigned int count, int indexStart, const ImVector<ImDrawIdx>& indexBuffer, const ImVector<ImDrawVert>& vertBuffer, void* texturePtr)
{
    if (count < 3)
        return;
	
    Texture* texture = (Texture*)texturePtr;

    unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

    rlBegin(RL_TRIANGLES);
    rlSetTexture(textureId);

    for (unsigned int i = 0; i <= (count - 3); i += 3)
    {
        if(rlCheckRenderBatchLimit(3)) 
        {
            rlBegin(RL_TRIANGLES);
            rlSetTexture(textureId);
        }

        ImDrawIdx indexA = indexBuffer[indexStart + i];
        ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
        ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

        ImDrawVert vertexA = vertBuffer[indexA];
        ImDrawVert vertexB = vertBuffer[indexB];
        ImDrawVert vertexC = vertBuffer[indexC];

        rlImGuiTriangleVert(vertexA);
        rlImGuiTriangleVert(vertexB);
        rlImGuiTriangleVert(vertexC);
    }
    rlEnd();
}

static void EnableScissor(float x, float y, float width, float height)
{
    rlEnableScissorTest();
    ImGuiIO& io = ImGui::GetIO();
    rlScissor((int)(x * io.DisplayFramebufferScale.x),
		int((GetScreenHeight() - (int)(y + height)) * io.DisplayFramebufferScale.y),
		(int)(width * io.DisplayFramebufferScale.x),
		(int)(height * io.DisplayFramebufferScale.y));
}

static void rlRenderData(ImDrawData* data)
{
    rlDrawRenderBatchActive();
    rlDisableBackfaceCulling();

    for (int l = 0; l < data->CmdListsCount; ++l)
    {
        const ImDrawList* commandList = data->CmdLists[l];

        for (const auto& cmd : commandList->CmdBuffer)
        {
            EnableScissor(cmd.ClipRect.x - data->DisplayPos.x, cmd.ClipRect.y - data->DisplayPos.y, cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x), cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y));
            if (cmd.UserCallback != nullptr)
            {
                cmd.UserCallback(commandList, &cmd);
  
                continue;
            }

            rlImGuiRenderTriangles(cmd.ElemCount, cmd.IdxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
            rlDrawRenderBatchActive();
        }
    }

    rlSetTexture(0);
    rlDisableScissorTest();
    rlEnableBackfaceCulling();
}

void SetupMouseCursors()
{
    MouseCursorMap[ImGuiMouseCursor_Arrow] = MOUSE_CURSOR_ARROW;
    MouseCursorMap[ImGuiMouseCursor_TextInput] = MOUSE_CURSOR_IBEAM;
    MouseCursorMap[ImGuiMouseCursor_Hand] = MOUSE_CURSOR_POINTING_HAND;
    MouseCursorMap[ImGuiMouseCursor_ResizeAll] = MOUSE_CURSOR_RESIZE_ALL;
    MouseCursorMap[ImGuiMouseCursor_ResizeEW] = MOUSE_CURSOR_RESIZE_EW;
    MouseCursorMap[ImGuiMouseCursor_ResizeNESW] = MOUSE_CURSOR_RESIZE_NESW;
    MouseCursorMap[ImGuiMouseCursor_ResizeNS] = MOUSE_CURSOR_RESIZE_NS;
    MouseCursorMap[ImGuiMouseCursor_ResizeNWSE] = MOUSE_CURSOR_RESIZE_NWSE;
    MouseCursorMap[ImGuiMouseCursor_NotAllowed] = MOUSE_CURSOR_NOT_ALLOWED;
}

void rlSetupFontAwesome()
{
    static const ImWchar icons_ranges[] = { 0xe005, 0xf8ff, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;

    // icons_config.GlyphMaxAdvanceX = std::numeric_limits<float>::max();
    icons_config.RasterizerMultiply = 1.0f;
    icons_config.OversampleH = 2;
    icons_config.OversampleV = 1;

    icons_config.GlyphRanges = icons_ranges;

    ImGuiIO& io = ImGui::GetIO();

    float size = 11;
#if !defined(__APPLE__)
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI))
        size *= GetWindowScaleDPI().y;


	icons_config.RasterizerMultiply = GetWindowScaleDPI().y;
#endif

    io.Fonts->AddFontFromMemoryCompressedTTF((void*)fa_solid_900_compressed_data, fa_solid_900_compressed_size, size, &icons_config, icons_ranges);
}

void rlImGuiBeginInitImGui()
{
    ImGui::CreateContext(nullptr);
}

void rlImGuiEndInitImGui()
{
    SetupMouseCursors();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_raylib";

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_Home] = KEY_HOME;
    io.KeyMap[ImGuiKey_End] = KEY_END;
    io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = KEY_A;
    io.KeyMap[ImGuiKey_C] = KEY_C;
    io.KeyMap[ImGuiKey_V] = KEY_V;
    io.KeyMap[ImGuiKey_X] = KEY_X;
    io.KeyMap[ImGuiKey_Y] = KEY_Y;
    io.KeyMap[ImGuiKey_Z] = KEY_Z;

    io.MousePos = ImVec2(0, 0);

    io.SetClipboardTextFn = rlImGuiSetClipText;
    io.GetClipboardTextFn = rlImGuiGetClipText;

    io.ClipboardUserData = nullptr;


    rlImGuiReloadFonts();
}

void rlImGuiSetup(bool dark)
{
    rlImGuiBeginInitImGui();

    if (dark)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsLight();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

#ifndef NO_FONT_AWESOME
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryCompressedTTF((void*)fa_solid_900_compressed_data, fa_solid_900_compressed_size, FONT_AWESOME_ICON_SIZE, &icons_config, icons_ranges);
#endif

    rlImGuiEndInitImGui();
}

void rlImGuiReloadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels = nullptr;

    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, nullptr);
    Image image = GenImageColor(width, height, BLANK);
    memcpy(image.data, pixels, width * height * 4);

    if (FontTexture.id != 0)
        UnloadTexture(FontTexture);

    FontTexture = LoadTextureFromImage(image);
    UnloadImage(image);
    io.Fonts->TexID = &FontTexture;
}

void rlImGuiBegin()
{
    rlImGuiNewFrame();
    rlImGuiEvents();
    ImGui::NewFrame();
}

void rlImGuiEnd()
{
    ImGui::Render();
    rlRenderData(ImGui::GetDrawData());
}

void rlImGuiShutdown()
{
    UnloadTexture(FontTexture);

    ImGui::DestroyContext();
}

void rlImGuiImage(const Texture *image)
{
    ImGui::Image((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

bool rlImGuiImageButton(const Texture *image) {
    return ImGui::ImageButton((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

void rlImGuiImageSize(const Texture *image, int width, int height)
{
    ImGui::Image((ImTextureID)image, ImVec2(float(width), float(height)));
}

void rlImGuiImageRect(const Texture* image, int destWidth, int destHeight, Rectangle sourceRect)
{
    ImVec2 uv0;
    ImVec2 uv1;

    if (sourceRect.width < 0)
    {
        uv0.x = -((float)sourceRect.x / image->width);
        uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / image->width));
    }
    else
    {
        uv0.x = (float)sourceRect.x / image->width;
        uv1.x = uv0.x + (float)(sourceRect.width / image->width);
    }

    if (sourceRect.height < 0)
    {
        uv0.y = -((float)sourceRect.y / image->height);
        uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / image->height));
    }
    else
    {
        uv0.y = (float)sourceRect.y / image->height;
        uv1.y = uv0.y + (float)(sourceRect.height / image->height);
    }

    ImGui::Image((ImTextureID)image, ImVec2(float(destWidth), float(destHeight)),uv0,uv1);
}

void rlImGuiImageRenderTexture(const RenderTexture* image)
{
    if (!image)
        return;

    rlImGuiImageRect(&image->texture, image->texture.width, image->texture.height, Rectangle{ 0,0, float(image->texture.width), -float(image->texture.height) });
}

void rlImGuiImageRenderTextureFit(const RenderTexture* image, bool center)
{
	if (!image)
		return;

    ImVec2 area = ImGui::GetContentRegionAvail();

    float scale =  area.x / image->texture.width;

    float y = image->texture.height * scale;
    if (y > area.y)
    {
        scale = area.y / image->texture.height;
    }

    int sizeX = int(image->texture.width * scale);
    int sizeY = int(image->texture.height * scale);

    if (center)
    {
        ImGui::SetCursorPosX(0);
        ImGui::SetCursorPosX(area.x/2 - sizeX/2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (area.y / 2 - sizeY / 2));
    }

    rlImGuiImageRect(&image->texture, sizeX, sizeY, Rectangle{ 0,0, float(image->texture.width), -float(image->texture.height) });
}