// Fill out your copyright notice in the Description page of Project Settings.


#include "Spells/Results/VectorColorIntAimResultHolder.h"

void UVectorColorIntAimResultHolder::Set(FVector InVector, FColor InColor, int32 InInt)
{
	Vector = InVector;
	Color = InColor;
	Int = InInt;
}
