// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Results/ActorColorAimResultHolder.h"

void UActorColorAimResultHolder::Set(AActor* InActor, FColor InColor)
{
	Actor = InActor;
	Color = InColor;
}
