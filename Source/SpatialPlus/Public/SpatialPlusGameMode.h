#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EntityId.h"

#include "SpatialPlusGameMode.generated.h"

class USpatialPlusGameInstance;
class USpatialOS;
class UCommander;

UCLASS(BlueprintType)
class SPATIALPLUS_API ASpatialPlusGameMode 
	: public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
	FString WorkerTypeOverride;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
	FString WorkerIdOverride;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
	bool bUseExternalIp;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
	float ProcessOperationsInterval;

	ASpatialPlusGameMode();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSpatialOSConnected();
	virtual void OnSpatialOSConnected_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSpatialOSDisconnected();
	virtual void OnSpatialOSDisconnnected_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSpatialOSFailedToConnect();
	virtual void OnSpatialOSFailedToConnect_Implementation();

	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnGetSpawnerEntityId, bool, bSuccess, FString, ErrorMessage, FEntityId, SpawnerEntityId);
	UFUNCTION(BlueprintCallable)
	virtual void GetSpawnerEntityId(const FOnGetSpawnerEntityId& Callback, int32 TimeoutMs = 100);

	void StartPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetSpatialOsWorkerType();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsConnectedToSpatialOS();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCommander* SendWorkerCommand();

protected:
	UPROPERTY(Transient)
	USpatialPlusGameInstance* GameInstance;
	inline USpatialPlusGameInstance* GetGameInstance();

	FTimerHandle ProcessOperationsHandle;
	void ProcessOperations();

	DECLARE_DELEGATE(FOnUnbind)

	USpatialOS* GetSpatialOS();

	UPROPERTY()
	UCommander* Commander;

	FOnGetSpawnerEntityId* OnGetSpawnerEntityIdCallback;

	FOnUnbind OnUnbind;
	void UnbindEntityQueryCallback();
	std::uint64_t EntityQueryCallback;
};