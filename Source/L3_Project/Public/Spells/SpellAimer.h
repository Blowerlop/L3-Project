// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpellAimer.generated.h"

class UAimResultHolder;

UCLASS()
class L3_PROJECT_API ASpellAimer : public APawn
{
	GENERATED_BODY()

public:
	ASpellAimer();
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void Init(USceneComponent* Socket);
	
	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void Stop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAimResultHolder* GetAimResult() const;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsAiming;

protected:
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* OwnerSocket;
	
	UFUNCTION(BlueprintCallable)
	void MoveToOwner();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Init_Internal();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Start_Internal();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Stop_Internal();

	UFUNCTION(BlueprintImplementableEvent)
	void Update_Internal();
};