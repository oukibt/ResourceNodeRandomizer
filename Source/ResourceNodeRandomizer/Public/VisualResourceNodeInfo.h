#pragma once

#include "CoreMinimal.h"
#include "FGResourceDescriptor.h"

enum class EOreType : uint8
{
    Bauxite,
    Caterium, // Gold
    Coal,
    Copper,
    Iron,
    Limestone, // Stone
    Quartz,
    SAM,
    Sulfur,
    Uranium,
};

class VisualResourceNodeInfo
{
public:
    VisualResourceNodeInfo(EOreType InOreType, FString InMeshPath, FString InMaterialPath, FVector InOffset, FVector InScale)
        : OreType(InOreType), MeshPath(InMeshPath), MaterialPath(InMaterialPath), Offset(InOffset), Scale(InScale)
    {
    }

    //

    bool TryLoadFromAssets()
    {
        if (Loaded) return true;

        Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
        if (!Mesh) return false;

        Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));
        if (!Material) return false;

        Loaded = true;
        return true;
    }

    bool Loaded = false;

public:
    EOreType OreType;
    FString MeshPath;
    FString MaterialPath;
    FVector Offset;
    FVector Scale;

    TSubclassOf<UFGResourceDescriptor> ResourceClass = NULL;
    UStaticMesh* Mesh = nullptr;
    UMaterialInterface* Material = nullptr;
};