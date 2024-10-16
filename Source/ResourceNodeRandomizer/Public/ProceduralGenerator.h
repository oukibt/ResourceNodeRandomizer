#pragma once

#include "CoreMinimal.h"

class RESOURCENODERANDOMIZER_API ProceduralGenerator
{
public:
    ProceduralGenerator();
    ProceduralGenerator(int32 InSeed);

    static int32 ProceduralRangeIntByVector(int32 Seed, FVector Input, int32 Min, int32 MaxInclusive);
    static float ProceduralRangeFloatByVector(int32 Seed, FVector Input, float Min, float MaxInclusive);
    static int32 ProceduralRangeIntByFloat(int32 Seed, float Input, int32 Min, int32 MaxInclusive);
    static float ProceduralRangeFloatByFloat(int32 Seed, float Input, float Min, float MaxInclusive);

    int32 ProceduralRangeIntByVector(FVector Input, int32 Min, int32 MaxInclusive);
    float ProceduralRangeFloatByVector(FVector Input, float Min, float MaxInclusive);
    int32 ProceduralRangeIntByFloat(float Input, int32 Min, int32 MaxInclusive);
    float ProceduralRangeFloatByFloat(float Input, float Min, float MaxInclusive);

    static const int32 InvalidSeed = -1;
    int32 Seed = InvalidSeed;

private:

    static int32 HashVector(FVector Input);
    static int32 HashFloat(float Input);
};
