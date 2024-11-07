#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "FGResourceNode.h"
#include "VisualResourceNodeInfo.h"

class FCustomResourceNode
{
public:
    FCustomResourceNode(FVector InLocation, EOreType InOreType, EResourcePurity InPurity);

    void SetLocation(FVector NewLocation);
    void SetOreType(EOreType NewOreType);
    void SetPurity(EResourcePurity NewPurity);

    FVector GetLocation() const;
    EOreType GetOreType() const;
    EResourcePurity GetPurity() const;

    bool operator==(const FCustomResourceNode& other) const;

private:
    FVector Location;
    EOreType OreType;
    EResourcePurity Purity;
};


class ResourceNodeGrouper
{
public:
    void AddResourceNode(const FCustomResourceNode& ResourceNode);
    void GroupNodes(float DistanceToGroup);

private:
    void GroupResourceNodes(float DistanceToGroup);
    void RemoveGroupedNodesFromStandardList();

public:
    TMap<EOreType, TMap<EResourcePurity, int>> OreTypeWithPurity;
    TArray<TArray<FCustomResourceNode>> ResourceNodeGroups;
    TMap<EOreType, TArray<FCustomResourceNode>> StandardResourceNodeList;
};
