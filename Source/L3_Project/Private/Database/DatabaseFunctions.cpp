// Fill out your copyright notice in the Description page of Project Settings.
 
 
 #include "Database/DatabaseFunctions.h"

#include "Templates/SharedPointer.h"
 #include "HttpModule.h"
 #include "Interfaces/IHttpResponse.h"
 #include "Dom/JsonObject.h"
 #include "Serialization/JsonReader.h"
 #include "Serialization/JsonSerializer.h"
 
 FString UDatabaseFunctions::LoadFirebaseApiKey()
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

void UDatabaseFunctions::CheckUserAvailability(const FString& Username, const TFunction<void(bool)>& Callback)
{
     FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/Players/%s.json"),
        *Username);

     TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
     Request->SetURL(Url);
     Request->SetVerb("GET");
     Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

     Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
     {
         if (!bWasSuccessful || !Response.IsValid())
         {
             UE_LOG(LogTemp, Error, TEXT("Username check failed"));
             Callback(false);
             return;
         }

         FString ResponseStr = Response->GetContentAsString();
         UE_LOG(LogTemp, Log, TEXT("Username check response: %s"), *ResponseStr);

         bool bIsAvailable = ResponseStr == "null";
         Callback(bIsAvailable);
     });

     Request->ProcessRequest();
}

FString UDatabaseFunctions::HashString(const FString& target)
{
     return  FMD5::HashAnsiString(*target);
}

