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
