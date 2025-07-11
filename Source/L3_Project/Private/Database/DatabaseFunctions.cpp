#include "Database/DatabaseFunctions.h"

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "CharacterManagement/CharacterManagerSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Templates/SharedPointer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Base64.h"
#include "Misc/Guid.h"
#include "HAL/PlatformFilemanager.h"
#include "Networking/BaseGameInstance.h"

FString UDatabaseFunctions::LoadFirebaseApiKey()
{
    FString JsonRaw;
    const FString FilePath = FPaths::ProjectContentDir() / TEXT("Keys.json");

    if (!FFileHelper::LoadFileToString(JsonRaw, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Loading Keys.json failed"));
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

    UE_LOG(LogTemp, Error, TEXT("API Key missing in Keys.json"));
    return FString();
}

bool UDatabaseFunctions::LoadFirebaseAdminConfig(FString& OutAdminID, FString& OutAdminPassword)
{
    FString JsonRaw;
    const FString FilePath = FPaths::ProjectContentDir() / TEXT("Keys.json");

    if (!FFileHelper::LoadFileToString(JsonRaw, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Loading Keys.json failed"));
        return false;
    }

    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonRaw);

    if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
    {
        TSharedPtr<FJsonObject> ApiKeysObject = RootObject->GetObjectField(TEXT("ServerInfos"));
        if (ApiKeysObject.IsValid())
        {
            if (ApiKeysObject->TryGetStringField(TEXT("Email"), OutAdminID) && 
                ApiKeysObject->TryGetStringField(TEXT("Password"), OutAdminPassword))
            {
                return true;
            }
        }
    }
    
    UE_LOG(LogTemp, Error, TEXT("can't find ServerInfos in Keys.json"));
    return false;
}

void UDatabaseFunctions::Logout(UObject* WorldContext)
{
    if (!IsValid(WorldContext))
    {
        UE_LOG(LogTemp, Error, TEXT("WorldContext is not valid during logout"));
        return;
    }
    
    const auto GameInstance = Cast<UBaseGameInstance>(WorldContext->GetWorld()->GetGameInstance());
    if (!IsValid(GameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is not valid during logout"));
        return;
    }

    if (const auto CharacterManager = GameInstance->GetSubsystem<UCharacterManagerSubsystem>();
        IsValid(GameInstance->GetSubsystem<UCharacterManagerSubsystem>()))
    {
        CharacterManager->OnFirebaseLogout();
    }
    
    GameInstance->OnFirebaseLogout();
}

FString UDatabaseFunctions::HashString(const FString& Target)
{
    return FMD5::HashAnsiString(*Target);
}

FString UDatabaseFunctions::GetIdToken()
{
    return UBaseGameInstance::FirebaseIdToken;
}

void UDatabaseFunctions::CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback)
{
    // This call doesn't work... Commented one should, but we need .indexOn in UserNames.
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/UserNames/%s.json?auth=null"),
        /*TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/UserNames.json?orderBy=\"Name\"&equalTo=\"%s\"&auth=null"),*/
        *Username);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("CheckUserAvailability failed"));
            Callback(false);
            return;
        }

        //UE_LOG(LogTemp, Error, TEXT("CheckUserAvailability response: %s"), *Response->GetContentAsString());
        
        const FString ResponseStr = Response->GetContentAsString();
        Callback(ResponseStr == "null");
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::CheckCharacterNameAvailability(const FString& UserName, const FString& CharacterName, const FString& IdToken, const TFunction<void(bool)>& Callback)
{
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/Players/%s/Characters/%s.json?auth=%s"),
        *UserName, *CharacterName, *IdToken);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("CheckCharacterNameAvailability failed"));
            Callback(false);
            return;
        }

        const FString ResponseStr = Response->GetContentAsString();
        Callback(ResponseStr == "null");
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::AuthRequest(const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    const FString FirebaseApiKey = LoadFirebaseApiKey();

    if (FirebaseApiKey.IsEmpty())
    {
        OnFailure.ExecuteIfBound("Missing API Key");
        return;
    }

    const FString Url = FString::Printf(TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=%s"), *FirebaseApiKey);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("email", Email);
    JsonObject->SetStringField("password", Password);
    JsonObject->SetBoolField("returnSecureToken", true);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("Firebase auth request failed");
            return;
        }

        const FString ResponseStr = Response->GetContentAsString();

        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            if (JsonResponse->HasField(TEXT("error")))
            {
                FString ErrorMessage = JsonResponse->GetObjectField(TEXT("error"))->GetStringField(TEXT("message"));
                OnFailure.ExecuteIfBound(ErrorMessage);
                return;
            }

            FString localId = JsonResponse->GetStringField(TEXT("localId"));
            UBaseGameInstance::FirebaseIdToken = JsonResponse->GetStringField(TEXT("idToken"));

            OnSuccess.ExecuteIfBound(localId);
        }
        else
        {
            OnFailure.ExecuteIfBound("Could not parse JSON");
        }
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::RegisterRequest(const FString& UserName, const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    const FString FirebaseApiKey = LoadFirebaseApiKey();

    if (FirebaseApiKey.IsEmpty())
    {
        OnFailure.ExecuteIfBound("Missing API Key");
        return;
    }

    const FString Url = FString::Printf(TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=%s"), *FirebaseApiKey);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("email", Email);
    JsonObject->SetStringField("password", Password);
    JsonObject->SetBoolField("returnSecureToken", true);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);

    CheckUserAvailability(UserName, [Request, OnFailure, OnSuccess](bool bAvailable)
    {
        if (!bAvailable)
        {
            OnFailure.ExecuteIfBound("Name already exists");
            return;
        }

        Request->OnProcessRequestComplete().BindLambda([OnFailure, OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!bWasSuccessful || !Response.IsValid())
            {
                OnFailure.ExecuteIfBound("Firebase register request failed");
                return;
            }

            const FString ResponseStr = Response->GetContentAsString();

            TSharedPtr<FJsonObject> JsonResponse;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
            if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
            {
                if (JsonResponse->HasField(TEXT("error")))
                {
                    FString ErrorMessage = JsonResponse->GetObjectField(TEXT("error"))->GetStringField(TEXT("message"));
                    OnFailure.ExecuteIfBound(ErrorMessage);
                    return;
                }

                FString localId = JsonResponse->GetStringField(TEXT("localId"));
                UBaseGameInstance::FirebaseIdToken = JsonResponse->GetStringField(TEXT("idToken"));

                OnSuccess.ExecuteIfBound(localId);
            }
            else
            {
                OnFailure.ExecuteIfBound("Could not parse JSON");
            }
        });

        Request->ProcessRequest();
    });
}

