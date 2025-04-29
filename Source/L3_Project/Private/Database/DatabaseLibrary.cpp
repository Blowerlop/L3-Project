#include "Database/DatabaseLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"

void UDatabaseLibrary::FirebaseAuthRequest(const FString& Email, const FString& Password)
{
    const FString FirebaseApiKey = TEXT("TODO API KEY");
    const FString Url = FString::Printf(TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=%s"), *FirebaseApiKey);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Corps JSON
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