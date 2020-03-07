// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GeneralSettings.generated.h"

USTRUCT()
struct FGameFieldWithParams
{
	GENERATED_BODY()
	FGameFieldWithParams()
	{
		GameField = nullptr;
		Length = 0;
	}
	UPROPERTY(EditAnywhere, Category = "GeneralSettings")
	class AGameField *GameField;
	/** Map length. 0 - default */
	UPROPERTY(EditAnywhere, Category = "Blueprints", meta = (UIMin = "0"))
	int32 Length;
};

UCLASS()
class CHICKENRUN_API AGeneralSettings : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGeneralSettings();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	class AClickActor* GetClickActor(int32 iXPos, int32 iYPos);
	class AGameField* GetGameFieldForRow(int32 iRow);
	bool GetTilePosition(FVector vecLocation, int32 &iOutPosX, int32 &iOutPosY);
	class UHUDWidget* GetHUDWidget() { return pHUDWidget; }
	TSubclassOf<class AActor> GetLogClass() const;
	TSubclassOf<class AActor> GetCarClass() const;
	TSubclassOf<class AActor> GetCoinClass() const;
	bool IsSimpleControl() const;
	int32 GetGameFieldZ() const { return GameFieldZ; }
	bool Restart(int32 &iPlayerStartXPos, int32 &iPlayerStartYPos); // return true - success
	void PawnPositionChanged(int32 iXPos, int32 iYPos);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralSettings")
	class AGameField *GameFieldSingle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralSettings")
	int32 DragStep;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralSettings")
	float BurnTime;

	/** 0 - don't destroy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GeneralSettings", meta = (UIMin = "0", UIMax = "100"))
	float RowDestroyTime;

	UPROPERTY(EditAnywhere, Category = "GeneralSettings")
	TArray<FGameFieldWithParams> GameFieldSequence;

	int32 GetTileSize() const { return TileSize; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameField)
	float BackgroundSpeed1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameField)
	float BackgroundSpeed2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameField)
	float BackgroundSpeed3;



	const int32 iMapStartX = 5000;
	const int32 iMapStartY = -1200;
protected:

	UPROPERTY(Category = GameField, VisibleAnywhere, BlueprintReadOnly)
	int32 TileSize = 100;
	UPROPERTY(Category = GameField, VisibleAnywhere, BlueprintReadOnly)
	int32 GameFieldZ = 0;
	/** Dummy root component */
	UPROPERTY(Category = ClickActor, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;
	
	TSubclassOf<class UHUDWidget> pHUDWidgetClass; // blueprint class
	class UHUDWidget *pHUDWidget;

private:
	TArray<class AGameField*> aGameFieldSequenceClone;
	AActor *pMaineCameraActor;
	FVector vecMaineCameraPos;
	float vecMaineCameraPosY;
	UMaterialInstanceDynamic *pBackgroundMaterial;
	class AGameField *GameFieldActive;

	int32 iPlayerStartXPosition; // for hud widget
	int32 iPlayerSteps; // for hud widget

	int32 iBurnedRow;
	float fBurnedRowTime;
};
