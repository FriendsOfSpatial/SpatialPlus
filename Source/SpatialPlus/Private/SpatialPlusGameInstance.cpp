#include "SpatialPlusGameInstance.h"

#include "SpatialOS.h"
#include "EntityPipeline.h"
#include "EntityRegistry.h"
#include "SpatialOSComponentUpdater.h"
#include "SimpleEntitySpawnerBlock.h"

USpatialPlusGameInstance::USpatialPlusGameInstance()
{
	SpatialOSComponentUpdater = CreateDefaultSubobject<USpatialOSComponentUpdater>(TEXT("ComponentUpdater"));
}

void USpatialPlusGameInstance::Init()
{
	UGameInstance::Init();

	SpatialOS = NewObject<USpatialOS>(this);
	SpatialOS->OnConnectedDelegate.AddDynamic(this, &USpatialPlusGameInstance::OnSpatialOSConnected);
	SpatialOS->OnDisconnectedDelegate.AddDynamic(this, &USpatialPlusGameInstance::OnSpatialOSDisconnected);

	EntityRegistry = NewObject<UEntityRegistry>(this);
}

void USpatialPlusGameInstance::Shutdown()
{
	UGameInstance::Shutdown();

	const auto SpatialOS = GetSpatialOS();
	SpatialOS->OnConnectedDelegate.RemoveAll(this);
	SpatialOS->OnDisconnectedDelegate.RemoveAll(this);
}

void USpatialPlusGameInstance::ProcessOperations(float DeltaTime)
{
	const auto SpatialOS = GetSpatialOS();
	if (SpatialOS == nullptr || SpatialOS->GetEntityPipeline() == nullptr)
		return;

	SpatialOS->ProcessOps();
	SpatialOS->GetEntityPipeline()->ProcessOps(SpatialOS->GetView(), SpatialOS->GetConnection(), GetWorld());
	SpatialOSComponentUpdater->UpdateComponents(EntityRegistry, DeltaTime);
}

USpatialOS* USpatialPlusGameInstance::GetSpatialOS() const
{
	checkf(SpatialOS, TEXT("SpatialOS is null!"));

	return SpatialOS;
}

UEntityRegistry* USpatialPlusGameInstance::GetEntityRegistry() const
{
	checkf(EntityRegistry, TEXT("EntityRegistry is null!"));

	return EntityRegistry;
}

void USpatialPlusGameInstance::OnSpatialOSConnected()
{
	SpatialOS->GetEntityPipeline()->AddBlock(GetSpawnerBlock(GetEntityRegistry()));

	TArray<FString> EntityBlueprintPaths;
	EntityBlueprintPaths.Add(TEXT("/Game/EntityBlueprints"));
	EntityRegistry->RegisterEntityBlueprints(EntityBlueprintPaths);

	GetWorld()->GetTimerManager().SetTimer(MetricsReporterHandle, this, &USpatialPlusGameInstance::SendMetrics, 2.0f, true, 2.0f);
}

void USpatialPlusGameInstance::OnSpatialOSDisconnected()
{
	GetWorld()->GetTimerManager().ClearTimer(MetricsReporterHandle);
}

UEntityPipelineBlock* USpatialPlusGameInstance::GetSpawnerBlock(UEntityRegistry* EntityRegistry)
{
	auto Result = NewObject<USimpleEntitySpawnerBlock>();
	Result->Init(EntityRegistry);
	return Result;
}

void USpatialPlusGameInstance::SendMetrics() const
{
	const auto SpatialOS = GetSpatialOS();
	auto Connection = SpatialOS->GetConnection().Pin();
	if (Connection.IsValid())
		Connection->SendMetrics(SpatialOS->GetMetrics());
}