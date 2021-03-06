
#include "Engine/EngineCommon.h"

#include "Foundation/FileSystem/File.h"

#include "Engine/Text/TextManager.h"
#include "Engine/Text/TextRenderer.h"
#include "Engine/Text/TextShaders.h"
#include "Engine/Input/TextInputContext.h"
#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/RenderErrorMacros.h"
#include "Engine/Rendering/RenderState.h"
#include "Engine/Asset/AssetLoader.h"

#include <codecvt>

TextManager g_TextManager;

static std::map<int, std::unique_ptr<TextRenderer>> s_Fonts;
std::vector<std::unique_ptr<TextBackupFont>> s_BackupFonts;

TextManager::TextManager() :
  m_TextVertexBuffer(true)
{

}

void TextManager::Init()
{
  m_TextShader = MakeQuickShaderProgram(kBasicTextVertexShader, kBasicTextFragmentShader);

#ifdef _MSC_VER
  //LoadBackupFont("C:\\Windows\\Fonts\\arial.ttf");
  //LoadBackupFont("C:\\Windows\\Fonts\\gulim.ttc");
#endif

  SetTextMode();
  SetPrimaryColor();
  SetShadowColor();
  SetSelectionColor();
  SetSelectionBkgColor();
}

void TextManager::ShutDown()
{
  s_Fonts.clear();
}

void TextManager::LoadFont(czstr font_path, int font_id, int font_size)
{
  s_Fonts.emplace(std::make_pair(font_id, std::make_unique<TextRenderer>(FontAsset::Load(font_path), font_size, s_BackupFonts)));
}

bool TextManager::IsFontLoaded(int font_id)
{
  auto itr = s_Fonts.find(font_id);
  if (itr == s_Fonts.end())
  {
    return false ;
  }

  auto & font = itr->second;
  return font->Loaded();
}

void TextManager::AddTextToBuffer(czstr text, int font_id, TextBufferBuilder & vertex_builder, int sel_start, int sel_end, int cursor_pos)
{
  auto itr = s_Fonts.find(font_id);
  if (itr == s_Fonts.end())
  {
    return;
  }

  auto & font = itr->second;
  if (font->Loaded() == false)
  {
    return;
  }

  if (vertex_builder.m_FontId == kInvalidFontId)
  {
    vertex_builder.m_FontId = font_id;
  }
  else if (vertex_builder.m_FontId != font_id)
  {
    throw std::runtime_error("Cannot use a single text buffer with multiple fonts");
  }

  m_GlyphPositions.clear();
  font->CreateVertexBufferForString(text, strlen(text), sel_start, sel_end, cursor_pos, m_Settings, vertex_builder, m_GlyphPositions);
}

void TextManager::AddTextToBuffer(std::shared_ptr<TextInputContext> & context, int font_id, TextBufferBuilder & vertex_builder, const char * prompt)
{
  std::string text = prompt;
  std::size_t prompt_length = TextInputContext::GetMultibyteLength(prompt);
  text += context->GetCurrentInput();

  int cursor_pos = (int)(context->GetCursorPos() + prompt_length);

  auto & compo = context->GetCurrentComposition();
  std::size_t compo_size = context->GetMultibyteLength(compo.data());

  double s;
  bool show_cursor = std::modf(context->GetTimeSinceLastUpdate(), &s) <= 0.5;

  if (compo_size)
  {
    text.insert(text.begin() + prompt_length + context->GetCharacterByteOffset(context->GetCursorPos()), compo.begin(), compo.end());
    AddTextToBuffer(text.data(), font_id, vertex_builder, cursor_pos, (int)(cursor_pos + compo_size), show_cursor ? (int)(cursor_pos + compo_size) : -1);
  }
  else
  {
    AddTextToBuffer(text.data(), font_id, vertex_builder, -1, -1, cursor_pos);
  }
}

