#include "ProceduralGenerator.h"
#include "Math/UnrealMathUtility.h"

ProceduralGenerator::ProceduralGenerator()
{
}

ProceduralGenerator::ProceduralGenerator(int32 InSeed) : Seed(InSeed)
{
}

int32 ProceduralGenerator::ProceduralRangeIntByVector(int32 Seed, FVector Input, int32 Min, int32 MaxInclusive)
{
    int32 XHash = HashVector(Input);
    int32 CombinedSeed = (Seed ^ XHash);
    FRandomStream RandomStream(CombinedSeed);

    return RandomStream.RandRange(Min, MaxInclusive);
}

float ProceduralGenerator::ProceduralRangeFloatByVector(int32 Seed, FVector Input, float Min, float MaxInclusive)
{
    int32 XHash = HashVector(Input);
    int32 CombinedSeed = (Seed ^ XHash);
    FRandomStream RandomStream(CombinedSeed);

    return RandomStream.FRandRange(Min, MaxInclusive);
}

int32 ProceduralGenerator::ProceduralRangeIntByFloat(int32 Seed, float Input, int32 Min, int32 MaxInclusive)
{
    int32 InputHash = HashFloat(Input);
    int32 CombinedSeed = (Seed ^ InputHash);
    FRandomStream RandomStream(CombinedSeed);

    return RandomStream.RandRange(Min, MaxInclusive);
}

float ProceduralGenerator::ProceduralRangeFloatByFloat(int32 Seed, float Input, float Min, float MaxInclusive)
{
    int32 InputHash = HashFloat(Input);
    int32 CombinedSeed = (Seed ^ InputHash);
    FRandomStream RandomStream(CombinedSeed);

    return RandomStream.FRandRange(Min, MaxInclusive);
}

int32 ProceduralGenerator::HashVector(FVector Input)
{
    int32 XHash = static_cast<int32>(FMath::FloorToInt(Input.X) * 73856093);
    int32 YHash = static_cast<int32>(FMath::FloorToInt(Input.Y) * 19349663);
    int32 ZHash = static_cast<int32>(FMath::FloorToInt(Input.Z) * 83492791);

    return XHash ^ (YHash << 1) ^ (ZHash << 2);
}

int32 ProceduralGenerator::HashFloat(float Input)
{
    union
    {
        float f; int32 i;
    } u;
    u.f = Input;

    int32 FloatBitsHash = u.i ^ (u.i << 13) ^ (u.i >> 17) ^ (u.i << 5);

    int32 MixedHash = static_cast<int32>(FloatBitsHash * 98765.4321f);

    MixedHash ^= (MixedHash << 11);
    MixedHash ^= (MixedHash >> 19);
    MixedHash ^= (MixedHash << 7);

    return MixedHash;
}

//

int32 ProceduralGenerator::ProceduralRangeIntByVector(FVector Input, int32 Min, int32 MaxInclusive)
{
    return ProceduralRangeIntByVector(Seed, Input, Min, MaxInclusive);
}

float ProceduralGenerator::ProceduralRangeFloatByVector(FVector Input, float Min, float MaxInclusive)
{
    return ProceduralRangeFloatByVector(Seed, Input, Min, MaxInclusive);
}

int32 ProceduralGenerator::ProceduralRangeIntByFloat(float Input, int32 Min, int32 MaxInclusive)
{
    return ProceduralRangeIntByFloat(Seed, Input, Min, MaxInclusive);
}

float ProceduralGenerator::ProceduralRangeFloatByFloat(float Input, float Min, float MaxInclusive)
{
    return ProceduralRangeFloatByFloat(Seed, Input, Min, MaxInclusive);
}