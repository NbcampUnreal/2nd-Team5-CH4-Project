//GM_BattleMode.cpp

#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
#include "GameModes/GI_BattleInstance.h"
#include "Level/Spawn.h"
#include "Kismet/GameplayStatics.h"


AGM_BattleMode::AGM_BattleMode()
{
	// SeamlessTravel 활성화
	bUseSeamlessTravel = true;
}

void AGM_BattleMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AGS_BattleState* GS = GetGameState<AGS_BattleState>();
	if (IsValid(GS) == false)
	{
		return;
	}

	if (GS->MatchState != EMatchState::Waiting)
	{
		AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer);
		PC->Client_KickWithMessage(TEXT("게임이 진행중이라 입장이 불가능합니다."));
		return;
	}

	// 최대 인원 초과 시 퇴장 처리 (예: 4명 제한)
	if (AlivePlayerControllers.Num() >= 4) // 또는 별도 변수 사용 가능
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer))
		{
			PC->Client_KickWithMessage(TEXT("최대 인원이 초과되어 입장이 제한됩니다."));
		}
		return;
	}

	if (HasAuthority())
	{
		// 닉네임 설정 로직 추가
		if (APS_PlayerState* PS = Cast<APS_PlayerState>(NewPlayer->PlayerState))
		{
			if (AMyPlayerController* MPC = Cast<AMyPlayerController>(NewPlayer))
			{
				FString ID = MPC->GetPlayerUniqueID();

				if (!ID.IsEmpty())
				{
					if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
					{
						if (GI->PlayerInfoMap.Contains(ID))
						{
							// PlayerState에 닉네임 설정
							FString StoredNickname = GI->PlayerInfoMap[ID].Nickname;
							if (!StoredNickname.IsEmpty())
							{
								PS->SetPlayerNickName(StoredNickname);
								UE_LOG(LogTemp, Log, TEXT("[BattleMode] Set nickname: %s for player ID: %s"), *StoredNickname, *ID);
							}
						}
					}
				}
			}
		}

		FString PlayerName = NewPlayer->GetName();

		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer))
			{
				FString ID = PC->GetPlayerUniqueID(); // Controller에서 고유 ID 얻는 함수

				if (FPlayerInfo* Info = GI->PlayerInfoMap.Find(ID))
				{
					if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
					{
						PS->SetMatchHealth(Info->MatchHealth);
					}
				}
			}
		}

		if (IsValid(GS))
		{
			const FString Msg = FString::Printf(TEXT("%s Welcome!"), *PlayerName);
			GS->Multicast_PrintMessage(Msg);
			AMyPlayerController* NewPC = Cast<AMyPlayerController>(NewPlayer);
			if (IsValid(NewPC))
			{
				AlivePlayerControllers.Add(NewPC);
			}
		}

		SpawnPlayerInBattle(NewPlayer);
	}
}

void AGM_BattleMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AMyPlayerController* ExitingPlayerController = Cast<AMyPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && AlivePlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);

		// ID 가져오기
		FString ID = ExitingPlayerController->GetPlayerUniqueID();

		// GameInstance에서 정보 삭제
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (!(GetWorld() && GetWorld()->IsInSeamlessTravel()))
			{
				if (GI->PlayerInfoMap.Remove(ID) > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("[Logout] Removed player info for ID: %s"), *ID);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Logout] Skipped removing player info during travel. ID: %s"), *ID);
			}
		}
	}
}


