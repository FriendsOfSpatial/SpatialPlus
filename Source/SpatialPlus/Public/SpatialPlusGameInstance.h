#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SpatialPlusGameInstance.generated.h"

class USpatialOS;
class UEntityRegistry;
class USpatialOSComponentUpdater;
class UEntityPipelineBlock;

UCLASS(BlueprintType)
class SPATIALPLUS_API USpatialPlusGameInstance 
	: public UGameInstance
{
	GENERATED_BODY()
	
public:
	USpatialPlusGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void ProcessOperations(float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USpatialOS* GetSpatialOS() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEntityRegistry* GetEntityRegistry() const;

protected:
	FTimerHandle MetricsReporterHandle;

	UPROPERTY(Transient)
	USpatialOS* SpatialOS;

	UPROPERTY(Transient)
	UEntityRegistry* EntityRegistry;

	UPROPERTY(Transient)
	USpatialOSComponentUpdater* SpatialOSComponentUpdater;

	UFUNCTION()
	virtual void OnSpatialOSConnected();

	UFUNCTION()
	virtual void OnSpatialOSDisconnected();

	/* Returns an instance of a user-specified spawner block. Default returns USimpleEntitySpawnerBlock */
	virtual UEntityPipelineBlock* GetSpawnerBlock(UEntityRegistry* EntityRegistry);

	void SendMetrics() const;
};