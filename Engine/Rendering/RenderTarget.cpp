
#include "Engine/EngineCommon.h"
#include "Engine/Rendering/RenderTarget.h"
#include "Engine/Rendering/RenderErrorMacros.h"

#include <gl3w/gl3w.h>

RenderTarget::RenderTarget()
{
  m_DepthBufferName = 0;
  m_FrameBufferName = 0;
  m_TextureName = 0;
  m_LoadError = 0;
  m_Width = 0;
  m_Height = 0;
}

RenderTarget::RenderTarget(RenderTarget && rhs) noexcept
{
  Move(std::move(rhs));
}

RenderTarget::~RenderTarget()
{
  Destroy();
}

RenderTarget & RenderTarget::operator = (RenderTarget && rhs) noexcept
{
  Destroy();
  Move(std::move(rhs));
  return *this;
}

void RenderTarget::Move(RenderTarget && rhs) noexcept
{
  m_DepthBufferName = rhs.m_DepthBufferName;
  m_FrameBufferName = rhs.m_FrameBufferName;
  m_TextureName = rhs.m_TextureName;
  m_LoadError = rhs.m_LoadError;
  m_Width = rhs.m_Width;
  m_Height = rhs.m_Height;

  rhs.m_DepthBufferName = 0;
  rhs.m_FrameBufferName = 0;
  rhs.m_TextureName = 0;
  rhs.m_LoadError = 0;
  rhs.m_Width = 0;
  rhs.m_Height = 0;
}

void RenderTarget::Destroy()
{
  if (m_FrameBufferName != 0)
  {
    glDeleteFramebuffers(1, &m_FrameBufferName);
    m_FrameBufferName = 0;
  }

  if (m_TextureName != 0)
  {
    glDeleteTextures(1, &m_TextureName);
    m_TextureName = 0;
  }  
  
  if (m_DepthBufferName != 0)
  {
    glDeleteRenderbuffers(1, &m_DepthBufferName);
    m_DepthBufferName = 0;
  }

  m_LoadError = 0;
  m_Width = 0;
  m_Height = 0;
}


void RenderTarget::CreateRenderTarget(int width, int height, int depth_size, int stencil_size)
{
  Destroy();

  glGenFramebuffers(1, &m_FrameBufferName); CHECK_GL_LOAD_ERROR;
  auto fb_destroy_on_error = gsl::finally([&] { if (m_LoadError != 0) { glDeleteFramebuffers(1, &m_FrameBufferName); m_FrameBufferName = 0; } });

  glGenTextures(1, &m_TextureName); CHECK_GL_LOAD_ERROR;
  auto texture_destroy_on_error = gsl::finally([&] { if (m_LoadError != 0) { glDeleteTextures(1, &m_TextureName); m_TextureName = 0; } });

  glBindTexture(GL_TEXTURE_2D, m_TextureName); CHECK_GL_LOAD_ERROR;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); CHECK_GL_LOAD_ERROR;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureName, 0);

  if (depth_size > 0 || stencil_size > 0)
  {
    GLenum depth_mode;
    switch (depth_size)
    {
    case 0:
      if (stencil_size != 8)
      {
        m_LoadError = GL_INVALID_VALUE;
        return;
      }

      depth_mode = GL_STENCIL_INDEX8;
      break;
    case 16:
      if (stencil_size != 0)
      {
        m_LoadError = GL_INVALID_VALUE;
        return;
      }

      depth_mode = GL_DEPTH_COMPONENT16;
      break;
    case 24:
      switch (stencil_size)
      {
      case 0:
        depth_mode = GL_DEPTH_COMPONENT24;
        break;
      case 8:
        depth_mode = GL_DEPTH24_STENCIL8;
        break;
      default:
        m_LoadError = GL_INVALID_VALUE;
        return;
      }
      break;
    default:
      m_LoadError = GL_INVALID_VALUE;
      return;
    }

    glGenRenderbuffers(1, &m_DepthBufferName); CHECK_GL_LOAD_ERROR;
    auto db_destroy_on_error = gsl::finally([&] { if (m_LoadError != 0) { glDeleteRenderbuffers(1, &m_DepthBufferName); m_DepthBufferName = 0; } });

    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferName); CHECK_GL_LOAD_ERROR;
    glRenderbufferStorage(GL_RENDERBUFFER, depth_mode, width, height); CHECK_GL_LOAD_ERROR;

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferName);
  }

  m_Width = width;
  m_Height = height;
}
