#include "Database/DatabaseLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Base64.h"

FString UDatabaseLibrary::LoadFirebaseApiKey()
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

void UDatabaseLibrary::AuthRequest(const FString& Email, const FString& Password)
{
    FString FirebaseApiKey = LoadFirebaseApiKey();

    if (FirebaseApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Missing API Key"));
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

    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Firebase auth request failed"));
            return;
        }

        FString ResponseStr = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Firebase auth response: %s"), *ResponseStr);

        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            FString IdToken = JsonResponse->GetStringField("idToken");
            UE_LOG(LogTemp, Log, TEXT("Firebase idToken: %s"), *IdToken);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Could not parse JSON"));
        }
    });

    Request->ProcessRequest();
}

void UDatabaseLibrary::RegisterRequest(const FString& UserName, const FString& Email, const FString& Password)
{
}

void UDatabaseLibrary::GetData(const FString& UserID, const FString& DataID)
{
    
}
