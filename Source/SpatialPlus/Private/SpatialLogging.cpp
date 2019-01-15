// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "SpatialLogging.h"

#include "CoreMinimal.h"
#include "Engine/NetDriver.h"
#include "EntityRegistry.h"
#include "GameFramework/Actor.h"
#include "SpatialNetDriver.h"
#include "Connection/SpatialWorkerConnection.h"

FString FSpatialLogging::LogPrefix(AActor* Actor)
{
	FString WorkerId("Unknown");
	int32 EntityId = -1;
	if (USpatialNetDriver* SpatialNetDriver = Cast<USpatialNetDriver>(Actor->GetNetDriver()))
	{
		WorkerId = SpatialNetDriver->Connection->GetWorkerId();
		EntityId = SpatialNetDriver->GetEntityRegistry()->GetEntityIdFromActor(Actor);
	}
	else
	{
		// No SpatialOS net driver, so just return the actor's name.
		return Actor->GetName();
	}

	if (EntityId == 0)
	{
		return FString::Printf(TEXT("%s %s (non-spatial actor)"), *WorkerId, *Actor->GetName());
	}

	return FString::Printf(TEXT("%s %s (%d)"), *WorkerId, *Actor->GetName(), EntityId);
}