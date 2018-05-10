#include "ExportSnapshotCommandlet.h"

#include "EntityBuilder.h"
#include "improbable/collections.h"
#include "improbable/standard_library.h"
#include <improbable/worker.h>
#include <array>

using namespace improbable;

using Components = worker::Components<Persistence, EntityAcl, Position, Metadata>;

const int GSpawnerEntityId = 1;

int32 UExportSnapshotCommandlet::Main(const FString& Params)
{
	// TODO: Replace LogTemp
    FString CombinedPath = FPaths::Combine(*FPaths::GetPath(FPaths::GetProjectFilePath()), TEXT("../../../snapshots"));
    UE_LOG(LogTemp, Display, TEXT("Combined path %s"), *CombinedPath);
	
    if (FPaths::CollapseRelativeDirectories(CombinedPath))
        GenerateSnapshot(CombinedPath);
    else
        UE_LOG(LogTemp, Display, TEXT("Path was invalid - snapshot not generated"));

    return 0;
}

void UExportSnapshotCommandlet::GenerateSnapshot(const FString& savePath) const
{
    const FString FullPath = FPaths::Combine(*savePath, TEXT("default.snapshot"));

    std::unordered_map<worker::EntityId, worker::Entity> SnapshotEntities;
    SnapshotEntities.emplace(std::make_pair(GSpawnerEntityId, CreateSpawnerEntity()));
    worker::Option<std::string> Result = worker::SaveSnapshot(Components{}, TCHAR_TO_UTF8(*FullPath), SnapshotEntities);
    if (!Result.empty())
    {
	    auto ErrorString = Result.value_or("");
        UE_LOG(LogTemp, Display, TEXT("Error: %s"), UTF8_TO_TCHAR(ErrorString.c_str()));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *FullPath);
    }
}

worker::Entity UExportSnapshotCommandlet::CreateSpawnerEntity() const
{
    const Coordinates InitialPosition{0, 0, 0};

    const WorkerAttributeSet UnrealWorkerAttributeSet{worker::List<std::string>{"UnrealWorker"}};
    const WorkerAttributeSet UnrealClientAttributeSet{worker::List<std::string>{"UnrealClient"}};

    const WorkerRequirementSet UnrealWorkerWritePermission{{UnrealWorkerAttributeSet}};
    const WorkerRequirementSet AnyWorkerReadPermission{{UnrealClientAttributeSet, UnrealWorkerAttributeSet}};

    auto SnapshotEntity =
        unreal::FEntityBuilder::Begin()
            .AddPositionComponent(Position::Data{InitialPosition}, UnrealWorkerWritePermission)
            .AddMetadataComponent(Metadata::Data("Spawner"))
            .SetPersistence(true)
            .SetReadAcl(AnyWorkerReadPermission)
            .Build();

    return SnapshotEntity;
}