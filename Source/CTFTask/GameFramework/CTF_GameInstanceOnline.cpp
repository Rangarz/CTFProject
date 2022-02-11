// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_GameInstanceOnline.h"
#include "OnlineSubsystemUtils.h"

UCTF_GameInstanceOnline::UCTF_GameInstanceOnline() : 
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionCompleted)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted))
{
	
}


void UCTF_GameInstanceOnline::CreateSession(int32 NumPublicConnections, bool IsLANMatch)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(false);
		return;
	}

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = IsLANMatch;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set(SETTING_MAPNAME, FString("Your Level Name"), EOnlineDataAdvertisementType::ViaOnlineService);

	CreateSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		OnCreateSessionCompleteEvent.Broadcast(false);
	}
}

void UCTF_GameInstanceOnline::OnCreateSessionCompleted(FName SessionName, bool Successful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreateSessionCompleteEvent.Broadcast(Successful);
}



void UCTF_GameInstanceOnline::StartSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnStartSessionCompleteEvent.Broadcast(false);
		return;
	}

	StartSessionCompleteDelegateHandle =
		sessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!sessionInterface->StartSession(NAME_GameSession))
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);

		OnStartSessionCompleteEvent.Broadcast(false);
	}
}

void UCTF_GameInstanceOnline::OnStartSessionCompleted(FName SessionName, bool Successful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	OnStartSessionCompleteEvent.Broadcast(Successful);
}


void UCTF_GameInstanceOnline::DestroySession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionCompleteEvent.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle =
		sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

		OnDestroySessionCompleteEvent.Broadcast(false);
	}
}

void UCTF_GameInstanceOnline::OnDestroySessionCompleted(FName SessionName, bool Successful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	OnDestroySessionCompleteEvent.Broadcast(Successful);
}

void UCTF_GameInstanceOnline::FindSessions(int32 MaxSearchResults, bool IsLANQuery)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle =
		sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IsLANQuery;

	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UCTF_GameInstanceOnline::OnFindSessionsCompleted(bool Successful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), Successful);
		return;
	}

	OnFindSessionsCompleteEvent.Broadcast(LastSessionSearch->SearchResults, Successful);
}

void UCTF_GameInstanceOnline::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle =
		sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UCTF_GameInstanceOnline::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnJoinGameSessionCompleteEvent.Broadcast(Result);
}

bool UCTF_GameInstanceOnline::TryTravelToCurrentSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		return false;
	}

	FString connectString;
	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		return false;
	}

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->ClientTravel(connectString, TRAVEL_Absolute);
	return true;
}