void TextManager::RenderBuffer(TextBufferBuilder & vertex_builder, RenderState & render_state)
{
  if (vertex_builder.m_FontId == kInvalidFontId)
  {
    return;
  }

  auto itr = s_Fonts.find(vertex_builder.m_FontId);
  if (itr == s_Fonts.end())
  {
    return;
  }

  auto & font = itr->second;
  if (font->Loaded() == false)
  {
    return;
  }

  RenderVec4 screen_bounds = { -1, -1, 1, 1 };
  if (m_Settings.m_TextBounds)
  {
    screen_bounds.x = (float)m_Settings.m_TextBounds->m_Start.x / (float)render_state.GetRenderWidth();
    screen_bounds.y = (float)m_Settings.m_TextBounds->m_Start.y / (float)render_state.GetRenderHeight();
    screen_bounds.w = (float)m_Settings.m_TextBounds->m_End.x / (float)render_state.GetRenderWidth();
    screen_bounds.z = (float)m_Settings.m_TextBounds->m_End.y / (float)render_state.GetRenderHeight();

    screen_bounds -= RenderVec4{ 0.5f, 0.5f, 0.5f, 0.5f };
    screen_bounds *= 2.0f;
  }

  render_state.EnableBlendMode();

  m_TextVertexBuffer.SetBufferData(vertex_builder.m_Verts, VertexBufferType::kTriangles);

  m_TextShader.Bind();
  m_TextVertexBuffer.Bind();
  m_TextVertexBuffer.CreateDefaultBinding(m_TextShader);

  font->BindGlyphTexture(0);

  m_TextShader.SetUniform(COMPILE_TIME_CRC32_STR("u_GlyphTexture"), 0);
  m_TextShader.SetUniform(COMPILE_TIME_CRC32_STR("u_ScreenSize"), (float)render_state.GetRenderWidth(), (float)render_state.GetRenderHeight());
  m_TextShader.SetUniform(COMPILE_TIME_CRC32_STR("u_Bounds"), screen_bounds);

  m_TextVertexBuffer.Draw();

  m_TextVertexBuffer.Unbind();
  m_TextShader.Unbind();
}

void TextManager::RenderText(czstr text, int font_id, RenderState & render_state, int sel_start, int sel_end, int cursor_pos)
{
  TextBufferBuilder buffer_builder;
  AddTextToBuffer(text, font_id, buffer_builder, sel_start, sel_end, cursor_pos);
  RenderBuffer(buffer_builder, render_state);
}

void TextManager::RenderInputText(std::shared_ptr<TextInputContext> & context, int font_id, RenderState & render_state, const char * prompt)
{
  TextBufferBuilder buffer_builder;
  AddTextToBuffer(context, font_id, buffer_builder, prompt);
  RenderBuffer(buffer_builder, render_state);
}

Box TextManager::GetTextSize(czstr text, int font_id)
{
  auto itr = s_Fonts.find(font_id);
  if (itr == s_Fonts.end())
  {
    return{};
  }

  auto & font = itr->second;
  if (font->Loaded() == false)
  {
    return{};
  }

  return font->GetTextSize(text, strlen(text));
}

Box TextManager::GetTextSize(std::shared_ptr<TextInputContext> & context, int font_id, const char * prompt)
{
  std::string text = prompt;
  std::size_t prompt_length = TextInputContext::GetMultibyteLength(prompt);
  text += context->GetCurrentInput();

  int cursor_pos = (int)(context->GetCursorPos() + prompt_length);

  auto & compo = context->GetCurrentComposition();
  std::size_t compo_size = context->GetMultibyteLength(compo.data());

  double s;
  bool show_cursor = std::modf(context->GetTimeSinceLastUpdate(), &s) <= 0.5;

  if (compo_size)
  {
    text.insert(text.begin() + prompt_length + context->GetCharacterByteOffset(context->GetCursorPos()), compo.begin(), compo.end());
    return GetTextSize(text.data(), font_id);
  }
  else
  {
    return GetTextSize(text.data(), font_id);
  }
}

bool TextManager::BindGlyphTexture(int font_id, int texture_stage)
{
  auto itr = s_Fonts.find(font_id);
  if (itr == s_Fonts.end())
  {
    return false;
  }

  auto & font = itr->second;
  if (font->Loaded() == false)
  {
    return false;
  }

  font->BindGlyphTexture(texture_stage);
  return true;
}

void TextManager::SetTextMode(TextRenderMode mode)
{
  m_Settings.m_Mode = mode;
}

void TextManager::SetTextPos(const Vector2 & pos)
{
  m_Settings.m_TextPos = pos;
  m_Settings.m_TextBounds = {};
}

void TextManager::SetTextBounds(const Box & bounds)
{
  m_Settings.m_TextBounds = bounds;
}

void TextManager::ClearTextBounds()
{
  m_Settings.m_TextBounds = {};
}

void TextManager::SetPrimaryColor(const Color & color)
{
  m_Settings.m_PrimaryColor = color;
}

void TextManager::SetShadowColor(const Color & color)
{
  m_Settings.m_ShadowColor = color;
}

void TextManager::SetSelectionColor(const Color & color)
{
  m_Settings.m_SelectionColor = color;
}

void TextManager::SetSelectionBkgColor(const Color & color)
{
  m_Settings.m_SelectionBkgColor = color;
}

void TextManager::LoadBackupFont(czstr font_path)
{
  int file_open_error;
  auto buffer = g_AssetLoader.LoadFullFile(font_path, file_open_error);

  if (buffer)
  {
    s_BackupFonts.emplace_back(std::make_unique<TextBackupFont>(std::move(*buffer)));
  }
}
