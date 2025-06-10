#pragma once
#include "Effectable.generated.h"

class UEffectResolver;
class UEffectSystemConfiguration;
enum class EEffectType : uint8;
class UEffectDataAsset;
class UEffectInstance;

///
// UEffectable is only relevant on server. Effects will be applied on server and their consequences will be replicated to clients.
///

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
	
	UPROPERTY()
	TArray<UEffectInstance*> Instances;
};

class UEffectable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEffectsReplicated);
DECLARE_MULTICAST_DELEGATE_FourParams(FSrvOnEffectAdded, UEffectable*, UEffectDataAsset*, AActor*, FGuid); 
DECLARE_MULTICAST_DELEGATE_ThreeParams(FSrvOnEffectRemoved, UEffectable*, UEffectDataAsset*, FGuid);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class L3_PROJECT_API UEffectable : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UEffectable();

	static FSrvOnEffectAdded SrvOnEffectAddedDelegate;
	static FSrvOnEffectRemoved SrvOnEffectRemovedDelegate;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UEffectSystemConfiguration* Configuration;

	// pas ouf les 3 mêmes fonctions
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UEffectInstance* SrvAddEffect(UEffectDataAsset* EffectData, AActor* Applier);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvAddEffects(UPARAM(ref) const TArray<UEffectDataAsset*>& Effects, AActor* Applier);

	// Unreal UFunction doesn't support overloading
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvAddEffectsWithBuffer(UPARAM(ref) const TArray<UEffectDataAsset*>& Effects, AActor* Applier, TArray<UEffectInstance*>& OutAppliedEffects);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvRemoveEffect(UEffectInstance* Effect);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SrvRemoveEffects(UPARAM(ref) const TArray<UEffectInstance*>& Effects);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Cleanse();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Debuff();
	
	UPROPERTY(BlueprintReadOnly)
	TMap<EEffectType, UEffectInstanceContainer*> EffectsByType;

	UPROPERTY(BlueprintReadOnly)
	TMap<EEffectType, UEffectResolver*> ResolversCache;

	UPROPERTY(BlueprintReadOnly)
	TArray<EEffectType> ActiveResolvers;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEffectsReplicated OnEffectsReplicatedDelegate;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<UEffectDataAsset*, int> ReplicatedEffects;

	UFUNCTION(NetMulticast, Reliable)
	void AddEffectMulticast(UEffectDataAsset* Effect);
	UFUNCTION(NetMulticast, Reliable)
	void AddEffectsMulticast(const TArray<UEffectDataAsset*>& Effects);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveEffectMulticast(UEffectDataAsset* Effect);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveEffectsMulticast(const TArray<UEffectDataAsset*>& Effects);
	
private:
	TArray<UEffectInstance*> RemoveEffectsBuffer{};
	
	UEffectInstanceContainer* GetEffectContainer(EEffectType Type);

	void Refresh();

	static void GetValueForEachEffect(TMap<UEffectDataAsset*, float>& ValuesBuffer, TMap<UEffectDataAsset*, int>& EffectCounts);

	UEffectResolver* GetResolver(EEffectType Type);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
