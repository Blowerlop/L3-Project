#pragma once
#include "CoreMinimal.h"
#include "GroupManager.generated.h"

struct FClientData;
class UGroupableComponent;

USTRUCT(BlueprintType)
struct FInviteData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 GroupId{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 InviteId{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> GroupMembers{};
};

USTRUCT(BlueprintType)
struct FReplicatedGroupMemberData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 SelectedWeapon{};
};

class FServerGroupData
{
public:
	int32 GroupId{};
	
	TArray<UGroupableComponent*> GroupMembers{};

	void AddMember(UGroupableComponent* Player)
	{
		GroupMembers.Add(Player);
	}

	void RemoveMember(UGroupableComponent* Player)
	{
		GroupMembers.Remove(Player);
	}

	void RemoveMember(const UGroupableComponent* Player)
	{
		GroupMembers.Remove(const_cast<UGroupableComponent*>(Player));
	}

	TArray<FString> GetMembersAsString() const;
	
	TArray<FReplicatedGroupMemberData> GetAsReplicatedData() const;

	TArray<FClientData> GetMembersAsClientData() const;
};

USTRUCT(BlueprintType)
struct FReplicatedGroupData
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool IsValid = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 GroupId{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FReplicatedGroupMemberData> GroupMembers{};
};

class FGroupManager
{
public:
	static TMap<int32, FServerGroupData> Groups;

	static int CreateGroup(UGroupableComponent* Owner);
	static void AddToGroup(UGroupableComponent* Player, int32 GroupId);
	static void RemoveFromGroup(UGroupableComponent* Player, int32 GroupId);

	/// <summary>
	/// Will remove the player from the group without modifying player data.
	/// Modifying player replicated data when he is disconnecting could cause issues I guess.
	/// </summary>
	static void RemoveFromGroup(const UGroupableComponent* Player, int32 GroupId);
	
	static void DestroyGroup(int32 GroupId);
	
	static void InviteToGroup(UGroupableComponent* Inviter, UGroupableComponent* Invited);
	static void AcceptGroupInvite(UGroupableComponent* Invited, int32 InviteId);
	static bool IsGroupLeader(UGroupableComponent* Player);

	static FServerGroupData* GetGroup(int32 GroupId);

private:
	static int32 GroupIdCounter;

	static void RefreshGroup(FServerGroupData* Group);
};

