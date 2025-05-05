// Fill out your copyright notice in the Description page of Project Settings.


#include "Database/DatabaseFunctions.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

/*FString UDatabaseFunctions::LoadFirebaseApiKey()
{
    FString JsonRaw;
    const FString FilePath = FPaths::ProjectContentDir() / TEXT("Keys.json");

    if (!FFileHelper::LoadFileToString(JsonRaw, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Loading Keys.json Failed"));
        return FString();
    }

    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonRaw);

    if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
    {
        TSharedPtr<FJsonObject> ApiKeysObject = RootObject->GetObjectField(TEXT("APIKeys"));
        if (ApiKeysObject.IsValid())
        {
            FString EncodedKey;
            if (ApiKeysObject->TryGetStringField(TEXT("FirebaseKey"), EncodedKey))
            {
                FString DecodedKey;
                FBase64::Decode(EncodedKey, DecodedKey);
                return DecodedKey;
            }
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Error Key missing"));
    return FString();
}

void UDatabaseFunctions::AuthRequest(const FString& Email, const FString& Password, const FAuthSuccess& OnSuccess, const FAuthFailed& OnFailure)
{
    
}

void UDatabaseFunctions::RegisterRequest(const FString& UserName, const FString& Email, const FString& Password)
{
}

void UDatabaseFunctions::GetData(const FString& UserID, const FString& DataID)
{
}*/
