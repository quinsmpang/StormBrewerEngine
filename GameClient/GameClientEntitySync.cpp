
#include "Engine/Entity/Entity.h"
#include "Engine/EngineState.h"

#include "Runtime/Entity/EntityResource.h"

#include "GameClient/GameClientEntitySync.h"
#include "GameClient/GameContainer.h"


GameClientEntitySync::GameClientEntitySync(GameContainer & game) :
  m_GameContainer(game)
{

}

GameClientEntitySync::~GameClientEntitySync()
{
  DestroyAll();
}

void GameClientEntitySync::Sync(ServerObjectManager & obj_manager)
{
  auto & engine_state = m_GameContainer.GetEngineState();

  std::size_t last_index = 0;
  auto visitor = [&] (std::size_t index, NotNullPtr<ServerObject> obj)
  {
    EntityHandle cur_handle;
    if (m_Entities.HasAt(index))
    {
      cur_handle = m_Entities[index];
    }

    while (index != last_index)
    {
      if (m_Entities.HasAt(last_index))
      {
        m_Entities[last_index].Destroy();
      }

      last_index++;
    }

    last_index++;

    auto entity_asset = obj->GetEntityBinding();
    if (entity_asset)
    {
      auto entity = cur_handle.Resolve();
      if (entity)
      {
        auto asset_hash = crc64(entity_asset);
        if (asset_hash == entity->GetAssetNameHash())
        {
          return;
        }

        cur_handle.Destroy();
      }

      auto entity_resource = EntityResource::Load(entity_asset);
      if (entity_resource.GetResource()->IsLoaded() == false)
      {
        ASSERT(false, "Sync object bound asset must be preloaded");
        return;
      }

      auto new_entity = engine_state.CreateEntity(entity_resource.GetResource(), obj);
      m_Entities.InsertAt(index, new_entity->GetHandle());
    }
    else
    {
      cur_handle.Destroy();
    }
  };

  obj_manager.VisitObjects(visitor);

  while ((int)last_index <= m_Entities.HighestIndex())
  {
    if (m_Entities.HasAt(last_index))
    {
      m_Entities[last_index].Destroy();
    }

    last_index++;
  }
}

void GameClientEntitySync::DestroyAll()
{
  for (auto ent : m_Entities)
  {
    ent.second.Destroy();
  }

  m_Entities.Clear();
}

void GameClientEntitySync::SendEntityEvent(int entity_index, uint32_t type_name_hash, void * ev)
{
  auto entity = m_Entities[entity_index].Resolve();

  if (entity)
  {
    entity->TriggerEventHandler(type_name_hash, ev, nullptr);
  }
}

