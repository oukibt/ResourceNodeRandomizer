#include "SolidResourceNodeSpawner.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

#include "FGResourceNode.h"
#include "FGResourceNodeBase.h"
#include "FGResourceMiner.h"
#include "FGPortableMiner.h"
#include "FGBuildableResourceExtractor.h"
#include "Representation/FGResourceNodeRepresentation.h"
#include "ResourceNodeGrouper.h"

#include "ResourceNodeRandomizer.h"

#include "EngineUtils.h"

USolidResourceNodeSpawner::USolidResourceNodeSpawner()
{
}

int USolidResourceNodeSpawner::GetScannedUniqueResourceTypeSize() const
{
    return this->UniqueResourceNodeTypes.Num();
}

bool USolidResourceNodeSpawner::IsAllResourcesAlreadyScanned() const
{
    return this->UniqueResourceNodeTypes.Num() >= this->MaxUniqueClasses;
}

bool USolidResourceNodeSpawner::IsValidOreMeshPath(FString MeshPath)
{
    for (auto& asset : ValidResourceNodeAssets)
    {
        if (MeshPath.Equals(asset.MeshPath, ESearchCase::IgnoreCase))
        {
            return true;
        }
    }
    return false;
}

bool USolidResourceNodeSpawner::TryGetOreTypeByMeshPath(FString MeshPath, EOreType& OreType)
{
    for (auto& asset : ValidResourceNodeAssets)
    {
        if (MeshPath.Equals(asset.MeshPath, ESearchCase::IgnoreCase))
        {
            OreType = asset.OreType;
            return true;
        }
    }
    return false;
}

bool USolidResourceNodeSpawner::TryGetOreTypeFromName(FString Name, EOreType& OreType)
{
    for (auto& OrePair : OreTypeNameList)
    {
        if (Name.Equals(OrePair.Key, ESearchCase::IgnoreCase))
        {
            OreType = OrePair.Value;
            return true;
        }
    }

    return false;
}

bool USolidResourceNodeSpawner::TryGetNameFromOreType(EOreType OreType, FString& Name)
{
    for (auto& OrePair : OreTypeNameList)
    {
        if (OreType == OrePair.Value)
        {
            Name = OrePair.Key;
            return true;
        }
    }

    return false;
}

FVector USolidResourceNodeSpawner::GetCenterOfVectors(TArray<FVector> VectorList)
{
    int Count = VectorList.Num();
    if (Count == 0) return FVector();
    else if (Count == 1) return VectorList[0];

    // avoid low float precision

    float MaxLength = 0.0f;
    for (auto& Vector : VectorList)
    {
        float Length = Vector.Size();
        if (Length > MaxLength)
        {
            MaxLength = Length;
        }
    }

    if (MaxLength == 0.0f) return FVector();

    FVector VectorSum;
    for (auto& Vector : VectorList)
    {
        VectorSum += Vector / MaxLength;
    }

    return (VectorSum / Count) * MaxLength;
}

