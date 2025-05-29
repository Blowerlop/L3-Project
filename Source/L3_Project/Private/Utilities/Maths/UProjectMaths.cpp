// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/Maths/UProjectMaths.h"

FVector UProjectMaths::GetRandomPointInAnnulusOrDonutOrRingWhateverYouWant2D(const FVector origin, const float innerRadius, const float outerRadius)
{
    const float randomAngle = FMath::RandRange(0.0f, 2 * PI);
    const float randomRadius = FMath::Sqrt(FMath::RandRange(0.0f, 1.0f)) * (outerRadius - innerRadius) + innerRadius;
   
    const float x = randomRadius * FMath::Cos(randomAngle);
    const float y = randomRadius * FMath::Sin(randomAngle);

    return FVector(origin.X + x, origin.Y + y, origin.Z);
}

FVector UProjectMaths::GetRandomPointInAnnulusOrDonutOrRingWhateverYouWant3D(const FVector origin, const float innerRadius, const float outerRadius, const float height)
{
    const float randomAngle = FMath::RandRange(0.0f, 2 * PI);
    const float randomRadius = FMath::Sqrt(FMath::RandRange(0.0f, 1.0f)) * (outerRadius - innerRadius) + innerRadius;
       
    const float x = randomRadius * FMath::Cos(randomAngle);
    const float y = randomRadius * FMath::Sin(randomAngle);
    
    const float halfHeight = height * 0.5f;
    float z = FMath::RandRange(-halfHeight, halfHeight);
    
    return FVector(origin.X + x, origin.Y + y, origin.Z + z);
}

bool UProjectMaths::IsSubsetSum(TArray<int> array, int32 target)
{
    int32 N = array.Num();
    TArray<bool> Prev, Curr;
    Prev.Init(false, target + 1);
    Curr.Init(false, target + 1);

    Prev[0] = true;

    for (int32 i = 1; i <= N; ++i)
    {
        for (int32 j = 0; j <= target; ++j)
        {
            if (j < array[i - 1])
            {
                Curr[j] = Prev[j];
            }
            else
            {
                Curr[j] = Prev[j] || Prev[j - array[i - 1]];
            }
        }
        Prev = Curr;
    }
    return Prev[target];
}

TSet<int32> UProjectMaths::FindExtraValuesToReachTarget(const TArray<int32> array, int32 Target)
{
    TSet<int32> Sums;
    Sums.Add(0);

    for (int32 Num : array)
    {
        TSet<int32> NewSums;
        for (int32 ExistingSum : Sums)
        {
            NewSums.Add(ExistingSum + Num);
        }
        Sums.Append(NewSums);
    }

    TSet<int32> ExtraValues;
    for (int32 Sum : Sums)
    {
        int32 Extra = Target - Sum;
        if (Extra > 0)
        {
            ExtraValues.Add(Extra);
        }
    }

    return ExtraValues;
}
