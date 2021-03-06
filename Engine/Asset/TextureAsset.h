#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetTypeMacros.h"
#include "Engine/Rendering/Texture.h"

class ENGINE_EXPORT TextureAsset : public Asset
{
public:

  const Texture & GetTexture() const;

  int GetWidth() const;
  int GetHeight() const;

  Vector2 GetSize() const;

  NullOptPtr<const PixelBuffer> GetPixelBuffer() const;

  bool IsValid() const;

protected:

  virtual int PreProcessLoadedData(Buffer & buffer, bool load_deps) override;
  virtual void OnDataLoadComplete(Buffer & buffer, bool load_deps) override;


protected:
  Optional<PixelBuffer> m_PixelBuffer;
  Texture m_Texture;

public:
  ASSET_HEADER_FUNCS(TextureAsset);
};
