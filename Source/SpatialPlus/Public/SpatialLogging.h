// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// NOTE: This is from the SpatialGDK ThirdPersonShooter example

#pragma once

#include "CoreMinimal.h"

class FSpatialLogging 
{
public:
	// Helper method that returns a SpatialOS-specific prefix for log messages, including:
	//   Actor Name
	//   Worker Name
	//   Actor's Corresponding Entity Id
	// Will also work for non-spatial actors, and will report that they don't have an entity id.
	static FString LogPrefix(class AActor* Actor);
};