void UDatabaseFunctions::AuthRequest(const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure)
 {
     FString FirebaseApiKey = LoadFirebaseApiKey();

     if (FirebaseApiKey.IsEmpty())
     {
         UE_LOG(LogTemp, Error, TEXT("Missing API Key"));
         OnFailure.Execute("Missing API Key");
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

     Request->OnProcessRequestComplete().BindLambda([OnFailure, OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
     {
         if (!bWasSuccessful || !Response.IsValid())
         {
             UE_LOG(LogTemp, Error, TEXT("Firebase auth request failed"));
             OnFailure.Execute("Firebase auth request failed");
             return;
         }

         FString ResponseStr = Response->GetContentAsString();
         UE_LOG(LogTemp, Log, TEXT("Firebase auth response: %s"), *ResponseStr);

         TSharedPtr<FJsonObject> JsonResponse;
         TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
         if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
         {
             if (JsonResponse->HasField("error"))
             {
                 FString ErrorMessage = JsonResponse->GetObjectField("error")->GetStringField("message");
                 UE_LOG(LogTemp, Error, TEXT("Firebase error: %s"), *ErrorMessage);
                 OnFailure.Execute(ErrorMessage);
                 return;
             }
             
             FString IdToken = JsonResponse->GetStringField("localId");
             UE_LOG(LogTemp, Log, TEXT("Firebase idToken: %s"), *IdToken);
             OnSuccess.Execute(IdToken);
         }
         else
         {
             UE_LOG(LogTemp, Error, TEXT("Could not parse JSON"));
             OnFailure.Execute("Could not parse JSON");
         }
     });

     Request->ProcessRequest();
 }
 
 void UDatabaseFunctions::RegisterRequest(const FString& UserName, const FString& Email, const FString& Password, const FSuccess& OnSuccess, const FFailed& OnFailure)
 {
	FString FirebaseApiKey = LoadFirebaseApiKey();

     if (FirebaseApiKey.IsEmpty())
     {
         UE_LOG(LogTemp, Error, TEXT("Missing API Key"));
         OnFailure.Execute("Missing API Key");
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
             UE_LOG(LogTemp, Error, TEXT("Name already exists"));
             OnFailure.Execute("Name already exists");
             return;
         }
         
         Request->OnProcessRequestComplete().BindLambda([OnFailure, OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
         {
             if (!bWasSuccessful || !Response.IsValid())
             {
                 UE_LOG(LogTemp, Error, TEXT("Firebase register request failed"));
                 OnFailure.Execute("Firebase register request failed");
                 return;
             }

             FString ResponseStr = Response->GetContentAsString();
             UE_LOG(LogTemp, Log, TEXT("Firebase register response: %s"), *ResponseStr);

             TSharedPtr<FJsonObject> JsonResponse;
             TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
             if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
             {
                 if (JsonResponse->HasField("error"))
                 {
                     FString ErrorMessage = JsonResponse->GetObjectField("error")->GetStringField("message");
                     UE_LOG(LogTemp, Error, TEXT("Firebase error: %s"), *ErrorMessage);
                     OnFailure.Execute(ErrorMessage);
                     return;
                 }
                 
                 FString IdToken = JsonResponse->GetStringField("localId");
                 UE_LOG(LogTemp, Log, TEXT("Firebase idToken: %s"), *IdToken);
                 OnSuccess.Execute(IdToken);
             }
             else
             {
                 UE_LOG(LogTemp, Error, TEXT("Could not parse JSON"));
                 OnFailure.Execute("Could not parse JSON");
             }
         });

         Request->ProcessRequest();
     });
 }

void UDatabaseFunctions::SetPostRegisterData(const FString& UserName, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
 {
     TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
     Json->SetStringField("username", UserName);
     Json->SetNumberField("LVL", 1);
     Json->SetNumberField("Death", 0);
     Json->SetNumberField("Win", 0);
     Json->SetNumberField("Lose", 0);

     FString Path = FString::Printf(TEXT("Players/%s"), *UserName);
     
     SetData(Path, Json, IdToken, OnSuccess, OnFailure);
 }

void UDatabaseFunctions::LinkUserIDAndName(const FString& UserName, const FString& IdToken, const FString& UserId, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
     TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
     Json->SetStringField("Name", UserName);

     FString SafeUserId = FMD5::HashAnsiString(*UserId);
     
     FString Path = FString::Printf(TEXT("UserNames/%s"), *SafeUserId);
     
     SetData(Path, Json, IdToken, OnSuccess, OnFailure);
}

void UDatabaseFunctions::GetData(const FString& Path, const FString& DataID, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
     FString Url = FString::Printf(
        TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/%s/%s.json"),
        *Path, *DataID);

     TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
     Request->SetURL(Url);
     Request->SetVerb("GET");
     Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

     Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
     {
         if (!bWasSuccessful || !Response.IsValid())
         {
             UE_LOG(LogTemp, Error, TEXT("Get Data failed"));
             OnFailure.Execute("Get Data failed");
             return;
         }

         FString ResponseStr = Response->GetContentAsString();
         UE_LOG(LogTemp, Log, TEXT("Get Data response: %s"), *ResponseStr);
         OnSuccess.Execute(ResponseStr);
     });

     Request->ProcessRequest();
}

void UDatabaseFunctions::SetData(const FString& Path, const TSharedPtr<FJsonObject> Data, const FString& IdToken, const FSuccess& OnSuccess, const FFailed& OnFailure)
{
    FString FirebaseDatabaseUrl = TEXT("https://projet-l3-eb9d5-default-rtdb.europe-west1.firebasedatabase.app/"); // <-- remplace par ton projet

    // Construction de l'URL complète
    FString Url = FString::Printf(TEXT("%s/%s.json?auth=%s"), *FirebaseDatabaseUrl, *Path, *IdToken);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(Data.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("PUT")); // ou POST si tu veux ajouter plutôt qu’écraser
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (!bWasSuccessful || !Response.IsValid())
        {
            OnFailure.Execute(TEXT("Failed to send request to Firebase"));
            return;
        }

        if (Response->GetResponseCode() == 200)
        {
            OnSuccess.Execute(Response->GetContentAsString());
        }
        else
        {
            FString ErrorMsg = FString::Printf(TEXT("Firebase error: %s"), *Response->GetContentAsString());
            OnFailure.Execute(ErrorMsg);
        }
    });

    Request->ProcessRequest();
}