//GI_BattleInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_BattleInstance.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString ID;

	UPROPERTY()
	FString Nickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> CharacterClass;

	// 기본 생성자
	FPlayerInfo()
		: ID(TEXT("")), Nickname(TEXT("")), PlayerNum(0), CharacterClass(nullptr)
	{
	}

	// 닉네임 + 캐릭터 클래스 생성자
	FPlayerInfo(const FString& InID, const FString& InNickname, int32 InPlayerNum, TSubclassOf<APawn> InClass)
		: ID(InID), Nickname(InNickname), PlayerNum(InPlayerNum), CharacterClass(InClass)
	{
	}
};


UCLASS()
class TEAM05_API UGI_BattleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	bool RegisterPlayerID(const FString& ID, FString& OutNickname, int32& OutPlayerNum);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	bool RegisterNickname(const FString& ID, const FString& Nickname);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	bool RegisterCharacterClass(const FString& Nickname, TSubclassOf<APawn> CharacterClass);

	// 고유 ID를 통해 선택된 캐릭터 클래스를 반환
	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	TSubclassOf<APawn> GetCharacterClass(const FString& ID) const;

	// 로그아웃용 
	bool RemovePlayerInfo(const FString& ID);

	// 게임 시작여부 확인용
	bool HasInitializedLobby() const { return bHasInitializedLobby; }
	void SetInitializedLobby(bool bInit) { bHasInitializedLobby = bInit; }

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<FPlayerInfo> CachedSpawnList;

	UFUNCTION(BlueprintCallable)
	void AddPlayerSpawnInfo(int32 PlayerNum, TSubclassOf<APawn> CharacterClass);

public:

	UPROPERTY()
	TMap<FString, FPlayerInfo> PlayerInfoMap;

	bool bHasInitializedLobby = false;
};
