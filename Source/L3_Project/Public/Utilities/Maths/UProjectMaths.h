#pragma once

#include "CoreMinimal.h"
#include "UProjectMaths.generated.h"

/**
 * 
 */
UCLASS()
class L3_PROJECT_API UProjectMaths : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Maths")
	static FVector GetRandomPointInAnnulusOrDonutOrRingWhateverYouWant2D(const FVector origin, const float innerRadius, const float outerRadius);
	
	UFUNCTION(BlueprintCallable, Category="Maths")
	static FVector GetRandomPointInAnnulusOrDonutOrRingWhateverYouWant3D(const FVector origin, const float innerRadius, const float outerRadius, const float height);
};
