
#include "Foundation/Common.h"

#include "Allocator2D.h"

Allocator2D::Allocator2D(int width, int height) :
  m_BinPacker(width, height)
{

}

Optional<Allocator2DResult> Allocator2D::Allocate(int width, int height)
{
  auto result = m_BinPacker.Insert(width, height, false, rbp::GuillotineBinPack::RectBestAreaFit, rbp::GuillotineBinPack::SplitLongerAxis);
  if (result.height == 0)
  {
    return{};
  }

  return Allocator2DResult{ result.x, result.y, result.width == height };
}

