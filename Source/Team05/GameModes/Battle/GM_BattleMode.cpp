//GM_BattleMode.cpp

#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
#include "GameModes/GI_BattleInstance.h"
#include "AI/BaseAIController.h"
#include "Level/Spawn.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/RandomShuffle.h"


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

	// 최대 인원 초과 시 퇴장 처리 (예: 4명 제한 흠)
	if (AlivePlayerControllers.Num() >= 4) // 또는 별도 변수 사용 가능
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer))
		{
			PC->Client_KickWithMessage(TEXT("최대 인원이 초과되어 입장이 제한됩니다."));
		}
		return;
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

	if (GetWorld()->GetNumPlayerControllers() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Logout] 모든 플레이어 로그아웃됨 - DevLobby로 이동"));
		// 레벨 리셋 (Dedicated 서버용)
		FString LevelName = "DevLobby";
		GetWorld()->ServerTravel(LevelName + TEXT("?listen"));
	}
}


void AGM_BattleMode::OnCharacterDead(AMyPlayerController* InController)
{
	if (!IsValid(InController) || !AlivePlayerControllers.Contains(InController))
		return;

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);

	// 관전자 처리
	/*InController->ChangeState(NAME_Spectating);
	InController->UnPossess();*/

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
		if (AliveCount == 1 && AliveAIControllers.Num() == 0)
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
		}
		else if (AliveCount == 0)
		{
			// 남은 플레이어 중 랭크 없는 사람에게 순위 부여
			for (auto& Elem : GI->PlayerInfoMap)
			{
				if (Elem.Value.MatchRank == 0)
				{
					int32 Rank = GI->PlayerInfoMap.Num() - GI->PlayerRanking.Num();
					Elem.Value.MatchRank = Rank;
					GI->PlayerRanking.Add(Elem.Key);

					UE_LOG(LogTemp, Warning, TEXT("[BattleMode] Player %s is now Rank %d (전원 탈락 케이스)"), *Elem.Key, Rank);
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
			//GetWorldTimerManager().ClearTimer(MainTimerHandle); // 종료 타이머 멈춤
		}

		// 일정 시간 후 Ending 상태로 전환
		FTimerDelegate EndDelegate;
		EndDelegate.BindLambda([this]()
			{
				if (AGS_BattleState* GS = GetGameState<AGS_BattleState>())
				{
					GS->MatchState = EMatchState::Ending;
				}
			});
		GetWorldTimerManager().SetTimer(EndTimerHandle, EndDelegate, 1.0f, false); // Ending 상태 진입
	}
}

