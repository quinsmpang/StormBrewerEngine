#pragma once

#include <glm/glm.hpp>

static const float kPi = 3.14159265359f;
static const float kHalfPi = 1.57079632679f;
static const float k2Pi = 6.28318530718f;
static const float kSqrt2 = 1.41421356237f;

using zstr = gsl::zstring<>;
using czstr = gsl::czstring<>;
using zwstr = gsl::wzstring<>;
using czwstr = gsl::cwzstring<>;

using RenderVec2 = glm::vec2;
using RenderVec3 = glm::vec3;
using RenderVec4 = glm::vec4;
using RenderMat3 = glm::mat3x4;
using RenderMat4 = glm::mat4x4;

using Vector2f = glm::vec2;
using Vector3f = glm::vec3;
using Vector4f = glm::vec4;
using Mat3f = glm::mat3x4;
using Mat4f = glm::mat4x4;

static inline Vector2f SinCosf(float angle)
{
  return Vector2f{ cos(angle), sin(angle) };
}

static inline float WrapAngle(float angle)
{
  float n_angle = angle / k2Pi;
  return (n_angle - floorf(n_angle)) * k2Pi;
}

static inline float DiffAngle(float a, float b)
{
  auto wa = WrapAngle(a);
  auto wb = WrapAngle(b);

  if (wa > wb)
  {
    float d1 = wa - wb;
    float d2 = wb + k2Pi - wa;
    return d1 < d2 ? -d1 : d2;
  }
  else
  {
    float d1 = wb - wa;
    float d2 = wa + k2Pi - wb;
    return d1 < d2 ? d1 : -d2;
  }
}

static inline float OffsetAngle(float a, float b)
{
  auto wa = WrapAngle(a);
  auto wb = WrapAngle(b);

  if (wa > wb)
  {
    return std::min(wa - wb, wb + k2Pi - wa);
  }
  else
  {
    return std::min(wb - wa, wa + k2Pi - wb);
  }
}
