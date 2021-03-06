
#include "Game/GameLogicContainer.h"

GameLogicContainer::GameLogicContainer(
  GameController & game_controller,
  GameInstanceData & global_data,
  ServerObjectManager & object_manager,
  GameServerEventSender & server_event_sender,
  GameSimulationEventCallbacks & sim_event_sender,
  GameSharedGlobalResources & shared_global_resources,
  GameSharedInstanceResources & shared_instance_resources,
  const GameStage & stage,
  bool is_authority,
  int & send_timer) :
  m_GameController(game_controller),
  m_GlobalData(global_data),
  m_ObjectManager(object_manager),
  m_ServerEventSender(server_event_sender),
  m_SimEventSender(sim_event_sender),
  m_SharedGlobalResources(shared_global_resources),
  m_SharedInstanceResources(shared_instance_resources),
  m_Stage(stage),
  m_IsAuthority(is_authority),
  m_SendTimer(send_timer)
{

}

GameController & GameLogicContainer::GetGameController()
{
  return m_GameController;
}

GameInstanceData & GameLogicContainer::GetGlobalData()
{
  return m_GlobalData;
}

ServerObjectManager & GameLogicContainer::GetObjectManager()
{
  return m_ObjectManager;
}

GameServerEventSender & GameLogicContainer::GetEventSender()
{
  return m_ServerEventSender;
}

GameSimulationEventCallbacks & GameLogicContainer::GetSimEventCallbacks()
{
  return m_SimEventSender;
}

GameSharedGlobalResources & GameLogicContainer::GetSharedGlobalResources()
{
  return m_SharedGlobalResources;
}

GameSharedInstanceResources & GameLogicContainer::GetSharedInstanceResources()
{
  return m_SharedInstanceResources;
}

const GameStage & GameLogicContainer::GetStage()
{
  return m_Stage;
}

bool GameLogicContainer::IsAuthority()
{
  return m_IsAuthority;
}

void GameLogicContainer::TriggerImmediateSend()
{
  m_SendTimer = 0;
}
