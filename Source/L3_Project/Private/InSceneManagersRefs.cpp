// Fill out your copyright notice in the Description page of Project Settings.


#include "InSceneManagersRefs.h"


void UInSceneManagersRefs::RegisterManager(UClass* ManagerClass, AActor* ManagerInstance)
{
	if(InSceneManagers.Contains(ManagerClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Manager of type %s already registered"), *ManagerClass->GetName());
        return;
    }
    
    InSceneManagers.Add(ManagerClass, ManagerInstance);
}

void UInSceneManagersRefs::UnregisterManager(UClass* ManagerClass)
{
	if (!InSceneManagers.Contains(ManagerClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Manager of type %s not registered"), *ManagerClass->GetName());
		return;
	}
	
	InSceneManagers.Remove(ManagerClass);
}

AActor* UInSceneManagersRefs::GetManager(UClass* ManagerClass) const
{
	return InSceneManagers.FindRef(ManagerClass);
}
