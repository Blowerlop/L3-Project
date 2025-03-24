#pragma once
#include "Effectable.generated.h"

class UEffectResolver;
class UEffectSystemConfiguration;
enum class EEffectType : uint8;
class UEffectDataAsset;
class UEffectInstance;

UCLASS(BlueprintType)
class UEffectInstanceContainer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEffectInstance* GetInstance(const int Index) const
	{
		return Instances.IsValidIndex(Index) ? Instances[Index] : nullptr;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEffectInstance* GetInstanceUnsafe(const int Index) const
	{
		return Instances[Index];
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetNumInstances() const
	{
		return Instances.Num();
	}
	
	UFUNCTION(BlueprintCallable)
	void AddInstance(UEffectInstance* Instance)
	{
		Instances.Add(Instance);
	}
	
	UFUNCTION(BlueprintCallable)
	void RemoveInstance(UEffectInstance* Instance)
	{
		Instances.Remove(Instance);
	}
	
private:
	UPROPERTY()
	TArray<UEffectInstance*> Instances;
};

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class L3_PROJECT_API UEffectable : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UEffectSystemConfiguration* Configuration;
	
	UFUNCTION(BlueprintCallable)
	void AddEffect(UEffectDataAsset* EffectData, AActor* Applier);

	UFUNCTION(BlueprintCallable)
	void RemoveEffect(UEffectInstance* Effect);
	
	UPROPERTY(BlueprintReadOnly)
	TMap<EEffectType, UEffectInstanceContainer*> EffectsByType;

	UPROPERTY(BlueprintReadOnly)
	TMap<EEffectType, UEffectResolver*> ResolversCache;

	UPROPERTY(BlueprintReadOnly)
	TArray<EEffectType> ActiveResolvers;

private:
	UEffectInstanceContainer* GetEffectContainer(EEffectType Type);

	void Refresh();

	static void GetValueForEachEffect(TArray<float>& ValuesBuffer, TMap<UEffectDataAsset*, int>& EffectCounts);

	UEffectResolver* GetResolver(EEffectType Type);
};
