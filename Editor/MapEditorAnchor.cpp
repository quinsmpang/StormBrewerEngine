
#include "StormData/StormDataJson.h"

#include "Foundation/BasicTypes/BasicTypeFuncs.h"
#include "Foundation/Math/Intersection.h"

#include "Runtime/Map/MapTileJson.h"
#include "Runtime/Map/MapDef.refl.meta.h"

#include "Engine/Rendering/VertexBufferBuilder.h"
#include "Engine/Rendering/RenderUtil.h"
#include "Engine/Rendering/VertexList.h"
#include "Engine/Shader/ShaderManager.h"

#include "MapEditorAnchor.h"
#include "MapEditor.h"
#include "DrawUtil.h"


MapEditorAnchor::MapEditorAnchor(NotNullPtr<MapEditor> editor, MapDef & map, int layer_index) :
  m_Editor(editor),
  m_Map(map),
  m_LayerIndex(layer_index)
{

}

Optional<Vector2> MapEditorAnchor::GetPreviewAnchor()
{
  auto anchor = m_Map.m_Anchors.TryGet(m_LayerIndex);
  if (anchor == nullptr)
  {
    return{};
  }

  Vector2 point = anchor->GetPoint();
  if (m_Offset)
  {
    point += m_Offset.Value();
  }

  return point;
}

void MapEditorAnchor::Draw(GeometryVertexBufferBuilder & buffer, const Box & viewport_bounds, RenderVec2 & screen_center, float magnification)
{

}

void MapEditorAnchor::DrawControls(GeometryVertexBufferBuilder & buffer, const Box & viewport_bounds, RenderVec2 & screen_center, float magnification)
{
  auto point = GetPreviewAnchor();
  if (point == false)
  {
    return;
  }

  if (m_Selected || m_Highlighted)
  {
    DrawUtil::DrawHighlightedCornerControl(buffer, point.Value(), magnification);
  }
  else
  {
    DrawUtil::DrawCornerControl(buffer, point.Value(), magnification);
  }
}

void MapEditorAnchor::SetSelected()
{
  m_Selected = true;
}

void MapEditorAnchor::ClearSelected()
{
  m_Selected = false;
}

bool MapEditorAnchor::IsSelected()
{
  return m_Selected;
}

void MapEditorAnchor::SetPreviewOffset(Vector2 offset)
{
  m_Offset = offset;
}

void MapEditorAnchor::Select(const Box & selection_box)
{
  auto preview_anchor = GetPreviewAnchor();
  if (preview_anchor && PointInBox(selection_box, preview_anchor.Value()))
  {
    SetSelected();
  }
  else
  {
    ClearSelected();
  }
}

void MapEditorAnchor::SetHighlighted()
{
  m_Highlighted = true;
}

void MapEditorAnchor::ClearHighlighted()
{
  m_Highlighted = false;
}

bool MapEditorAnchor::IsHighlighted()
{
  return m_Highlighted;
}

void MapEditorAnchor::CommitPreview()
{
  if (m_Offset == false)
  {
    return;
  }

  if (m_Offset->x == 0 && m_Offset->y == 0)
  {
    return;
  }

  auto preview_anchor = GetPreviewAnchor();
  if (preview_anchor == false)
  {
    return;
  }

  ClearPreview();
  m_Offset = {};

  WriteData(preview_anchor.Value());
}

void MapEditorAnchor::ClearPreview()
{
  m_Offset.Clear();
}

void MapEditorAnchor::WriteData(const Vector2 & pos)
{
  auto anchor = m_Map.m_Anchors.TryGet(m_LayerIndex);
  if (anchor != nullptr)
  {
    m_Editor->BeginTransaction();
    anchor->m_X = pos.x;
    anchor->m_Y = pos.y;
    m_Editor->CommitChanges();
  }
}

void MapEditorAnchor::ToggleHidden()
{
  m_Hidden = !m_Hidden;
}

void MapEditorAnchor::SetHidden(bool hidden)
{
  m_Hidden = hidden;
}

bool MapEditorAnchor::IsHidden()
{
  return m_Hidden;
}