void UDatabaseFunctions::SetPostRegisterData(const FString& UserName, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString CharacterID = FGuid::NewGuid().ToString();

    TSharedPtr<FJsonObject> CharacterJson = MakeShareable(new FJsonObject);
    CharacterJson->SetStringField("Name", UserName);
    CharacterJson->SetNumberField("WeaponID", 0);
    CharacterJson->SetNumberField("SelectedSpells", 0);
    CharacterJson->SetNumberField("LVL", 1);
    CharacterJson->SetNumberField("Death", 0);
    CharacterJson->SetNumberField("Win", 0);
    CharacterJson->SetNumberField("Lose", 0);

    FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserName, *CharacterID);
    //SetData(Path, CharacterJson, IdToken, OnSuccess, OnFailure);
}

void UDatabaseFunctions::LinkUserIDAndName(const FString& UserName, const FString& UserId, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
    Json->SetStringField("Name", UserName);

    FString Path = FString::Printf(TEXT("UserNames/%s"), *UserId);
    SetData(Path, Json, IdToken, OnSuccess, OnFailure);
}

void UDatabaseFunctions::GetData(const FString& Path, const FString& DataID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s/%s.json?auth=%s"),
        *Path, *DataID, *IdToken);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("Get Data failed");
            return;
        }

        FString ResponseStr = Response->GetContentAsString();
        OnSuccess.ExecuteIfBound(ResponseStr);
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
        *Path, *IdToken);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(Data.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("PUT");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("Failed to send request to Firebase");
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.ExecuteIfBound(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.ExecuteIfBound(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::DeleteData(const FString& Path, const FString& IdToken, const FSuccess& OnSuccess,
    const FFailed& OnFailure)
{
    const FString Url = FString::Printf(
    TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
    *Path, *IdToken);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("DELETE");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("Failed to send request to Firebase");
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.ExecuteIfBound(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.ExecuteIfBound(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::SetPlayerData(const FString& UserName, const FString& FieldName, const FString& NewValue, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    // Prépare le JSON pour une mise à jour partielle
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(FieldName, NewValue);

    // Construit le chemin complet du joueur et personnage
    const FString Path = FString::Printf(TEXT("Players/%s"), *UserName);

    // Construit l'URL Firebase
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
        *Path, *IdToken);

    // Sérialise le JSON
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Prépare la requête PATCH
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("PATCH");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(RequestBody);

    // Gère la réponse
    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("SetPlayerData: Firebase PATCH request failed");
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.ExecuteIfBound(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.ExecuteIfBound(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::SetCharacterData_String(const FString& UserName, const FString& CharacterID, const FString& FieldName, const FString& NewValue, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    // Prépare le JSON pour une mise à jour partielle
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(FieldName, NewValue);

    // Construit le chemin complet du joueur et personnage
    const FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserName, *CharacterID);

    // Construit l'URL Firebase
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
        *Path, *IdToken);

    // Sérialise le JSON
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Prépare la requête PATCH
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("PATCH");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(RequestBody);

    // Gère la réponse
    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("SetPlayerData: Firebase PATCH request failed");
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.ExecuteIfBound(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.ExecuteIfBound(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::SetCharacterData_Int(const FString& UserName, const FString& CharacterID, const TArray<FString>& FieldName,
    const TArray<int>& NewValue, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    // Prépare le JSON pour une mise à jour partielle
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    for(auto i = 0; i < FieldName.Num(); i++)
    {
        if (i < NewValue.Num())
        {
            JsonObject->SetNumberField(FieldName[i], NewValue[i]);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SetCharacterData_Int: FieldName and NewValue arrays have different lengths"));
        }
    }

    // Construit le chemin complet du joueur et personnage
    const FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserName, *CharacterID);

    // Construit l'URL Firebase
    const FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
        *Path, *IdToken);

    // Sérialise le JSON
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Prépare la requête PATCH
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("PATCH");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(RequestBody);

    // Gère la réponse
    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("SetPlayerData: Firebase PATCH request failed");
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.ExecuteIfBound(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.ExecuteIfBound(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}

FString UDatabaseFunctions::CreateCharacter(const FString& UserName, const FString& IdToken, const FString& CharacterName, int WeaponID, int SelectedSpells, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString CharacterID = FGuid::NewGuid().ToString();

    TSharedPtr<FJsonObject> CharacterJson = MakeShareable(new FJsonObject);
    CharacterJson->SetStringField("Name", CharacterName);
    CharacterJson->SetNumberField("WeaponID", WeaponID);
    CharacterJson->SetNumberField("SelectedSpells", SelectedSpells);
    CharacterJson->SetNumberField("SelectedSkin", 0);
    CharacterJson->SetNumberField("Death", 0);
    CharacterJson->SetNumberField("Win", 0);
    CharacterJson->SetNumberField("Lose", 0);

    FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserName, *CharacterID);
    SetData(Path, CharacterJson, IdToken, OnSuccess, OnFailure);
    return CharacterID;
}

void UDatabaseFunctions::DeleteCharacter(const FString& UserID, const FString& CharacterID, const FString& IdToken,
    const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserID, *CharacterID);
    DeleteData(Path, IdToken, OnSuccess, OnFailure);
}

void UDatabaseFunctions::GetCharacterData(APlayerController* PlayerController, const FString& UserID, const FString& CharacterID, const FString& IdToken,
    const FGetCharacterCallback& Callback)
{
    FString Path = FString::Printf(TEXT("Players/%s/Characters/%s"), *UserID, *CharacterID);

    FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s.json?auth=%s"),
        *Path, *IdToken);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([PlayerController, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            Callback.ExecuteIfBound(false, "Failed to fetch character data", PlayerController);
            return;
        }

        FString ResponseStr = Response->GetContentAsString();

        if (ResponseStr == "null")
        {
            Callback.ExecuteIfBound(false, "Character or User not found", PlayerController);
            return;
        }
        
        Callback.ExecuteIfBound(true, ResponseStr, PlayerController);
    });

    Request->ProcessRequest();
}

void UDatabaseFunctions::GetAllCharacters(const FString& UID, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/Players/%s/Characters.json?auth=%s"),
        *UID, *IdToken);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.ExecuteIfBound("Failed to fetch characters");
            return;
        }

        FString ResponseStr = Response->GetContentAsString();
        OnSuccess.ExecuteIfBound(ResponseStr);
    });

    Request->ProcessRequest();
}
