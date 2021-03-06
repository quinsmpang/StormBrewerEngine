
#include "Engine/EngineCommon.h"
#include "Engine/UI/UIElementFullTexture.refl.meta.h"
#include "Engine/UI/UIElementExprBlock.h"

#include "Engine/Rendering/VertexBufferBuilder.h"
#include "Engine/Rendering/RenderUtil.h"
#include "Engine/Shader/ShaderManager.h"

#include "Runtime/UI/UIElementTypeRegister.h"

STORM_DATA_DEFAULT_CONSTRUCTION_IMPL(UIElementFullTextureInitData);
REGISTER_UIELEMENT_DATA(UIElementFullTexture, UIElementFullTextureInitData, UIElementFullTextureData);

UIElementType UIElementFullTexture::Type = UIElementType::kFullTexture;

UIElementFullTexture::UIElementFullTexture(const UIElementFullTextureInitData & init_data, const UIElementFullTextureData & data) :
  m_InitData(init_data),
  m_Data(data),
  m_Texture(TextureAsset::Load(m_InitData.m_TextureFile.data()))
{

}

void UIElementFullTexture::Update()
{
  if (m_Texture->IsValid())
  {
    auto width = m_Texture->GetWidth();
    auto height = m_Texture->GetHeight();

    SetActiveArea(Box::FromFrameCenterAndSize(Vector2(m_Data.m_CenterX, m_Data.m_CenterY), Vector2(width, height)));
    SetOffset(Vector2(m_Data.m_CenterX, m_Data.m_CenterY) - Vector2(width, height) / 2);
  }

  UIElement::Update();

  if (m_Texture->IsValid())
  {
    auto width = m_Texture->GetWidth();
    auto height = m_Texture->GetHeight();

    SetActiveArea(Box::FromFrameCenterAndSize(Vector2(m_Data.m_CenterX, m_Data.m_CenterY), Vector2(width, height)));
    SetOffset(Vector2(m_Data.m_CenterX, m_Data.m_CenterY) - Vector2(width, height) / 2);
  }
}

void UIElementFullTexture::Render(RenderState & render_state, RenderUtil & render_util, const Vector2 & offset)
{
  if (m_RenderDelegate)
  {
    m_RenderDelegate(*this, render_state, offset);
  }
  else
  {
    RenderDefault(render_state, render_util, offset);
  }

  UIElement::Render(render_state, render_util, offset);
}

void UIElementFullTexture::RenderDefault(RenderState & render_state, RenderUtil & render_util, const Vector2 & offset)
{
  auto texture = m_Texture.Resolve();
  if (texture == nullptr || texture->IsLoaded() == false)
  {
    return;
  }

  QuadVertexBufferBuilder buffer_builder;
  Color color = Color(m_Data.m_ColorR, m_Data.m_ColorG, m_Data.m_ColorB, m_Data.m_ColorA);

  QuadVertexBuilderInfo quad;
  quad.m_Position = Box::FromFrameCenterAndSize(Vector2(m_Data.m_CenterX, m_Data.m_CenterY), texture->GetSize());
  quad.m_Color = color;
  quad.m_TexCoords = Box::FromPoints(Vector2{}, texture->GetSize());
  quad.m_TextureSize = texture->GetSize();
  buffer_builder.AddQuad(quad);

  auto & vertex_buffer = render_util.GetScratchBuffer();
  buffer_builder.FillVertexBuffer(vertex_buffer);

  auto & shader = g_ShaderManager.GetDefaultShader();
  shader.Bind();
  shader.SetUniform(COMPILE_TIME_CRC32_STR("u_Offset"), (RenderVec2)offset);

  texture->GetTexture().BindTexture(0);

  vertex_buffer.Bind();
  vertex_buffer.CreateDefaultBinding(shader);
  vertex_buffer.Draw();
}

const UIElementFullTextureInitData & UIElementFullTexture::GetInitData()
{
  return m_InitData;
}

UIElementFullTextureData & UIElementFullTexture::GetData()
{
  return m_Data;
}

void UIElementFullTexture::SetCustomRenderCallback(Delegate<void, UIElementFullTexture &, RenderState &, const Vector2 &> && render_callback)
{
  m_RenderDelegate = std::move(render_callback);
}

AssetReference<TextureAsset> & UIElementFullTexture::GetTextureAsset()
{
  return m_Texture;
}

StormExprValueInitBlock UIElementFullTexture::GetLocalBlock()
{
  return UICreateInitBlockForDataType(m_Data);
}

StormExprValueInitBlock UIElementFullTexture::GetAsParentBlock()
{
  return UICreateInitBlockForDataType(m_Data, "p.");
}

UIElementExprBindingList UIElementFullTexture::CreateBindingList()
{
  return UICreateBindingList(m_Data);
}