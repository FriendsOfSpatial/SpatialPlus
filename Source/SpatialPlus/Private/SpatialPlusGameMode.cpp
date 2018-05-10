#include "SpatialPlusGameMode.h"
#include "SpatialPlusGameInstance.h"

#include "SpatialOS.h"
#include "EntityBuilder.h"
#include "SpatialOSWorkerConfigurationData.h"
#include "WorkerConnection.h"
#include "improbable/standard_library.h"
#include "Commander.h"
#include "EntityRegistry.h"

using namespace improbable;
using namespace improbable::unreal::core;

ASpatialPlusGameMode::ASpatialPlusGameMode()
	: EntityQueryCallback(-1),
	WorkerTypeOverride(TEXT("")),
	WorkerIdOverride(TEXT("")),
	bUseExternalIp(false),
	ProcessOperationsInterval(1.0f / 60.0f)
{
	bStartPlayersAsSpectators = true;
	DefaultPawnClass = nullptr;

	OnUnbind.BindUObject(this, &ASpatialPlusGameMode::UnbindEntityQueryCallback);
}

void ASpatialPlusGameMode::OnSpatialOSConnected_Implementation() { }

void ASpatialPlusGameMode::OnSpatialOSDisconnnected_Implementation() { }

void ASpatialPlusGameMode::OnSpatialOSFailedToConnect_Implementation() { }

void ASpatialPlusGameMode::GetSpawnerEntityId(const FOnGetSpawnerEntityId& Callback, int32 TimeoutMs)
{
	const auto SpatialOS = GetSpatialOS();

	auto LockedConnection = SpatialOS->GetConnection().Pin();
	if (!LockedConnection.IsValid())
		return;

	auto LockedDispatcher = SpatialOS->GetView().Pin();
	if (!LockedDispatcher.IsValid())
		return;

	// TODO: Something has changed
	//OnGetSpawnerEntityIdCallback = new FOnGetSpawnerEntityId(Callback);
	//const worker::query::EntityQuery& EntityQuery = {
	//	worker::query::SnapshotResultType{}
	//};

	//auto RequestId = LockedConnection->SendEntityQueryRequest(EntityQuery, StaticCast<std::uint32_t>(TimeoutMs));
	//EntityQueryCallback = LockedDispatcher->OnEntityQueryResponse([this, RequestId](const worker::EntityQueryResponseOp& Op)
	//{
	//	if (Op.RequestId != RequestId)
	//		return;

	//	if(!OnGetSpawnerEntityIdCallback->IsBound())
	//	{
	//		// TODO: Log
	//	}

	//	if(Op.StatusCode != worker::StatusCode::kSuccess)
	//	{
	//		OnGetSpawnerEntityIdCallback->ExecuteIfBound(false, FString::Printf(TEXT("Could not find spawner entity: %s"), Op.Message.c_str()), FEntityId());
	//		return;
	//	}

	//	if(Op.ResultCount == 0)
	//	{
	//		OnGetSpawnerEntityIdCallback->ExecuteIfBound(false, TEXT("Query returned 0 spawner entities"), FEntityId());
	//		return;
	//	}

	//	OnGetSpawnerEntityIdCallback->ExecuteIfBound(true, TEXT(""), FEntityId(Op.Result.begin()->first));
	//	GetWorldTimerManager().SetTimerForNextTick(UnbindEntityQueryCallback);
	//});
}

void ASpatialPlusGameMode::StartPlay()
{
	AGameModeBase::StartPlay();

	const auto SpatialOS = GetSpatialOS();
	SpatialOS->OnConnectedDelegate.AddDynamic(this, &ASpatialPlusGameMode::OnSpatialOSConnected);
	SpatialOS->OnConnectionFailedDelegate.AddDynamic(this, &ASpatialPlusGameMode::OnSpatialOSFailedToConnect);
	SpatialOS->OnDisconnectedDelegate.AddDynamic(this, &ASpatialPlusGameMode::OnSpatialOSDisconnected);

	auto WorkerConfiguration = FSOSWorkerConfigurationData();
	WorkerConfiguration.Networking.UseExternalIp = bUseExternalIp;

	if (!WorkerTypeOverride.IsEmpty())
		WorkerConfiguration.SpatialOSApplication.WorkerPlatform = WorkerTypeOverride;

	if (!WorkerIdOverride.IsEmpty())
		WorkerConfiguration.SpatialOSApplication.WorkerId = WorkerIdOverride;

	SpatialOS->ApplyConfiguration(WorkerConfiguration);
	SpatialOS->Connect();

	GetWorldTimerManager().SetTimer(ProcessOperationsHandle, this, &ASpatialPlusGameMode::ProcessOperations, ProcessOperationsInterval, true);
}

void ASpatialPlusGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AGameModeBase::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(ProcessOperationsHandle);

	UnbindEntityQueryCallback();

	const auto SpatialOS = GetSpatialOS();
	SpatialOS->Disconnect();

	SpatialOS->OnConnectedDelegate.RemoveAll(this);
	SpatialOS->OnConnectionFailedDelegate.RemoveAll(this);
	SpatialOS->OnDisconnectedDelegate.RemoveAll(this);
}

FString ASpatialPlusGameMode::GetSpatialOsWorkerType()
{
	const auto SpatialOS = GetSpatialOS();
	
	return SpatialOS->GetWorkerConfiguration().GetWorkerType();
}

bool ASpatialPlusGameMode::IsConnectedToSpatialOS()
{
	const auto SpatialOS = GetSpatialOS();
	return SpatialOS->IsConnected();
}

UCommander* ASpatialPlusGameMode::SendWorkerCommand()
{
	if(Commander == nullptr)
	{
		const auto SpatialOS = GetSpatialOS();
		Commander = NewObject<UCommander>(this, UCommander::StaticClass())->Init(nullptr, SpatialOS->GetConnection(), SpatialOS->GetView());
	}

	return Commander;
}

USpatialPlusGameInstance* ASpatialPlusGameMode::GetGameInstance()
{
	if (GameInstance == nullptr)
		GameInstance = Cast<USpatialPlusGameInstance>(GetWorld()->GetGameInstance());

	checkf(GameInstance, TEXT("Your GameInstance must be or derive from USpatialPlusGameInstance"));

	return GameInstance;
}

void ASpatialPlusGameMode::ProcessOperations()
{
	GetGameInstance()->ProcessOperations(GetWorld()->GetDeltaSeconds());
}

USpatialOS* ASpatialPlusGameMode::GetSpatialOS()
{
	return GetGameInstance()->GetSpatialOS();
}

void ASpatialPlusGameMode::UnbindEntityQueryCallback()
{
	if(EntityQueryCallback != -1)
	{
		const auto SpatialOS = GetSpatialOS();
		auto LockedDispatcher = SpatialOS->GetView().Pin();
		if(LockedDispatcher.IsValid())
		{
			LockedDispatcher->Remove(EntityQueryCallback);
			EntityQueryCallback = -1;
		}
	}
}