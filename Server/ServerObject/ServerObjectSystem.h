#pragma once

#include "Foundation/CallList/CallList.h"

#include "Server/ServerCommon.h"

class ServerObject;
class ServerObjectManager;
class ServerObjectInitData;
class ServerObjectUpdateList;
class ServerObjectNetBitWriter;
class ServerObjectNetBitReader;

struct ServerObjectTypeInfo
{
  uint32_t m_TypeNameHash;
  uint32_t m_TypeIndex;
  uint32_t m_BaseClassTypeNameHash;
  uint32_t m_InitDataTypeNameHash;
  NotNullPtr<const uint32_t *> m_BaseListClassesPtr;
  NotNullPtr<uint32_t> m_NumBaseClassesPtr;
  NotNullPtr<std::size_t> m_TypeIndexPtr;

  NotNullPtr<ServerObject>(*m_ObjectCreate)();
  NotNullPtr<ServerObject>(*m_ObjectDuplicate)(NotNullPtr<const ServerObject> rhs);

  void(*m_ObjectInit)(NotNullPtr<ServerObject> object, NullOptPtr<ServerObjectInitData> init_data);
  void(*m_ObjectCopy)(NotNullPtr<ServerObject> object, NotNullPtr<const ServerObject> rhs);
  void(*m_ObjectResetHandles)(NotNullPtr<ServerObject> object, const ServerObjectManager & obj_manager);
  void(*m_ObjectDestroy)(NotNullPtr<ServerObject> object);
  void(*m_AddToUpdateList)(NotNullPtr<ServerObject> object, ServerObjectUpdateList & l);

  void(*m_ObjectSerialize)(NotNullPtr<ServerObject> object, ServerObjectNetBitWriter & writer);
  void(*m_ObjectDeserialize)(NotNullPtr<ServerObject> object, ServerObjectNetBitReader & reader);
};

class ServerObjectSystem
{
public:

  void RegisterType(const ServerObjectTypeInfo & type_info);
  void FinalizeTypes();
private:

  friend class ServerObjectManager;

  NotNullPtr<ServerObject> AllocateObject(std::size_t type_index, NullOptPtr<ServerObjectInitData> init_data);
  NotNullPtr<ServerObject> DuplicateObject(NotNullPtr<const ServerObject> rhs);
  void CopyObject(NotNullPtr<ServerObject> object, NotNullPtr<const ServerObject> rhs);
  void FreeObject(NotNullPtr<ServerObject> object);
  void ResetObjectHandles(NotNullPtr<ServerObject> object, const ServerObjectManager & obj_manager);
  void FinalizeType(ServerObjectTypeInfo & type);

private:


  std::vector<ServerObjectTypeInfo> m_ObjectTypes;

  std::vector<std::pair<uint32_t, uint32_t>> m_ObjectDependencies;
  std::vector<int> m_ObjectUpdateIndex;
};

extern ServerObjectSystem g_ServerObjectSystem;
extern PreMainCallList g_ServerObjectRegisterCallList;