void USolidResourceNodeSpawner::ScanWorldResourceNodes(UWorld* World)
{
    if (!World) return;
    if (IsAllResourcesAlreadyScanned()) return;

///////////////////////////////////////////////////////////////
// Add Logging to find resources                             //
///////////////////////////////////////////////////////////////

 FResourceNodeRandomizerModule::Log(TEXT("=== Begin Logging All AFGResourceNodes in the World ==="));

    for (TActorIterator<AFGResourceNode> It(World); It; ++It)
    {
        AFGResourceNode* ResourceNode = *It;

        FString NodeName = ResourceNode->GetName();
        FString ResourceClass = ResourceNode->GetResourceClass() ? ResourceNode->GetResourceClass()->GetName() : TEXT("None");

        FString ResourceForm;
        switch (ResourceNode->GetResourceForm())
        {
            case EResourceForm::RF_SOLID: ResourceForm = TEXT("Solid"); break;
            case EResourceForm::RF_LIQUID: ResourceForm = TEXT("Liquid"); break;
            case EResourceForm::RF_GAS: ResourceForm = TEXT("Gas"); break;
            case EResourceForm::RF_HEAT: ResourceForm = TEXT("Heat"); break;
            default: ResourceForm = FString::Printf(TEXT("Unknown (%d)"), static_cast<int32>(ResourceNode->GetResourceForm())); break;
        }

        FString ResourceAmount;
        switch (ResourceNode->GetResourceAmount())
        {
            case EResourceAmount::RA_Poor: ResourceAmount = TEXT("Poor"); break;
            case EResourceAmount::RA_Normal: ResourceAmount = TEXT("Normal"); break;
            case EResourceAmount::RA_Rich: ResourceAmount = TEXT("Rich"); break;
            case EResourceAmount::RA_Infinite: ResourceAmount = TEXT("Infinite"); break;
            default: ResourceAmount = FString::Printf(TEXT("Unknown (%d)"), static_cast<int32>(ResourceNode->GetResourceAmount())); break;
        }

        FResourceNodeRandomizerModule::Log(FString::Printf(
            TEXT("%s|%s|%s|%s"),
            *NodeName, *ResourceClass, *ResourceForm, *ResourceAmount
        ));
    }

    FResourceNodeRandomizerModule::Log(TEXT("=== End Logging All AFGResourceNodes in the World ==="));

///////////////////////////////////////////////////////////////
// End Logging to find resources                             //
///////////////////////////////////////////////////////////////


    TSet<TSubclassOf<UFGResourceDescriptor>> UniqueResourceClasses;

    // ResouceNode (logical) and their ore meshes are not connected (Even with ResouceNode->GetMeshActor()).
    for (TActorIterator<AFGResourceNode> It(World); It; ++It)
    {
        if (UniqueResourceClasses.Num() >= this->MaxUniqueClasses) break;

        AFGResourceNode* ResourceNode = *It;
        if (ResourceNode->GetResourceNodeType() == EResourceNodeType::Node &&
            ResourceNode->GetResourceForm() == EResourceForm::RF_SOLID &&
            ResourceNode->GetResourceAmount() == EResourceAmount::RA_Infinite) // avoid deposits
        {
            if (!ResourceNodeClass)
            {
                ResourceNodeClass = ResourceNode->GetClass();
            }

            UniqueResourceClasses.FindOrAdd(ResourceNode->GetResourceClass());
        }
    }

    if (UniqueResourceClasses.Num() < this->MaxUniqueClasses) return;

    for (auto& ResourceClass : UniqueResourceClasses)
    {
        UTexture2D* CompasTexture = UFGResourceDescriptor::GetCompasTexture(ResourceClass);
        if (!CompasTexture) continue;

        FString TexturePath = CompasTexture->GetPathName();
        EOreType CurrentOreType;

        int PartIndex;
        for (PartIndex = 0; PartIndex < OreTypeNameList.Num(); PartIndex++)
        {
            if (TexturePath.Contains(OreTypeNameList[PartIndex].Key, ESearchCase::IgnoreCase))
            {
                CurrentOreType = OreTypeNameList[PartIndex].Value;
                break;
            }
        }
        if (PartIndex >= OreTypeNameList.Num()) continue;

        for (auto& asset : ValidResourceNodeAssets)
        {
            if (CurrentOreType == asset.OreType)
            {
                asset.ResourceClass = ResourceClass;
                UniqueResourceNodeTypes.Add(CurrentOreType, asset);

                break;
            }
        }
    }

    if (IsAllResourcesAlreadyScanned())
    {
        for (auto& resource : UniqueResourceNodeTypes)
        {
            if (!resource.Value.TryLoadFromAssets())
            {
                FResourceNodeRandomizerModule::Log(TEXT("ERROR: Failed to load resource node assets"));
            }
        }
    }
}

TPair<AFGResourceNode*, float> USolidResourceNodeSpawner::GetClosestCustomResourceNode(UWorld* World, const FVector Location, EOccupiedType OccupiedType)
{
    TPair<AFGResourceNode*, float> ClosestNode = MakeTuple(nullptr, FLT_MAX);

    for (auto& CustomResourceNode : CustomResourceNodeMap)
    {
        AFGResourceNode* ResourceNode = CustomResourceNode.Key;
        if (OccupiedType != EOccupiedType::Any)
        {
            bool Occupied = ResourceNode->IsOccupied();

            if (OccupiedType == EOccupiedType::Unoccupied && Occupied) continue;
            if (OccupiedType == EOccupiedType::Occupied && !Occupied) continue;
        }

        float CurrentDistance = FVector::Dist(Location, ResourceNode->GetActorLocation());
        if (CurrentDistance >= ClosestNode.Value) continue;

        ClosestNode.Key = ResourceNode;
        ClosestNode.Value = CurrentDistance;
    }

    return ClosestNode;
}

