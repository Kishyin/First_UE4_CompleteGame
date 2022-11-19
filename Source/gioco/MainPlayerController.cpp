// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HUDOverlayAsset)
	{
		//We load the HUD created in the editor in the c++ variable HUDOverlay
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthbar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthbar)
		{

			EnemyHealthbar->AddToViewport();
			EnemyHealthbar->SetVisibility(ESlateVisibility::Hidden);

		}
		FVector2D Allignment(0.f, 0.f);
		EnemyHealthbar->SetAlignmentInViewport(Allignment);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{

			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);

		}
	}


	
}


void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthbar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthbar->SetVisibility(ESlateVisibility::Visible);

	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthbar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthbar->SetVisibility(ESlateVisibility::Hidden);

	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthbar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		/*If you want to change dimension*/
		//PositionInViewport.Y -= 50.f;
		//FVector2D SizeInViewport(300.f, 25.f);

		FVector2D SizeInViewport(200.f, 25.f);

		EnemyHealthbar->SetPositionInViewport(PositionInViewport);
		EnemyHealthbar->SetDesiredSizeInViewport(SizeInViewport);
	}


}


void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeAndUI;

		SetInputMode(InputModeAndUI);

		bShowMouseCursor = true;

	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}


void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}