void AGM_BattleMode::OnCharacterDead(AMyPlayerController* InController)
{
	if (!IsValid(InController) || !AlivePlayerControllers.Contains(InController))
		return;

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);

	// 관전자 처리
	InController->ChangeState(NAME_Spectating);
	InController->UnPossess();

	// 사망 순서에 따른 데미지 계산
	int32 DeathOrder = DeadPlayerControllers.Num(); // 1이면 첫 번째 사망자
	int32 Damage = 0;

	switch (DeathOrder)
	{
	case 1: Damage = 100;	break;
	case 2: Damage = 100;	break;
	case 3: Damage = 100;	break;
	default: break;
	}

	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		FString PlayerID = InController->GetPlayerUniqueID(); // ID 가져오기

		if (FPlayerInfo* Info = GI->PlayerInfoMap.Find(PlayerID))
		{
			Info->MatchHealth = FMath::Clamp(Info->MatchHealth - Damage, 0, 100);

			// 순위 설정 (체력이 0이 된 첫 시점)
			if (Info->MatchHealth <= 0 && Info->MatchRank == 0)
			{
				int32 Rank = GI->PlayerInfoMap.Num() - GI->PlayerRanking.Num(); // 4인 기준: 4 → 3 → 2
				Info->MatchRank = Rank;
				GI->PlayerRanking.Add(PlayerID);

				UE_LOG(LogTemp, Warning, TEXT("[BattleMode] Player %s is now Rank %d"), *PlayerID, Info->MatchRank);
			}

			if (APS_PlayerState* PS = Cast<APS_PlayerState>(InController->PlayerState))
			{
				PS->SetMatchHealth(Info->MatchHealth);
			}
		}

		// 생존자 수 확인
		int32 AliveCount = 0;
		for (const auto& Elem : GI->PlayerInfoMap)
		{
			if (Elem.Value.MatchHealth > 0)
				++AliveCount;
		}

		// 마지막 1인 생존자 → 1등 설정 및 결과창 출력
		if (AliveCount == 1)
		{
			// 유일한 생존자 찾기
			for (auto& Elem : GI->PlayerInfoMap)
			{
				if (Elem.Value.MatchHealth > 0 && Elem.Value.MatchRank == 0)
				{
					Elem.Value.MatchRank = 1; // 마지막 생존자 = 1등
					UE_LOG(LogTemp, Warning, TEXT("[BattleMode] Player %s is now Rank 1"), *Elem.Key);
					break;
				}
			}

			// PlayerInfoMap을 순위 기준으로 정렬
			TArray<FPlayerInfo> SortedInfos;
			GI->PlayerInfoMap.GenerateValueArray(SortedInfos);
			SortedInfos.Sort([](const FPlayerInfo& A, const FPlayerInfo& B) {
				return A.MatchRank < B.MatchRank;
				});

			// 랭킹 정보 배열 생성
			TArray<FPlayerRankingInfo> FinalRankingList;
			for (const FPlayerInfo& Info : SortedInfos)
			{
				FPlayerRankingInfo RankInfo;
				RankInfo.Nickname = Info.Nickname;
				RankInfo.Rank = Info.MatchRank;
				FinalRankingList.Add(RankInfo);
			}

			// UI 전송
			for (AMyPlayerController* MyPC : AlivePlayerControllers)
			{
				if (IsValid(MyPC))
					MyPC->Client_ReceiveRankingInfo(FinalRankingList);
			}
			for (AMyPlayerController* MyPC : DeadPlayerControllers)
			{
				if (IsValid(MyPC))
					MyPC->Client_ReceiveRankingInfo(FinalRankingList);
			}

			GetWorldTimerManager().ClearTimer(MainTimerHandle); // 종료 타이머 멈춤
		}
	}
}

void AGM_BattleMode::SpawnPlayerInBattle(APlayerController* Player)
{
	// GameInstance에서 캐릭터 정보 받아오기
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		PlayerSpawnList = GI->CachedSpawnList;
		UE_LOG(LogTemp, Log, TEXT("[Spawn] Retrieved %d player(s) from CachedSpawnList."), PlayerSpawnList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] GameInstance cast failed."));
		return;
	}

	AMyPlayerController* MyPC = Cast<AMyPlayerController>(Player);
	if (!MyPC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] Invalid PlayerController."));
		return;
	}

	const FString UniqueID = MyPC->GetPlayerUniqueID();
	if (UniqueID.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] Failed to get Unique ID from Controller."));
		return;
	}

	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		if (!GI->PlayerInfoMap.Contains(UniqueID))
		{
			UE_LOG(LogTemp, Error, TEXT("[Spawn] ID %s not found in GameInstance."), *UniqueID);
			return;
		}

		const FPlayerInfo& Info = GI->PlayerInfoMap[UniqueID];

		if (!Info.CharacterClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[Spawn] CharacterClass is null for ID %s"), *UniqueID);
			return;
		}

		TArray<AActor*> FoundPoints;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawn::StaticClass(), FoundPoints);
		UE_LOG(LogTemp, Log, TEXT("[Spawn] Found %d spawn point(s)."), FoundPoints.Num());

		bool bSpawned = false;

		for (AActor* Actor : FoundPoints)
		{
			ASpawn* Point = Cast<ASpawn>(Actor);
			if (Point && Point->GetPlayerNum() == Info.PlayerNum)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
					Info.CharacterClass,
					Point->GetActorLocation(),
					Point->GetActorRotation(),
					Params
				);

				if (NewPawn)
				{
					Player->Possess(NewPawn);
					UE_LOG(LogTemp, Log, TEXT("[Spawn] ID %s: Possessed Pawn at %s"), *UniqueID, *Point->GetName());
					bSpawned = true;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[Spawn] Failed to spawn pawn for ID %s."), *UniqueID);
				}
				break;
			}
		}

		if (!bSpawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Spawn] No spawn point found for PlayerNum %d"), Info.PlayerNum);
		}
	}
}