bool USolidResourceNodeSpawner::WorldIsLoaded(UWorld* World)
{
    // standard or custom
    for (TActorIterator<AFGResourceNode> It(World); It; ++It)
    {
        return true;
    }

    return false;
}

void USolidResourceNodeSpawner::ReplaceStandardResourceNodesUpdate(UWorld* World)
{
    if (!IsAllResourcesAlreadyScanned()) return;
    ProceduralGenerator Randomizer(Seed);

    // Destroy Resource Nodes Meshes

    // (6.0ms) (it's threaded operation, so not a big deal)
    for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
    {
        UStaticMeshComponent* StaticMeshComponent = *It;
        if (StaticMeshComponent && StaticMeshComponent->GetWorld() == World)
        {
            if (UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh())
            {
                if (IsValidOreMeshPath(StaticMesh->GetPathName()) &&
                    !StaticMeshComponent->ComponentTags.Contains(CustomResourceNodeTag))
                {
                    StaticMeshComponent->SetActive(false);
                    StaticMeshComponent->SetVisibility(false);
                    StaticMeshComponent->DestroyComponent();
                }
            }
        }
    }

    /*
    // faster but not work properly always (0.5ms)
    for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
    {
        TArray<UStaticMeshComponent*> StaticMeshComponents;
        (*It)->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

        for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
        {
            if (UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh())
            {
                if (IsValidOreMeshPath(StaticMesh->GetPathName()) && !StaticMeshComponent->ComponentTags.Contains(CustomResourceNodeTag))
                {
                    StaticMeshComponent->SetActive(false);
                    StaticMeshComponent->SetVisibility(false);
                    StaticMeshComponent->DestroyComponent();
                }
            }
        }
    }
    */

    TArray<FCustomResourceNode> StandardResourceNodeCollection;

    // Destroy Resource Nodes
    for (TActorIterator<AFGResourceNode> It(World); It; ++It)
    {
        AFGResourceNode* ResourceNode = *It;
        if (ResourceNode->GetResourceNodeType() == EResourceNodeType::Node &&
            ResourceNode->GetResourceForm() == EResourceForm::RF_SOLID &&
            ResourceNode->GetResourceAmount() == EResourceAmount::RA_Infinite)
        {
            if (CustomResourceNodeMap.Contains(ResourceNode)) continue;

            UTexture2D* CompasTexture = UFGResourceDescriptor::GetCompasTexture(ResourceNode->GetResourceClass());
            if (!CompasTexture) continue;

            FString TexturePath = CompasTexture->GetPathName();
            EOreType CurrentOreType;

            int PartIndex;
            for (PartIndex = 0; PartIndex < OreTypeNameList.Num(); PartIndex++)
            {
                if (TexturePath.Contains(OreTypeNameList[PartIndex].Key, ESearchCase::IgnoreCase))
                {
                    CurrentOreType = OreTypeNameList[PartIndex].Value;
                    break;
                }
            }
            if (PartIndex >= OreTypeNameList.Num()) continue;

            //

            FCustomResourceNode CustomNodeDescription(ResourceNode->GetActorLocation(), CurrentOreType, ResourceNode->GetResoucePurity());
            StandardResourceNodeCollection.Add(CustomNodeDescription);

            ResourceNode->Destroy();
        }
    }

    if (!StandardResourceNodeCollection.IsEmpty())
    {
        ResourceNodeGrouper NodeGrouper;
        for (auto& ResourceNode : StandardResourceNodeCollection)
        {
            NodeGrouper.AddResourceNode(ResourceNode);
        }

        NodeGrouper.GroupNodes(7000.0f); // 70m

        // Groups
        for (auto& Group : NodeGrouper.ResourceNodeGroups)
        {
            TArray<FVector> GroupLocations;
            for (auto& ResourceNode : Group)
            {
                GroupLocations.Add(ResourceNode.GetLocation());
            }

            FVector Center = GetCenterOfVectors(GroupLocations);
            int RandomResourceTypeIndex = Randomizer.ProceduralRangeIntByVector(Center, 0, MakeGroupTypes.Num() - 1);
            EOreType NewResourcesNodeOreType = MakeGroupTypes[RandomResourceTypeIndex];

            for (auto& ResourceNode : Group)
            {
                if (MakeGroupTypes.Contains(ResourceNode.GetOreType()))
                {
                    SpawnCustomResourceNode(World, NewResourcesNodeOreType, ResourceNode.GetLocation(), ResourceNode.GetPurity(), true);
                }
                else
                {
                    SpawnCustomResourceNode(World, ResourceNode.GetOreType(), ResourceNode.GetLocation(), ResourceNode.GetPurity(), true);
                }
            }
        }

        // Invididuals
        TArray<FCustomResourceNode> CustomResourceNodesList;
        for (auto& OreArray : NodeGrouper.StandardResourceNodeList)
        {
            if (ShuffleTypes.Contains(OreArray.Key))
            {
                CustomResourceNodesList.Append(OreArray.Value);
            }
        }

        // Procedural Shuffle
        const int32 LastResourceNodeIndex = CustomResourceNodesList.Num() - 1;
        for (int32 i = LastResourceNodeIndex; i > 0; i--)
        {
            FVector LocationForSwapA = CustomResourceNodesList[i].GetLocation();

            int32 ProceduralIndex = Randomizer.ProceduralRangeIntByVector(LocationForSwapA, 0, i);

            if (i != ProceduralIndex)
            {
                FVector TempLocation = CustomResourceNodesList[i].GetLocation();
                CustomResourceNodesList[i].SetLocation(CustomResourceNodesList[ProceduralIndex].GetLocation());
                CustomResourceNodesList[ProceduralIndex].SetLocation(TempLocation);
            }
        }

        for (auto& OreArray : NodeGrouper.StandardResourceNodeList)
        {
            if (!ShuffleTypes.Contains(OreArray.Key))
            {
                CustomResourceNodesList.Append(OreArray.Value);
            }
        }

        for (auto& ResourceNode : CustomResourceNodesList)
        {
            SpawnCustomResourceNode(World, ResourceNode.GetOreType(), ResourceNode.GetLocation(), ResourceNode.GetPurity(), true);
        }
    }

    //

    for (TActorIterator<AFGBuildableResourceExtractor> It(World); It; ++It)
    {
        AFGBuildableResourceExtractor* ResourceExtractor = *It;

        if (CustomResourceNodeMap.Contains(Cast<AFGResourceNode>(ResourceExtractor->mExtractableResource))) continue;
        if (ResourceExtractor->GetExtractorTypeName() != TEXT("Miner")) continue; // Ore miners only (any mk)

        FVector ExtractorLocation = ResourceExtractor->GetActorLocation() - FVector(0.0f, 0.0f, 150.0f); // lower == closer
        TPair<AFGResourceNode*, float> ClosestNode = GetClosestCustomResourceNode(World, ExtractorLocation, EOccupiedType::Unoccupied);

        if (ClosestNode.Key && ClosestNode.Value < 700.0f) // 7m
        {
            ClosestNode.Key->SetIsOccupied(true);
            ResourceExtractor->mExtractableResource = ClosestNode.Key;
        }
        // I'm not sure what functionality this may break, but it appears that custom nodes
        // Are handled gracefully already by the engine, and so this may be un-necessary
//        else
//        {
//            ResourceExtractor->Destroy();
//        }
    }

    for (TActorIterator<AFGPortableMiner> It(World); It; ++It)
    {
        AFGPortableMiner* PortableMiner = *It;

        if (CustomResourceNodeMap.Contains(Cast<AFGResourceNode>(PortableMiner->mExtractResourceNode))) continue;

        FVector ExtractorLocation = PortableMiner->GetActorLocation();
        TPair<AFGResourceNode*, float> ClosestNode = GetClosestCustomResourceNode(World, ExtractorLocation, EOccupiedType::Any);

        if (ClosestNode.Key && ClosestNode.Value < 1500.0f) // 15m
        {
            PortableMiner->mExtractResourceNode = ClosestNode.Key;
        }
        // I'm not sure what functionality this may break, but it appears that custom nodes
        // Are handled gracefully already by the engine, and so this may be un-necessary
//        else
//        {
//            PortableMiner->Destroy();
//        }
    }
}

