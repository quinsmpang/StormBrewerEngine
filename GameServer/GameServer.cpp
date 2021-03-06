
#include "Foundation/Common.h"

#include "GameServer.h"

#include "Game/GameFullState.refl.meta.h"
#include "Game/GameMessages.refl.meta.h"
#include "Game/GameProtocol.h"

#include "StormNet/NetProtocolFuncs.h"


GameServer::GameServer(int max_clients, int port, GameStageManager & stage_manager, GameSharedGlobalResources & shared_global_resource) :
  ServerBase(max_clients, &m_Backend),
  m_StageManager(stage_manager),
  m_SharedGlobalResources(shared_global_resource),
#ifdef NET_USE_WEBRTC
  m_Backend(this, NetServerBackendWebrtcSettings{ (uint16_t)port, (uint16_t)max_clients, "localhost.key", "localhost.crt", NetGetProtocolPipeModes(ClientProtocolDef{}), NetGetProtocolPipeModes(ServerProtocolDef{}) }),
#else
  m_Backend(this, NetServerSettingsEnet(port, 0, max_clients)),
#endif
  m_GameInstanceManager(*this, stage_manager, shared_global_resource)
{

}

GameServer::~GameServer()
{

}

void GameServer::Update()
{
  ServerBase::Update();
}

GameInstanceManager & GameServer::GetGameInstanceManager()
{
  return m_GameInstanceManager;
}

GameClientConnection * GameServer::ConstructClient(void * client_mem, uint32_t connection_id, ProtocolType & proto)
{
  auto client = new (client_mem) GameClientConnection(*this, connection_id, proto);
  return client;
}

void GameServer::CleanupClient(GameClientConnection & client, ProtocolType & proto)
{
  client.RemoveFromGame();
}
