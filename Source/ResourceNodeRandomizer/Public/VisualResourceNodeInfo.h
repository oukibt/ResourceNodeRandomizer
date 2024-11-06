#pragma once

#include "CoreMinimal.h"
#include "FGResourceDescriptor.h"

enum class EOreType : uint8
{
    Bauxite,
    Caterium,
    Coal,
    Copper,
    Iron,
    Limestone,
    Quartz,
    SAM,
    Sulfur,
    Uranium,
};

class VisualResourceNodeInfo
{
public:
    VisualResourceNodeInfo(EOreType InOreType, FString InMeshPath, FString InMaterialPath, FVector InOffset, FVector InScale);

    bool TryLoadFromAssets();

    bool Loaded = false;

public:
    EOreType OreType;
    FString MeshPath;
    FString MaterialPath;
    FVector Offset;
    FVector Scale;

    TSubclassOf<UFGResourceDescriptor> ResourceClass = nullptr;
    UStaticMesh* Mesh = nullptr;
    UMaterialInterface* Material = nullptr;
};
