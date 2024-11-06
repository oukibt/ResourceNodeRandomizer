#include "WorldSeedManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "ResourceNodeRandomizer.h"

AWorldSeedManager::AWorldSeedManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AWorldSeedManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// void AWorldSeedManager::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

void AWorldSeedManager::OnWorldInitialized(const UWorld::FActorsInitializedParams& someActorsInitializedParams)
{
    FResourceNodeRandomizerModule::Log(FString::Printf(TEXT("Loaded Seed: %d"), Seed));
    ResourceNodeSpawner->Seed = Seed;

    ResourceNodeSpawner->Cleanup(GetWorld());
    ResourceNodeSpawner->ScanWorldResourceNodes(GetWorld());
    ResourceNodeSpawner->ReplaceStandardResourceNodesUpdate(GetWorld());
}

void AWorldSeedManager::PostLoadGame_Implementation(int32 SaveVersion, int32 GameVersion)
{
    FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &AWorldSeedManager::OnWorldInitialized);
}

int32 AWorldSeedManager::GenerateSeed() const
{
    // Seed = (int32)(FMath::Rand() * FMath::Rand()); // around 93.5m unique values from 0 to 0x7FFFFFFF
    return static_cast<int32>(FMath::Rand() * FMath::Rand());
}

void AWorldSeedManager::InitRandom()
{
    Seed = GenerateSeed();
    // Seed = (int32)(FMath::RandRange(0.0, 1.0) * 0x7FFFFFFF); // 32768 unique values only
    // Seed = FMath::RandRange(0, 0x7FFFFFFF); // 32768 unique values only

    FResourceNodeRandomizerModule::Log(FString::Printf(TEXT("Created Seed: %d"), Seed));
}

bool AWorldSeedManager::ShouldSave_Implementation() const
{
    return true;
}

void AWorldSeedManager::PreSaveGame_Implementation(int32 SaveVersion, int32 GameVersion)
{

}

void AWorldSeedManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWorldSeedManager, Seed);
}