void AGM_BattleMode::OnAIDead(ABaseAIController* InAIController)
{
	if (!IsValid(InAIController) || !AliveAIControllers.Contains(InAIController))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleMode][AI] Invalid or not in Alive list."));
		return;
	}

	// 리스트 갱신
	AliveAIControllers.Remove(InAIController);
	DeadAIControllers.Add(InAIController);

	// Controller 흠(Destroy할까 고민중)
	InAIController->UnPossess();

	// 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("[BattleMode][AI] AI 사망 처리 완료. Alive: %d / Dead: %d"),
		AliveAIControllers.Num(), DeadAIControllers.Num());

	// 현재 살아있는 플레이어 수 체크 (체력이 0 이상인 사람 기준)
	int32 AlivePlayerCount = 0;

	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		for (const auto& Elem : GI->PlayerInfoMap)
		{
			if (Elem.Value.MatchHealth > 0)
			{
				++AlivePlayerCount;
			}
		}

		// 플레이어가 단 1명만 살아남았다면 → 해당 플레이어를 1등 처리하고 결과 UI 출력
		if (AlivePlayerCount == 1)
		{
			// 생존자 1명 → 1등 처리
			for (auto& Elem : GI->PlayerInfoMap)
			{
				if (Elem.Value.MatchHealth > 0 && Elem.Value.MatchRank == 0)
				{
					Elem.Value.MatchRank = 1;
					GI->PlayerRanking.Add(Elem.Key);
					UE_LOG(LogTemp, Warning, TEXT("[BattleMode][AI] 최종 생존자 %s → Rank 1"), *Elem.Key);
					break;
				}
			}
		}

		// 정렬 및 결과 전송 (공통)
		if (AlivePlayerCount <= 1)
		{
			TArray<FPlayerInfo> SortedInfos;
			GI->PlayerInfoMap.GenerateValueArray(SortedInfos);
			SortedInfos.Sort([](const FPlayerInfo& A, const FPlayerInfo& B) {
				return A.MatchRank < B.MatchRank;
				});

			TArray<FPlayerRankingInfo> FinalRankingList;
			for (const FPlayerInfo& Info : SortedInfos)
			{
				FPlayerRankingInfo RankInfo;
				RankInfo.Nickname = Info.Nickname;
				RankInfo.Rank = Info.MatchRank;
				FinalRankingList.Add(RankInfo);
			}

			for (AMyPlayerController* PC : AlivePlayerControllers)
			{
				if (IsValid(PC))
					PC->Client_ReceiveRankingInfo(FinalRankingList);
			}
			for (AMyPlayerController* PC : DeadPlayerControllers)
			{
				if (IsValid(PC))
					PC->Client_ReceiveRankingInfo(FinalRankingList);
			}

			GetWorldTimerManager().ClearTimer(MainTimerHandle); // 종료 타이머 멈춤

			// 일정 시간 후 Ending 상태로 전환
			FTimerDelegate EndDelegate;
			EndDelegate.BindLambda([this]()
				{
					if (AGS_BattleState* GS = GetGameState<AGS_BattleState>())
					{
						GS->MatchState = EMatchState::Ending;
					}
				});
			GetWorldTimerManager().SetTimer(EndTimerHandle, EndDelegate, 1.0f, false); // Ending 상태 진입
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

		// 스폰 포인트 가져오기
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
								UE_LOG(LogTemp, Log, TEXT("[SeamlessTravel] Restored nickname: %s for player ID: %s"), *StoredNickname, *PlayerID);
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
						UE_LOG(LogTemp, Warning, TEXT("[BattleMode] NewPlayerAdd"));
						AlivePlayerControllers.Add(NewPC);
					}
				}
			}
		}

		// 싱글모드면 다른 타이머와 추가 함수 작동
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (GI->bSingleMode)
			{
				// 1초 간격으로 로비 상태 확인
				GetWorldTimerManager().SetTimer(SingleTimerHandle, this, &AGM_BattleMode::OnSingleTimerElapsed, 1.0f, true);
			}
			else {
				// 1초 간격으로 로비 상태 확인
				GetWorldTimerManager().SetTimer(MainTimerHandle, this, &AGM_BattleMode::OnMainTimerElapsed, 1.0f, true);
			}
		}
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

		// 입력 막기 (플레이어만)
		for (AMyPlayerController* PC : AlivePlayerControllers)
		{
			if (IsValid(PC))
			{
				PC->Client_SetInputEnabled(false);
			}
		}

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
			// AI 스폰 함수
			SpawnRandomAIs();
			GS->MatchState = EMatchState::Playing;
			// 입력 허용 (플레이어만)
			for (AMyPlayerController* PC : AlivePlayerControllers)
			{
				if (IsValid(PC))
				{
					PC->Client_SetInputEnabled(true);
				}
			}
		}

		break;
	}
	case EMatchState::Playing:
	{
		// 현재 생존 플레이어, AI 수 업데이트
		GS->AlivePlayerControllerCount = AlivePlayerControllers.Num() + AliveAIControllers.Num();

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
			// 모든 플레이어를 타이틀로 복귀
			for (auto PC : AlivePlayerControllers) PC->ReturnToTitle();
			for (auto PC : DeadPlayerControllers) PC->ReturnToTitle();

			MainTimerHandle.Invalidate();

			// 레벨 리셋 (Dedicated 서버용)
			FString LevelName = "DevLobby";
			GetWorld()->ServerTravel(LevelName + TEXT("?listen"), false);

			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 레벨 초기화 및 DevLobby로 이동"));
		}
		break;
	}
	default:
		break;
	}
}

