// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "FGResourceNode.h"
#include "VisualResourceNodeInfo.h"

class FCustomResourceNode
{
public:
    FCustomResourceNode(FVector InLocation, EOreType InOreType, EResourcePurity InPurity)
        : Location(InLocation), OreType(InOreType), Purity(InPurity)
    {
    }

    void SetLocation(FVector NewLocation)
    {
        Location = NewLocation;
    }

    void SetOreType(EOreType NewOreType)
    {
        OreType = NewOreType;
    }

    void SetPurity(EResourcePurity NewPurity)
    {
        Purity = NewPurity;
    }

    FVector GetLocation() const
    {
        return Location;
    }

    EOreType GetOreType() const
    {
        return OreType;
    }

    EResourcePurity GetPurity() const
    {
        return Purity;
    }

    bool operator==(const FCustomResourceNode& other) const
    {
        return (Location == other.Location && OreType == other.OreType && Purity == other.Purity);
    }

private:
    FVector Location;
    EOreType OreType;
    EResourcePurity Purity;
};

class ResourceNodeGrouper
{
public:
    void AddResourceNode(const FCustomResourceNode& ResourceNode)
    {
        EOreType OreType = ResourceNode.GetOreType();
        StandardResourceNodeList.FindOrAdd(OreType).Add(ResourceNode);

        EResourcePurity Purity = ResourceNode.GetPurity();
        TMap<EResourcePurity, int>& PurityMap = OreTypeWithPurity.FindOrAdd(OreType);
        PurityMap.FindOrAdd(Purity)++;
    }

    void GroupNodes(const float DistanceToGroup)
    {
        for (auto& ResourceNodePair : StandardResourceNodeList)
        {
            TArray<FCustomResourceNode>& ResourceNodes = ResourceNodePair.Value;
            ResourceNodes.Sort([](const FCustomResourceNode& A, const FCustomResourceNode& B)
            {
                return A.GetLocation().X < B.GetLocation().X;
            });
        }

        GroupResourceNodes(DistanceToGroup);
        RemoveGroupedNodesFromStandardList();
    }

private:
    void GroupResourceNodes(const float DistanceToGroup)
    {
        ResourceNodeGroups.Empty();

        for (auto& ResourceNodePair : StandardResourceNodeList)
        {
            TArray<FCustomResourceNode>& ResourceNodes = ResourceNodePair.Value;

            for (auto& ResourceNode : ResourceNodes)
            {
                bool bAlreadyInGroup = false;

                for (auto& Group : ResourceNodeGroups)
                {
                    if (Group.Contains(ResourceNode))
                    {
                        bAlreadyInGroup = true;
                        break;
                    }
                }

                if (bAlreadyInGroup)
                {
                    continue;
                }

                TArray<FCustomResourceNode> NewGroup;
                NewGroup.Add(ResourceNode);

                for (auto& AnotherResourceNode : ResourceNodes)
                {
                    if (ResourceNode == AnotherResourceNode) continue;

                    float Distance = FVector::Dist(AnotherResourceNode.GetLocation(), ResourceNode.GetLocation());

                    if (Distance > DistanceToGroup) continue;

                    for (auto& Group : ResourceNodeGroups)
                    {
                        if (Group.Contains(AnotherResourceNode))
                        {
                            NewGroup.Append(Group);
                            ResourceNodeGroups.Remove(Group);
                            break;
                        }
                    }

                    NewGroup.Add(AnotherResourceNode);
                }

                if (NewGroup.Num() > 1)
                {
                    ResourceNodeGroups.Add(NewGroup);
                }
            }
        }

        for (auto It = ResourceNodeGroups.CreateIterator(); It; ++It)
        {
            if (It->Num() <= 1)
            {
                It.RemoveCurrent();
            }
        }
    }

    void RemoveGroupedNodesFromStandardList()
    {
        for (const auto& Group : ResourceNodeGroups)
        {
            for (const auto& Node : Group)
            {
                for (auto ResourceNodePairIt = StandardResourceNodeList.CreateIterator(); ResourceNodePairIt; ++ResourceNodePairIt)
                {
                    ResourceNodePairIt.Value().RemoveSingle(Node);

                    if (ResourceNodePairIt.Value().Num() == 0)
                    {
                        ResourceNodePairIt.RemoveCurrent();
                    }
                }
            }
        }
    }

public:
    TMap<EOreType, TMap<EResourcePurity, int>> OreTypeWithPurity;
    TArray<TArray<FCustomResourceNode>> ResourceNodeGroups;
    TMap<EOreType, TArray<FCustomResourceNode>> StandardResourceNodeList;
};