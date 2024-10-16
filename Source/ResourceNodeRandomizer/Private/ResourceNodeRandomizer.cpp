#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#include "ResourceNodeRandomizer.h"
#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "FResourceNodeRandomizerModule"

TSharedPtr<FOutputDeviceFile> FResourceNodeRandomizerModule::WeakLogDevice = nullptr;

void FResourceNodeRandomizerModule::StartupModule()
{
    // UE_LOG don't logging for some reason
    FString LogFilePath = FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA")), TEXT("FactoryGame/Saved/Logs/ResourceNodeRandomizer/Logs.txt"));
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(LogFilePath), true);
    
    if (!WeakLogDevice.IsValid())
    {
        WeakLogDevice = MakeShareable(new FOutputDeviceFile(*LogFilePath, true));
    }

    //

    ResourceNodeSpawner = NewObject<USolidResourceNodeSpawner>();
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FResourceNodeRandomizerModule::OnPostLoadMap);
}

void FResourceNodeRandomizerModule::ShutdownModule()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

    if (WeakLogDevice.IsValid())
    {
        FOutputDeviceFile* CustomLog = WeakLogDevice.Get();
        if (CustomLog)
        {
            CustomLog->TearDown();
        }

        WeakLogDevice.Reset();
    }
}

//

void FResourceNodeRandomizerModule::Log(FString LogData)
{
    if (WeakLogDevice.IsValid())
    {
        FOutputDeviceFile* LogFilePtr = WeakLogDevice.Get();
        if (LogFilePtr)
        {
            FDateTime Now = FDateTime::Now();
            FString Timestamp = Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));

            LogFilePtr->Logf(TEXT("[%s] %s"), *Timestamp, *LogData);
        }
    }
}

//

// In menu
void FResourceNodeRandomizerModule::OnPostLoadMap(UWorld* LoadedWorld)
{
    float scanInterval = 1.5f;
    LoadedWorld->GetTimerManager().SetTimer(ScanTimerHandle, FTimerDelegate::CreateRaw(this, &FResourceNodeRandomizerModule::ScanWorldForResourceNodes, LoadedWorld), scanInterval, true);
}

void FResourceNodeRandomizerModule::ScanWorldForResourceNodes(UWorld* LoadedWorld)
{
    if (!GEngine || !LoadedWorld) return;

    //

    if (ResourceNodeSpawner->WorldIsLoaded(LoadedWorld))
    {
        if (!SeedManager.IsValid())
        {
            for (TActorIterator<AWorldSeedManager> It(LoadedWorld->GetWorld()); It; ++It)
            {
                AWorldSeedManager* NewSeedManager = *It;

                if (!SeedManager.IsValid())
                {
                    SeedManager = NewSeedManager;
                }
                else
                {
                    if (SeedManager != NewSeedManager)
                    {
                        NewSeedManager->Destroy();
                    }
                }
            }

            if (!SeedManager.IsValid())
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                SeedManager = LoadedWorld->SpawnActor<AWorldSeedManager>(AWorldSeedManager::StaticClass(), FTransform(FVector()), SpawnParams);
                if (SeedManager.IsValid())
                {
                    SeedManager->InitRandom();
                }
            }

            if (SeedManager.IsValid())
            {
                ResourceNodeSpawner->Seed = SeedManager->Seed;
            }
        }

        ResourceNodeSpawner->ScanWorldResourceNodes(LoadedWorld);
        ResourceNodeSpawner->ReplaceStandardResourceNodesUpdate(LoadedWorld);

    }
    else
    {
        ResourceNodeSpawner->Cleanup(LoadedWorld);

        if (SeedManager.IsValid())
        {
            AWorldSeedManager* SeedManagerStrong = SeedManager.Get();
            if (SeedManagerStrong->IsValidLowLevel())
            {
                SeedManagerStrong->Destroy();
            }
        }
        SeedManager = nullptr;
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FResourceNodeRandomizerModule, ResourceNodeRandomizer)