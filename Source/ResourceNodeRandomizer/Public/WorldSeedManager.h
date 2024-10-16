#pragma once

#include "CoreMinimal.h"
#include "FGSaveInterface.h"
#include "ProceduralGenerator.h"
#include "WorldSeedManager.generated.h"

UCLASS()
class RESOURCENODERANDOMIZER_API AWorldSeedManager : public AActor, public IFGSaveInterface
{
	GENERATED_BODY()
	
public:	
	AWorldSeedManager();

	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaTime) override;

public:
	void OnWorldInitialized(const UWorld::FActorsInitializedParams& someActorsInitializedParams);

	void InitRandom();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Replicated) // , ReplicatedUsing = OnRep_SeedChanged)
	int32 Seed = ProceduralGenerator::InvalidSeed;

	virtual bool ShouldSave_Implementation() const override;
	virtual void PreSaveGame_Implementation(int32 SaveVersion, int32 GameVersion) override;
	virtual void PostLoadGame_Implementation(int32 SaveVersion, int32 GameVersion) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
