// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskHUD.h"
#include "Engine/AssetManager.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "CTF_GameState.h"
#include "CTF_PlayerState.h"

ATaskHUD::ATaskHUD()
{
}


void ATaskHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!CrosshairTexture.ToString().IsEmpty())
	{
		CrosshairTex = CrosshairTexture.LoadSynchronous();
	}

	if (ScreenWidgetClass != NULL)
	{
		ScreenWidget = CreateWidget<UScreen_Widget>(GetWorld(), ScreenWidgetClass);

		if (ScreenWidget != nullptr)
		{
			ScreenWidget->AddToViewport();
		}
	}
}

void ATaskHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATaskHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GameState == nullptr)
	{
		//Setup one time only
		GameState = (ACTF_GameState*)GetWorld()->GetGameState();
		if(GameState != nullptr)
		{
			GameState->OnTimeChangedEvent.AddUObject(this, &ATaskHUD::OnTimeChanged);
			GameState->OnScoreAChangedEvent.AddUObject(this, &ATaskHUD::OnScoreAChanged);
			GameState->OnScoreBChangedEvent.AddUObject(this, &ATaskHUD::OnScoreBChanged);
		}
	}


	if (CrosshairTex && GameState)
	{
		if(GameState->Time != 0)
		{
			if(bShowCursor)
			{
				// Draw a simple crosshair

				// find center of the Canvas
				const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

				// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
				const FVector2D CrosshairDrawPosition((Center.X), (Center.Y + 20.0f));

				// draw the crosshair
				FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
				TileItem.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(TileItem);
			}
		}
	}
}

void ATaskHUD::ShowScreenUI(bool IsTrue)
{
	if (ScreenWidget != nullptr)
	{
		ScreenWidget->ShowUI(IsTrue);
	}
}

void ATaskHUD::ShowScreenFlag(bool IsTrue)
{
	if (ScreenWidget != nullptr)
	{
		ScreenWidget->ShowFlag(IsTrue);
	}
}

void ATaskHUD::ShowCursor(bool IsTrue)
{
	bShowCursor = IsTrue;
}

void ATaskHUD::UpdateHealth(float Health)
{
	if (ScreenWidget != nullptr)
	{
		ScreenWidget->UpdateHealth(Health);
	}
}

void ATaskHUD::OnTimeChanged()
{
	if(ScreenWidget != nullptr)
	{
		if(GameState != nullptr)
		{
			ScreenWidget->ChangeTime(GameState->Time);

			if(GameState->Time == 0)
			{
				ACTF_PlayerState* PS = (ACTF_PlayerState* )GetOwningPlayerController()->PlayerState;
				if(PS != nullptr)
				{
					if(GameState->TeamAScore > GameState->TeamBScore)
					{
						//Team A Won
						if(PS->IsTeamA)
						{
							ScreenWidget->EnableEndScreen(FText::FromString("You won!"));
						}
						else
						{
							ScreenWidget->EnableEndScreen(FText::FromString("You lost!"));
						}
					}
					else if(GameState->TeamAScore < GameState->TeamBScore)
					{
						//Team B Won
						if(PS->IsTeamA)
						{
							ScreenWidget->EnableEndScreen(FText::FromString("You lost!"));
						}
						else
						{
							ScreenWidget->EnableEndScreen(FText::FromString("You won!"));
						}
					}
					else
					{
						//Its a tie
						ScreenWidget->EnableEndScreen(FText::FromString("Tie!"));
					}
				}
			}
		}
	}
}

void ATaskHUD::OnScoreAChanged()
{
	if(ScreenWidget != nullptr)
	{
		if(GameState != nullptr)
		{
			ScreenWidget->ChangeScoreA(GameState->TeamAScore);
		}
	}
}

void ATaskHUD::OnScoreBChanged()
{
	if(ScreenWidget != nullptr)
	{
		if(GameState != nullptr)
		{
			ScreenWidget->ChangeScoreB(GameState->TeamBScore);
		}
	}
}