//plus NameTag UI Nickname 
void AGM_BattleMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		// 플레이어 고유 ID 얻기 (컨트롤러에서 제공한다고 가정)
		FString PlayerID = TEXT("Unknown");
		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(PC))
		{
			PlayerID = MyPC->GetPlayerUniqueID();
		}
		// 닉네임 정보 복원 로직 추가
		if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
		{
			if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
			{
				if (!PlayerID.IsEmpty())
				{
					if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
					{
						if (GI->PlayerInfoMap.Contains(PlayerID))
						{
							// 닉네임 유지
							FString StoredNickname = GI->PlayerInfoMap[PlayerID].Nickname;
							if (!StoredNickname.IsEmpty())
							{
								PS->SetPlayerNickName(StoredNickname);
								UE_LOG(LogTemp, Log, TEXT("[SeamlessTravel] Restored nickname: %s for player ID: %s"), *StoredNickname, *ID);
							}
						}
					}
				}
			}
		}

		// GameInstance에서 체력 정보 가져오기
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (FPlayerInfo* Info = GI->PlayerInfoMap.Find(PlayerID))
			{
				if (Info->MatchHealth <= 0)
				{
					// 체력이 0이하인 경우 → 관전자 처리
					PC->UnPossess();
					PC->ChangeState(NAME_Spectating);

					DeadPlayerControllers.Add(Cast<AMyPlayerController>(PC));
					UE_LOG(LogTemp, Warning, TEXT("[BattleMode] Player %s is eliminated and entered as spectator."), *PlayerID);
				}
				else {
					SpawnPlayerInBattle(PC); // 스폰 함수 호출

					AMyPlayerController* NewPC = Cast<AMyPlayerController>(PC);
					if (IsValid(NewPC))
					{
						AlivePlayerControllers.Add(NewPC);
					}
				}
			}
		}

		// 1초 간격으로 로비 상태 확인
		GetWorldTimerManager().SetTimer(MainTimerHandle, this, &AGM_BattleMode::OnMainTimerElapsed, 1.0f, true);
	}
}

void AGM_BattleMode::OnMainTimerElapsed()
{
	// 게임 상태 가져오기
	AGS_BattleState* GS = GetGameState<AGS_BattleState>();
	if (!IsValid(GS)) return;

	switch (GS->MatchState)
	{
	case EMatchState::Waiting:
	{
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying - DeadPlayerControllers.Num())
		{
			RemainWaitingTimeForPlaying = WaitingTime;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Waiting - Countdown"));
			--RemainWaitingTimeForPlaying;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			GS->MatchState = EMatchState::Playing;
		}

		break;
	}
	case EMatchState::Playing:
	{
		// 현재 생존 플레이어 수 업데이트
		GS->AlivePlayerControllerCount = AlivePlayerControllers.Num();

		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Playing - 생존자 수: %d"), GS->AlivePlayerControllerCount);

		// 생존자가 1명 이하이면 게임 종료 상태로
		if (GS->AlivePlayerControllerCount <= 1)
		{
			GS->MatchState = EMatchState::Ending;
			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 생존자 1명 이하 - Ending 상태로 전환"));
		}
		break;
	}
	case EMatchState::Ending:
	{
		--RemainWaitingTimeForEnding;
		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Ending - 종료까지 %d초 남음"), RemainWaitingTimeForEnding);

		if (RemainWaitingTimeForEnding <= 0)
		{
			MainTimerHandle.Invalidate();

			// 레벨 리셋 (Dedicated 서버용)
			FString LevelName = "DevLobby";
			GetWorld()->ServerTravel(LevelName + TEXT("?listen"), true);

			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 레벨 초기화 및 DevLobby로 이동"), true);
		}
		break;
	}
	default:
		break;
	}
}

void AGM_BattleMode::BeginPlay()
{
	Super::BeginPlay();

	// 1초 간격으로 로비 상태 확인
	GetWorldTimerManager().SetTimer(MainTimerHandle, this, &AGM_BattleMode::OnMainTimerElapsed, 1.0f, true);
}
