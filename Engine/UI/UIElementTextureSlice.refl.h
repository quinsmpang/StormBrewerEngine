#pragma once

#include "Engine/EngineCommon.h"
#include "Engine/UI/UIElement.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/Rendering/VertexBuffer.h"

struct UIElementTextureSliceInitData
{
  STORM_REFL;

  RString STORM_REFL_ATTR_VAL(file, image) m_SpriteFile;

  RBool m_SliceHorizontally = false;
  RBool m_SliceVertically = false;

  RInt m_StartX = 0;
  RInt m_StartY = 0;
  RInt m_EndX = 10;
  RInt m_EndY = 10;

  RInt m_StartCutX = 3;
  RInt m_StartCutY = 3;
  RInt m_EndCutX = 3;
  RInt m_EndCutY = 3;
};

struct UIElementTextureSliceData
{
  STORM_REFL;

  float m_StartX = 0.0f;
  float m_StartY = 0.0f;
  float m_EndX = 0.0f;
  float m_EndY = 0.0f;
  float m_Angle = 0.0f;
  float m_Enabled = 1.0f;

  float m_ColorR = 1.0f;
  float m_ColorG = 1.0f;
  float m_ColorB = 1.0f;
  float m_ColorA = 1.0f;

  float m_Active = 0.0f;
};

class RenderState;

class UIElementTextureSlice : public UIElement
{
public:
  UIElementTextureSlice(const UIElementTextureSliceInitData & init_data, const UIElementTextureSliceData & data = {});

  static int Type;

  void Update() override;
  void Render(RenderState & render_state, RenderUtil & render_util) override;
  void RenderDefault(RenderState & render_state, RenderUtil & render_util);

  const UIElementTextureSliceInitData & GetInitData();
  UIElementTextureSliceData & GetData();

  void SetCustomRenderCallback(Delegate<void, UIElementTextureSlice &, RenderState &> && render_callback);

  AssetReference<TextureAsset> & GetTextureAsset();

private:

  UIElementTextureSliceInitData m_InitData;
  UIElementTextureSliceData m_Data;

  Delegate<void, UIElementTextureSlice &, RenderState &> m_RenderDelegate;
  AssetReference<TextureAsset> m_Texture;

  VertexBuffer m_VertexBuffer;
};
