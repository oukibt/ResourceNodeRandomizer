#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SolidResourceNodeSpawner.h"
#include "WorldSeedManager.h"
#include "Misc/OutputDeviceFile.h"

USolidResourceNodeSpawner* ResourceNodeSpawner = nullptr;

class FResourceNodeRandomizerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override
	{
		return true;
	}

	static void Log(FString LogData);

private:
	void OnPostLoadMap(UWorld* LoadedWorld);
	void ScanWorldForResourceNodes(UWorld* World);

	static TSharedPtr<FOutputDeviceFile> WeakLogDevice;
	TWeakObjectPtr<AWorldSeedManager> SeedManager = nullptr;

	FTimerHandle ScanTimerHandle;
};