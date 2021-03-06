#pragma once

#include "Foundation/Common.h"

#include "Game/GameNetworkSettings.h"

class GameSimulationEventCallbacks
{
public:
  virtual void HandleWinGame(int winning_team) {};

#if NET_MODE == NET_MODE_TURN_BASED_DETERMINISTIC
  virtual void HandleTimeExpired() {};
#endif
};