void AGM_BattleMode::OnSingleTimerElapsed()
{
	// 게임 상태 가져오기
	AGS_BattleState* GS = GetGameState<AGS_BattleState>();
	if (!IsValid(GS)) return;

	switch (GS->MatchState)
	{
	case EMatchState::Waiting:
	{
		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Waiting - Countdown"));
		--RemainWaitingTimeForPlaying;

		// 입력 막기 (플레이어만)
		for (AMyPlayerController* PC : AlivePlayerControllers)
		{
			if (IsValid(PC))
			{
				PC->Client_SetInputEnabled(false);
			}
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			// AI 스폰 함수
			SpawnRandomAIs();
			GS->MatchState = EMatchState::Playing;
			// 입력 허용 (플레이어만)
			for (AMyPlayerController* PC : AlivePlayerControllers)
			{
				if (IsValid(PC))
				{
					PC->Client_SetInputEnabled(true);
				}
			}
		}

		break;
	}
	case EMatchState::Playing:
	{
		// 현재 생존 플레이어, AI 수 업데이트
		GS->AlivePlayerControllerCount = AlivePlayerControllers.Num() + AliveAIControllers.Num();

		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Playing - 생존자 수: %d / Playing - AI 수 : %d"), AlivePlayerControllers.Num(), AliveAIControllers.Num())

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
			for (auto PC : AlivePlayerControllers) PC->ReturnToTitle();
			for (auto PC : DeadPlayerControllers) PC->ReturnToTitle();

			MainTimerHandle.Invalidate();

			// 레벨 리셋
			FString LevelName = "DevMenu";
			GetWorld()->ServerTravel(LevelName + TEXT("?listen"), false);

			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 레벨 초기화 및 DevLobby로 이동"), true);
		}
		break;
	}
	default:
		break;
	}
}

void AGM_BattleMode::SpawnRandomAIs()
{
	if (!HasAuthority()) return;

	const int32 MaxPlayerCount = 4;
	int32 CurrentPlayerCount = AlivePlayerControllers.Num() + DeadPlayerControllers.Num(); // 접속한 플레이어 수
	int32 NumAIsToSpawn = MaxPlayerCount - CurrentPlayerCount;

	if (NumAIsToSpawn <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AI Spawn] No AI needed. Players full (%d/%d)."), CurrentPlayerCount, MaxPlayerCount);
		return;
	}

	TArray<TSubclassOf<APawn>> AIPawnClasses = {
		BP_KnightAIClass,
		BP_MageAIClass,
		BP_BarbarianAIClass,
		BP_RogueAIClass
	};

	// 랜덤으로 섞기
	Algo::RandomShuffle(AIPawnClasses);

	// 스폰 포인트 가져오기
	TArray<AActor*> FoundPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawn::StaticClass(), FoundPoints);

	TArray<ASpawn*> AISpawnPoints;
	int32 CurrentAlivePlayerCount = AlivePlayerControllers.Num(); // 현재 생존한 플레이어 수

	for (AActor* Actor : FoundPoints)
	{
		if (ASpawn* Point = Cast<ASpawn>(Actor))
		{
			// 플레이어 자리 다음 번호부터 AI 자리
			if (Point->GetPlayerNum() > CurrentAlivePlayerCount)
			{
				AISpawnPoints.Add(Point);
			}
		}
	}

	// AI 스폰
	for (int32 i = 0; i < NumAIsToSpawn && i < AISpawnPoints.Num() && i < AIPawnClasses.Num(); ++i)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASpawn* Point = AISpawnPoints[i];
		TSubclassOf<APawn> AIClass = AIPawnClasses[i];

		APawn* NewAIPawn = GetWorld()->SpawnActor<APawn>(
			AIClass,
			Point->GetActorLocation(),
			Point->GetActorRotation(),
			Params
		);

		if (NewAIPawn)
		{
			// AIController 생성 → BP_AIController 사용
			if (AIControllerClass)
			{
				ABaseAIController* AIController = GetWorld()->SpawnActor<ABaseAIController>(AIControllerClass);
				if (AIController)
				{
					AIController->Possess(NewAIPawn);
					AliveAIControllers.Add(AIController);
					UE_LOG(LogTemp, Log, TEXT("[AI Spawn] AI %d spawned at %s"), i + 1, *Point->GetName());
				}
			}
		}
	}
}

void AGM_BattleMode::BeginPlay()
{
	Super::BeginPlay();

}
