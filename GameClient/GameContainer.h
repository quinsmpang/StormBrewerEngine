#pragma once

#include "Foundation/Common.h"

#include "Engine/Window/Window.h"
#include "Engine/Rendering/RenderState.h"
#include "Engine/Rendering/RenderUtil.h"
#include "Engine/EngineState.h"

#include "Game/GameStageManager.h"
#include "Game/GameSharedGlobalResources.h"

#include "GameClient/GameMode.h"
#include "GameClient/GameClientGlobalResources.h"
#include "GameClient/GameClientInstanceData.h"
#include "GameClient/GameClientSystems.h"

class GameMode;
class GameNetworkClient;

template <typename GameMode>
struct GameModeDef {};

struct GameContainerInitSettings
{
  bool m_AutoConnect = false;
  bool m_AutoBotGame = false;
  std::string m_UserName;
};

class GameContainer
{
public:
  GameContainer(const Window & window, std::unique_ptr<GameContainerInitSettings> && init_settings = nullptr);
  ~GameContainer();

  void SetInitialMode();

  EngineState & GetEngineState();
  Window & GetWindow();

  NullOptPtr<GameContainerInitSettings> GetInitSettings();
  void ReleaseInitSettings();

  GameLevelList & GetLevelList();
  GameSharedGlobalResources & GetSharedGlobalResources();
  GameClientGlobalResources & GetClientGlobalResources();

  NullOptPtr<GameClientInstanceData> GetInstanceData();
  void SetInstanceData(NullOptPtr<GameClientInstanceData> instance_data);

  NullOptPtr<GameClientSystems> GetClientSystems();
  void SetClientSystems(NullOptPtr<GameClientSystems> client_systems);

  RenderState & GetRenderState();
  RenderUtil & GetRenderUtil();
  
  void StartNetworkClient();
  void StopNetworkClient();
  GameNetworkClient & GetClient();
  GameNetworkClientInitSettings & GetNetworkInitSettings();

  bool AllGlobalResourcesLoaded();

  void Update();
  void Render();

  template <typename Mode, typename ... Args>
  void SwitchMode(const GameModeDef<Mode> & def, Args && ... args)
  {
    m_Mode = std::make_unique<Mode>(*this, std::forward<Args>(args)...);
    m_Mode->Initialize();
  }

private:
  EngineState m_EngineState;

  Window m_Window;
  std::unique_ptr<GameMode> m_Mode;
  std::unique_ptr<GameNetworkClient> m_Client;
  std::unique_ptr<GameContainerInitSettings> m_InitSettings;

  GameNetworkClientInitSettings m_NetInitSettings;

  GameSharedGlobalResources m_SharedGlobalResources;
  GameClientGlobalResources m_ClientGlobalResources;

  NullOptPtr<GameClientInstanceData> m_ClientInstanceData;
  NullOptPtr<GameClientSystems> m_ClientSystems;

  GameLevelList m_LevelList;

  RenderState m_RenderState;
  RenderUtil m_RenderUtil;
};


