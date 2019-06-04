// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathmatchGameState.h"
#include "HeroState.h"
#include "ScoreboardEntryData.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"


void ADeathmatchGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		KillTallyObj = NewObject<UKillfeedEntryData>(this);
		OnRep_KillTallyObj();
	}
}

void ADeathmatchGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathmatchGameState, KillTallyObj);
}

bool ADeathmatchGameState::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (KillTallyObj != nullptr)
	{
		WroteSomething |= Channel->ReplicateSubobject(KillTallyObj, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

TArray<UScoreboardEntryData*> ADeathmatchGameState::GetScoreboard()
{
	TArray<UScoreboardEntryData*> Scoreboard{};

	for (APlayerState* PlayerState : PlayerArray)
	{
		const auto Hero = (AHeroState*)PlayerState;

		UScoreboardEntryData* Item = NewObject<UScoreboardEntryData>();
		Item->Name = Hero->GetPlayerName();
		Item->Kills = Hero->Kills;
		Item->Deaths = Hero->Deaths;
		Item->Ping = Hero->Ping;

		Scoreboard.Add(Item);
	}

	Scoreboard.Sort([](const UScoreboardEntryData & A, const UScoreboardEntryData & B)
		{
			if (A.Kills == B.Kills)
			{
				return A.Deaths < B.Deaths;
			}
			
			return A.Kills > B.Kills;
		});

	return std::move(Scoreboard);
}

//TArray<UKillfeedEntryData*> ADeathmatchGameState::GetKillfeed()
//{
	/*
	TArray<UKillfeedEntryData*> Killfeed{};

	for (auto* Entry : KillfeedData)
	{
		UKillfeedEntryData* Copy = NewObject<UKillfeedEntryData>();
		Copy->Winner = Entry->Winner;
		Copy->Verb = Entry->Verb;
		Copy->Loser = Entry->Loser;

		Killfeed.Add(Copy);
	}

	UE_LOG(LogTemp, Warning, TEXT("GETTING KILLFEED"));
	for (auto* e : Killfeed)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Entry %d %s %s %s"), KillfeedData.Num(),
			*e->Winner, *e->Verb, *e->Loser);
	}

	return std::move(Killfeed);*/
//}

void ADeathmatchGameState::AddKillfeedData(const FString& Victor, const FString& Verb, const FString& Dead)
{
	if (!HasAuthority()) return;
	
	LogMsgWithRole("ADeathmatchGameState::AddKillfeedData()");
	
	KillTallyObj->TotalKills++;


	/*UKillfeedEntryData* Entry = NewObject<UKillfeedEntryData>(this);
	Entry->Winner = Victor;
	Entry->Verb = "killed";
	Entry->Loser = Dead;
	LastKill = Entry;*/
	
	



//	KillfeedData.Add(Entry);

	/*
	UE_LOG(LogTemp, Warning, TEXT("KILLFEED"));
	for (auto* entry : KillfeedData)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Entry %d %s %s %s"), KillfeedData.Num(),
			*entry->Winner, *entry->Verb, *entry->Loser);
	}
	*/
}

void ADeathmatchGameState::OnRep_KillTallyObj()
{
	LogMsgWithRole("ADeathmatchGameState::OnRep_KillTallyObj()");
}

//void ADeathmatchGameState::OnRep_KillfeedDataUpdated()
//{
//	
//}



void ADeathmatchGameState::LogMsgWithRole(FString message) const
{
	FString m = GetRoleText() + ": " + message;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *m);
}
FString ADeathmatchGameState::GetEnumText(ENetRole role) const
{
	switch (role) {
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "Sim";
	case ROLE_AutonomousProxy:
		return "Auto";
	case ROLE_Authority:
		return "Auth";
	case ROLE_MAX:
		return "MAX (error?)";
	default:
		return "ERROR";
	}
}
FString ADeathmatchGameState::GetRoleText() const
{
	auto Local = Role;
	auto Remote = GetRemoteRole();


	//if (Remote == ROLE_SimulatedProxy) //&& Local == ROLE_Authority
	//	return "ListenServer";

	//if (Local == ROLE_Authority)
	//	return "Server";

	//if (Local == ROLE_AutonomousProxy) // && Remote == ROLE_Authority
	//	return "OwningClient";

	//if (Local == ROLE_SimulatedProxy) // && Remote == ROLE_Authority
	//	return "SimClient";

	return GetEnumText(Role) + " " + GetEnumText(GetRemoteRole()) + " Ded:" + (IsRunningDedicatedServer() ? "True" : "False");

}

