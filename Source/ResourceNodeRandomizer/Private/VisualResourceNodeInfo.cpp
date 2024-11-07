#include "VisualResourceNodeInfo.h"
#include "ResourceNodeRandomizer.h"

VisualResourceNodeInfo::VisualResourceNodeInfo(EOreType InOreType, FString InMeshPath, FString InMaterialPath, FVector InOffset, FVector InScale)
	: OreType(InOreType), MeshPath(InMeshPath), MaterialPath(InMaterialPath), Offset(InOffset), Scale(InScale)
{
}

bool VisualResourceNodeInfo::TryLoadFromAssets()
{
	if (Loaded) return true;

	Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
	if (!Mesh)
	{
		FResourceNodeRandomizerModule::Log(FString::Printf(
			TEXT("Failed to load mesh | OreType: %d | MeshPath: %s | MaterialPath: %s"),
			static_cast<uint8>(OreType), *MeshPath, *MaterialPath
		));
		return false;
	}

	Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));
	if (!Material)
	{
		FResourceNodeRandomizerModule::Log(FString::Printf(
			TEXT("Failed to load material | OreType: %d | MeshPath: %s | MaterialPath: %s"),
			static_cast<uint8>(OreType), *MeshPath, *MaterialPath
		));
		return false;
	}

	FResourceNodeRandomizerModule::Log(FString::Printf(
		TEXT("Successfully loaded assets | OreType: %d | MeshPath: %s | MaterialPath: %s | Offset: %s | Scale: %s"),
		static_cast<uint8>(OreType), *MeshPath, *MaterialPath, *Offset.ToString(), *Scale.ToString()
	));

	Loaded = true;
	return true;
}
