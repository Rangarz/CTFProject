// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CTF_GameInstanceOnline.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnCreateSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnStartSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnDestroySessionComplete, bool, Successful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCSOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FCSOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);


/**
 * 
 */
UCLASS()
class CTFTASK_API UCTF_GameInstanceOnline : public UGameInstance
{
	GENERATED_BODY()

public:

	UCTF_GameInstanceOnline();

	//Create Session
	void CreateSession(int32 NumPublicConnections, bool IsLANMatch);

	FCSOnCreateSessionComplete OnCreateSessionCompleteEvent;


	//Start the created Session
	void StartSession();

	FCSOnStartSessionComplete OnStartSessionCompleteEvent;

	//Destroy Session
	void DestroySession();

	FCSOnDestroySessionComplete OnDestroySessionCompleteEvent;

	//Find Sessions
	void FindSessions(int32 MaxSearchResults, bool IsLANQuery);

	FCSOnFindSessionsComplete OnFindSessionsCompleteEvent;


	//Join Session
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	FCSOnJoinSessionComplete OnJoinGameSessionCompleteEvent;



protected:
	void OnCreateSessionCompleted(FName SessionName, bool Successful);
	void OnStartSessionCompleted(FName SessionName, bool Successful);
	void OnDestroySessionCompleted(FName SessionName, bool Successful);
	void OnFindSessionsCompleted(bool Successful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	bool TryTravelToCurrentSession();

private:
//Create
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

//Start
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

//Destroy
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

//Find
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

//Join
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
};
