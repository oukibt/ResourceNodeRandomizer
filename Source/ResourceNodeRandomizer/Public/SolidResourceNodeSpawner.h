#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FGResourceNode.h"
#include "WorldSeedManager.h"
#include "VisualResourceNodeInfo.h"
#include "SolidResourceNodeSpawner.generated.h"

/**
 * Manages spawning and grouping of custom resource nodes in the game world.
 */
UCLASS()
class RESOURCENODERANDOMIZER_API USolidResourceNodeSpawner : public UObject
{
    GENERATED_BODY()

    enum class EOccupiedType : uint8
    {
        Any,
        Occupied,
        Unoccupied,
    };

public:
    USolidResourceNodeSpawner();

    const int MaxUniqueClasses = 10; // number of ore types in EOreType
    
    TPair<AFGResourceNode*, float> GetClosestCustomResourceNode(UWorld* World, const FVector Location, EOccupiedType OccupiedType = EOccupiedType::Any);
    int GetScannedUniqueResourceTypeSize() const;
    bool IsAllResourcesAlreadyScanned() const;
    bool WorldIsLoaded(UWorld* World);

    bool TryGetOreTypeFromName(FString Name, EOreType& OreType);
    bool TryGetNameFromOreType(EOreType OreType, FString& Name);

    void ScanWorldResourceNodes(UWorld* World);
    void ReplaceStandardResourceNodesUpdate(UWorld* World);
    void Cleanup(UWorld* World);
    
    TArray<VisualResourceNodeInfo> ValidResourceNodeAssets = {
        VisualResourceNodeInfo(EOreType::Copper,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_OreCopper_01.ResourceNode_OreCopper_01",
            "/Game/FactoryGame/Resource/RawResources/OreCopper/Material/ResourceNode_Copper_Inst.ResourceNode_Copper_Inst",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::Coal,
            "/Game/FactoryGame/Resource/RawResources/Nodes/CoalResource_01.CoalResource_01",
            "/Game/FactoryGame/Resource/RawResources/Coal/Material/CoalResource_01_Inst.CoalResource_01_Inst",
            FVector(0.0f, 0.0f, 70.0f),
            FVector(1.0f, 1.0f, 1.0f)),

        VisualResourceNodeInfo(EOreType::Bauxite,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_OreIron_01.ResourceNode_OreIron_01",
            "/Game/FactoryGame/Resource/RawResources/OreBauxite/Material/MI_ResourceNode_OreBauxite.MI_ResourceNode_OreBauxite",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::Iron,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_OreIron_01.ResourceNode_OreIron_01",
            "/Game/FactoryGame/Resource/RawResources/OreIron/Material/ResourceNode_Iron_Inst.ResourceNode_Iron_Inst",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::Uranium,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_OreIron_01.ResourceNode_OreIron_01",
            "/Game/FactoryGame/Resource/RawResources/OreUranium/Material/ResourceNode_OreUranium_Inst.ResourceNode_OreUranium_Inst",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::Caterium,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_OreGold_01.ResourceNode_OreGold_01",
            "/Game/FactoryGame/Resource/RawResources/OreGold/Material/ResourceNode_Gold_Inst.ResourceNode_Gold_Inst",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::Quartz,
            "/Game/FactoryGame/Resource/RawResources/Nodes/ResourceNode_quartz.ResourceNode_quartz",
            "/Game/FactoryGame/Resource/RawResources/OreQuartz/Material/ResourceNode_Quartz_Inst.ResourceNode_Quartz_Inst",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(2.0f, 2.0f, 2.5f)),

        VisualResourceNodeInfo(EOreType::SAM,
            "/Game/FactoryGame/Resource/RawResources/SAM/Mesh/SM_SAM_Node_01.SM_SAM_Node_01",
            "/Game/FactoryGame/Resource/RawResources/SAM/Material/MI_SAM_Node_01.MI_SAM_Node_01",
            FVector(0.0f, 0.0f, -30.0f),
            FVector(1.25f, 1.25f, 1.25f)),

        VisualResourceNodeInfo(EOreType::Sulfur,
            "/Game/FactoryGame/Resource/RawResources/Nodes/SulfurResource_01.SulfurResource_01",
            "/Game/FactoryGame/Resource/RawResources/Sulfur/Material/Resource_Sulfur_Inst.Resource_Sulfur_Inst",
            FVector(0.0f, 0.0f, 70.0f),
            FVector(1.0f, 1.0f, 1.0f)),

        VisualResourceNodeInfo(EOreType::Limestone,
            "/Game/FactoryGame/Resource/RawResources/Nodes/Resource_Stone_01.Resource_Stone_01",
            "/Game/FactoryGame/Resource/RawResources/Stone/Material/MI_ResourceNode_Stone_Blocks.MI_ResourceNode_Stone_Blocks",
            FVector(0.0f, 0.0f, 55.0f),
            FVector(1.85f, 1.85f, 1.85f)),
    };

    TArray<TPair<FString, EOreType>> OreTypeNameList {
        MakeTuple("Bauxite", EOreType::Bauxite),
        MakeTuple("Caterium", EOreType::Caterium),
        MakeTuple("Coal", EOreType::Coal),
        MakeTuple("Copper", EOreType::Copper),
        MakeTuple("Iron", EOreType::Iron),
        MakeTuple("Stone", EOreType::Limestone),
        MakeTuple("Quartz", EOreType::Quartz),
        MakeTuple("SAM", EOreType::SAM),
        MakeTuple("Sulfur", EOreType::Sulfur),
        MakeTuple("Uranium", EOreType::Uranium),
    };

    TArray<EOreType> ShuffleTypes {
        EOreType::Bauxite,
        EOreType::Caterium,
        EOreType::Coal,
        EOreType::Copper,
        EOreType::Iron,
        EOreType::Limestone,
        EOreType::Quartz,
        EOreType::Sulfur,
    };

    TArray<EOreType> MakeGroupTypes {
        EOreType::Caterium,
        EOreType::Coal,
        EOreType::Copper,
        EOreType::Iron,
        EOreType::Limestone,
        EOreType::Quartz,
        EOreType::Sulfur,
    };
    
    int32 Seed = 0;
    TMap<EOreType, VisualResourceNodeInfo> UniqueResourceNodeTypes;

private:
    bool SpawnCustomResourceNode(UWorld* World, EOreType OreType, FVector Location, const EResourcePurity Purity, bool bUseRaycastAdjust);
    bool IsValidOreMeshPath(FString MeshPath);
    bool TryGetOreTypeByMeshPath(FString MeshPath, EOreType& OreType);
    FVector GetCenterOfVectors(TArray<FVector> VectorList);
    void LogAllResourceNodes(UWorld* World);

    TMap<AFGResourceNode*, UStaticMeshComponent*> CustomResourceNodeMap;
    UClass* ResourceNodeClass = nullptr;
    const FName CustomResourceNodeTag = "CustomResourceNode";
};