#pragma once

#include "Foundation/Handle/Handle.h"
#include "Foundation/SkipField/SkipFieldIterator.h"

#include "Server/ServerObject/ServerObjectHandle.h"
#include "Server/ServerObject/ServerObjectRegistrationMacros.h"

#include "StormRefl/StormReflMetaInfoBase.h"

class ServerObjectManager;
class ServerComponent;
class ServerGameState;

class ServerObject
{
public:
  DECLARE_BASE_SERVER_OBJECT;
  STORM_REFL;

  ServerObjectHandle GetObjectHandle() const;
  void Destroy(ServerObjectManager & obj_manager);

  virtual czstr GetDefaultEntityBinding();
  virtual czstr GetEntityBinding();

  bool IsDestroyed() const;
  int GetSlotIndex() const;

  template <typename Type>
  NullOptPtr<Type> CastTo()
  {
    if (CastToInternal((uint32_t)Type::TypeIndex))
    {
      return static_cast<Type *>(this);
    }

    return nullptr;
  }

private:

  template <typename Type>
  friend class SkipField;

  friend class ServerObjectSystem;
  friend class ServerObjectManager;

  const Handle & GetHandle() const;
  void SetHandle(Handle & handle);

  void SetIterator(const SkipFieldIterator & itr);
  const SkipFieldIterator & GetIterator() const;

private:
  bool m_IsStatic = false;
  int m_TypeIndex = 0;
  int m_SlotIndex = 0;

  SkipFieldIterator m_Iterator;
  Handle m_Handle;
  ServerObjectHandle m_ServerObjectHandle;
};

template <typename T>
NullOptPtr<T> ServerObjectHandle::ResolveTo(const ServerObjectManager & object_manager)
{
  static_assert(std::is_base_of<ServerObject, T>::value, "Must resolve to server object type");

  auto ptr = Resolve(object_manager);
  if (ptr == nullptr)
  {
    return nullptr;
  }

  if (ptr->CastToInternal((uint32_t)T::TypeIndex))
  {
    return static_cast<T *>(ptr);
  }

  return nullptr;
}

template <>
NullOptPtr<ServerObject> ServerObjectHandle::ResolveTo<ServerObject>(const ServerObjectManager & object_manager);

template <>
struct StormReflTypeInfo<ServerObject>
{
  using MyBase = void;
  static constexpr int fields_n = 0;
  template <int N> struct field_data_static {};
  template <int N, typename Self> struct field_data {};
  template <int N> struct annotations { static constexpr int annotations_n = 0; template <int A> struct annoation { }; };
  static constexpr auto GetName() { return "ServerObject"; }
  static constexpr auto GetNameHash() { return COMPILE_TIME_CRC32_STR("ServerObject"); }
  static ServerObject & GetDefault() { static ServerObject def; return def; }
};