bool USolidResourceNodeSpawner::SpawnCustomResourceNode(UWorld* World, EOreType OreType, FVector Location, const EResourcePurity Purity, bool bUseRaycastAdjust)
{
    if (!IsAllResourcesAlreadyScanned()) return false;
    ProceduralGenerator Randomizer(Seed);

    // Spawn Node

    AFGResourceNode* CustomNode = World->SpawnActor<AFGResourceNode>(ResourceNodeClass, Location, FRotator());
    if (!CustomNode) return false;

    VisualResourceNodeInfo ResourceNodeInfo = UniqueResourceNodeTypes[OreType];

    CustomNode->InitResource(ResourceNodeInfo.ResourceClass, EResourceAmount::RA_Infinite, Purity);

    CustomNode->mResourceNodeType = EResourceNodeType::Node;
    CustomNode->mCanPlacePortableMiner = true;
    CustomNode->mCanPlaceResourceExtractor = true;

    UActorComponent* RootComponent = CustomNode->GetRootComponent();
    if (!RootComponent)
    {
        UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(CustomNode);
        MeshComp->RegisterComponent();
        CustomNode->SetRootComponent(MeshComp);
    }

    CustomNode->GetRootComponent()->SetMobility(EComponentMobility::Movable);
    CustomNode->SetActorLocation(Location);

    if (!CustomNode->mResourceNodeRepresentation)
    {
        CustomNode->mResourceNodeRepresentation = NewObject<UFGResourceNodeRepresentation>(CustomNode);
    }

    CustomNode->mResourceNodeRepresentation->SetupResourceNodeRepresentation(CustomNode);

    CustomNode->UpdateMeshFromDescriptor();
    CustomNode->UpdateNodeRepresentation();

    CustomNode->InitRadioactivity();
    CustomNode->UpdateRadioactivity();

    CustomNode->mBoxComponent->SetWorldLocation(Location);
    CustomNode->mBoxComponent->SetWorldScale3D(FVector(30.0f, 30.0f, 2.0f));
    CustomNode->mBoxComponent->SetCollisionProfileName("Resource");

    // Spawn Mesh

    AActor* NewActor = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform(Location));
    if (!NewActor) return false;

    UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(NewActor);
    if (!MeshComponent) return false;

    //

    MeshComponent->SetCollisionProfileName("ResourceMesh");
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
    MeshComponent->SetGenerateOverlapEvents(false);

    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
    MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);

    //

    MeshComponent->ComponentTags.Add(CustomResourceNodeTag);
    MeshComponent->SetStaticMesh(ResourceNodeInfo.Mesh);
    MeshComponent->SetMaterial(0, ResourceNodeInfo.Material);
    MeshComponent->SetMobility(EComponentMobility::Movable);

    NewActor->SetRootComponent(MeshComponent);
    MeshComponent->RegisterComponent();

    //

    FRotator Rotation = FRotator(0.0f, Randomizer.ProceduralRangeFloatByVector(Location, 0.0f, 360.0f), 0.0f);
    MeshComponent->SetWorldRotation(Rotation);
    MeshComponent->SetWorldScale3D(ResourceNodeInfo.Scale);

    // Some meshes have a displaced pivot
    FVector Min, Max;
    MeshComponent->GetLocalBounds(Min, Max);

    FVector MeshPivot = (Min + Max) / 2.0f;

    FTransform MeshTransform = MeshComponent->GetComponentTransform();
    FVector AdjustedPivot = MeshTransform.TransformPosition(MeshPivot);

    FVector CorrectedLocation = Location - (AdjustedPivot - MeshTransform.GetLocation()) + ResourceNodeInfo.Offset;
    MeshComponent->SetWorldLocation(CorrectedLocation);

    //

    CustomResourceNodeMap.Add(CustomNode, MeshComponent);

    return true;
}

void USolidResourceNodeSpawner::Cleanup(UWorld* World)
{
    ResourceNodeClass = nullptr;
    CustomResourceNodeMap.Empty();
    UniqueResourceNodeTypes.Empty();
}