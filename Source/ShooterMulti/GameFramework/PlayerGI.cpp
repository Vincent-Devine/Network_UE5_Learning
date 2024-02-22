#include "PlayerGI.h"
#include "ShooterPS.h"

UPlayerGI::UPlayerGI(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	// force blue team on offline mode $$$ to remove when implementing online mode
	UserInfo.TeamNum = ETeam::Blue;
}

FPlayerInfo UPlayerGI::GetUserInfo()
{
	return UserInfo;
}

void UPlayerGI::SetUserInfo(ETeam InTeamNum, const FString& InUserName)
{
	UserInfo.TeamNum = InTeamNum;
	UserInfo.UserName = InUserName;

	UE_LOG(LogTemp, Warning, TEXT("UserInfo.TeamNum: %d"), UserInfo.TeamNum);
	UE_LOG(LogTemp, Warning, TEXT("InTeamNum: %d"), InTeamNum);
}

void UPlayerGI::SetUsername(const FString& InUserName)
{
	UserInfo.UserName = InUserName;
}

void UPlayerGI::SetTeamNum(ETeam InTeamNum)
{
	UserInfo.TeamNum = InTeamNum;

	UE_LOG(LogTemp, Warning, TEXT("UserInfo.TeamNum: %d"), UserInfo.TeamNum);
	UE_LOG(LogTemp, Warning, TEXT("InTeamNum: %d"), InTeamNum);
}