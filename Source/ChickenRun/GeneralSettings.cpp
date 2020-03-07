// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "GeneralSettings.h"
#include "GameField.h"
#include "HUDWidget.h"

// Sets default values
AGeneralSettings::AGeneralSettings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
	UStaticMeshComponent *pBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	pBlockMesh->SetStaticMesh(PlaneMesh.Get());
	pBlockMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	pBlockMesh->SetupAttachment(DummyRoot);

	UTextRenderComponent *pName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("UTextRenderComponent0"));
	pName->SetupAttachment(DummyRoot);
	pName->SetRelativeLocation(FVector(0.f, -50.f, 0.f));
	pName->SetRelativeRotation(FRotator(90, 0, -90));
	pName->SetRelativeScale3D(FVector(2, 2, 2));
	pName->SetHorizontalAlignment(EHTA_Center);
	pName->SetText(FText::FromString("GeneralSettings"));
	pName->SetTextRenderColor(FColor(255, 100, 0));
	pName->SetHiddenInGame(true);

	GameFieldSingle = nullptr;
	GameFieldActive = nullptr;

	pBackgroundMaterial = nullptr;
	pMaineCameraActor = nullptr;

	pHUDWidget = nullptr;
	pHUDWidgetClass = nullptr;
	{
		static ConstructorHelpers::FObjectFinder<UClass> ItemBlueprint(TEXT("WidgetBlueprint'/Game/TopDownCPP/Blueprints/BP_HUDWidget.BP_HUDWidget_C'"));
		if (ItemBlueprint.Object)
		{
			pHUDWidgetClass = ItemBlueprint.Object;
		}
	}

	BackgroundSpeed1 = 10;
	BackgroundSpeed2 = 3;
	BackgroundSpeed3 = 1;

	RowDestroyTime = 1;
}

// Called when the game starts or when spawned
void AGeneralSettings::BeginPlay()
{
	Super::BeginPlay();

	GameFieldActive = nullptr;
	pHUDWidget = nullptr;
	if (pHUDWidgetClass)
	{
		pHUDWidget = CreateWidget<UHUDWidget>(GetWorld(), pHUDWidgetClass);
		pHUDWidget->AddToViewport();
		pHUDWidget->pHUDWidget = pHUDWidget;
	}

	APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PController)
	{
		PController->ConsoleCommand(TEXT("stat fps"), true);
		PController->ConsoleCommand(TEXT("stat unit"), true);
		PController->ConsoleCommand(TEXT("t.MaxFPS 150"), true);
	}

	{
		if (MyHelper::GetActorFromScene(GetWorld(), "MaineCamera_Blueprint", pMaineCameraActor))
		{
			UStaticMeshComponent *pMesh = Cast<UStaticMeshComponent>(pMaineCameraActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (pMesh)
			{
				pBackgroundMaterial = pMesh->CreateDynamicMaterialInstance(0);
				pMesh->SetMaterial(0, pBackgroundMaterial);
				vecMaineCameraPos = pMaineCameraActor->GetActorLocation();
				vecMaineCameraPosY = vecMaineCameraPos.Y;
			}
		}
	}

	check(aGameFieldSequenceClone.Num() == 0);
	for (TArray<FGameFieldWithParams>::TIterator Itr(GameFieldSequence); Itr; ++Itr)
	{
		if ((*Itr).GameField == nullptr)
		{
			UE_LOG(LogGeneral, Error, TEXT("GameField in General Settings shouldn't be nullptr"));
			return;
		}
		AGameField *pGameField = (*Itr).GameField->Clone();
		if ((*Itr).Length != 0)
		{
			pGameField->SetMapLength((*Itr).Length);
		}
		aGameFieldSequenceClone.Push(pGameField);
	}
}

void AGeneralSettings::BeginDestroy()
{
	Super::BeginDestroy();
	for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
	{
		AGameField *pGameField = *Itr;
		pGameField->Destroy();
	}
}

// Called every frame
void AGeneralSettings::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	/*if (ActiveGameField)
	{
		ActiveGameField->TickPublic(DeltaTime);
	}*/

	if (GameFieldSingle)
	{
		GameFieldSingle->TickPublic(DeltaTime);
	}
	else
	{
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pGameField->TickPublic(DeltaTime);
		}
	}
	 

	if (pBackgroundMaterial != nullptr)
	{
		FVector vecNewPos = pMaineCameraActor->GetActorLocation();
		vecNewPos.Y = vecMaineCameraPosY;
		pMaineCameraActor->SetActorLocation(vecNewPos);
		if (vecNewPos != vecMaineCameraPos)
		{
			float fXDif = vecNewPos.X - vecMaineCameraPos.X;
			if (fXDif != 0)
			{
				pBackgroundMaterial->SetScalarParameterValue("Tex1ParamX", vecNewPos.X*0.0001f*BackgroundSpeed1);
				pBackgroundMaterial->SetScalarParameterValue("Tex2ParamX", vecNewPos.X*0.0001f*BackgroundSpeed2);
				pBackgroundMaterial->SetScalarParameterValue("Tex3ParamX", vecNewPos.X*0.0001f*BackgroundSpeed3);
			}
			float fYDif = vecNewPos.Y - vecMaineCameraPos.Y;
			if (fYDif != 0)
			{
				pBackgroundMaterial->SetScalarParameterValue("Tex1ParamY", vecNewPos.Y*0.0001f*BackgroundSpeed1);
				pBackgroundMaterial->SetScalarParameterValue("Tex2ParamY", vecNewPos.Y*0.0001f*BackgroundSpeed2);
				pBackgroundMaterial->SetScalarParameterValue("Tex3ParamY", vecNewPos.Y*0.0001f*BackgroundSpeed3);
			}
			vecMaineCameraPos = vecNewPos;
		}
	}

	if (RowDestroyTime > 0)
	{
		fBurnedRowTime += DeltaTime;
		if (fBurnedRowTime > RowDestroyTime)
		{
			fBurnedRowTime = 0;
			AGameField *pAGameField = GetGameFieldForRow(iBurnedRow);
			if (nullptr != pAGameField)
			{
				pAGameField->BurnRow(iBurnedRow);
			}
			++iBurnedRow;
		}
	}
}

class AClickActor* AGeneralSettings::GetClickActor(int32 iXPos, int32 iYPos)
{
	AClickActor *pAClickActor = nullptr;
	if (GameFieldSingle)
	{
		pAClickActor = GameFieldSingle->GetClickActor(iXPos, iYPos);
	}
	else
	{
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pAClickActor = pGameField->GetClickActor(iXPos, iYPos);
			if (pAClickActor != nullptr)
			{
				break;
			}
		}
	}
	
	return pAClickActor;
}

class AGameField* AGeneralSettings::GetGameFieldForRow(int32 iRow)
{
	AClickActor *pAClickActor = nullptr;
	if (GameFieldSingle)
	{
		pAClickActor = GameFieldSingle->GetClickActor(iRow, 0);
		if (pAClickActor != nullptr)
		{
			return GameFieldSingle;
		}
	}
	else
	{
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pAClickActor = pGameField->GetClickActor(iRow, 0);
			if (pAClickActor != nullptr)
			{
				return pGameField;
			}
		}
	}
	return nullptr;
}

bool AGeneralSettings::GetTilePosition(FVector vecLocation, int32 &iOutPosX, int32 &iOutPosY)
{
	float fLocX = vecLocation.X - iMapStartX;
	float fLocY = vecLocation.Y - iMapStartY;
	iOutPosX = (fLocX + TileSize / 2) / TileSize;
	iOutPosY = (fLocY + TileSize / 2) / TileSize;
	if (nullptr == GetClickActor(iOutPosX, iOutPosY))
	{
		UE_LOG(LogGeneral, Error, TEXT("GetTilePosition wrong vecLocation"));
		return false;
	}
	return true;
}

TSubclassOf<class AActor> AGeneralSettings::GetLogClass() const
{
	return GameFieldActive->Log;
}

TSubclassOf<class AActor> AGeneralSettings::GetCarClass() const
{
	return GameFieldActive->CarClass;
}

TSubclassOf<class AActor> AGeneralSettings::GetCoinClass() const
{
	return GameFieldActive->CoinClass;
}

bool AGeneralSettings::IsSimpleControl() const
{
	return GameFieldActive->IsSimpleControl();
}

bool AGeneralSettings::Restart(int32 &iPlayerStartXPos, int32 &iPlayerStartYPos)
{
	bool bRet = false;

	if (GameFieldSingle)
	{
		GameFieldActive = GameFieldSingle;
		GameFieldSingle->Restart(0);
		bRet = GameFieldSingle->RestartFirstMap(iPlayerStartXPos, iPlayerStartYPos);
	}
	else
	{
		int32 iMapLength = 0;
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pGameField->Restart(iMapLength);
			if (iMapLength == 0)
			{
				GameFieldActive = pGameField;
				bRet = aGameFieldSequenceClone[0]->RestartFirstMap(iPlayerStartXPos, iPlayerStartYPos);
			}
			iMapLength += pGameField->GetMapLength();
		}
	}
	iPlayerStartXPosition = iPlayerStartXPos;
	iPlayerSteps = 0;

	iBurnedRow = 0;
	fBurnedRowTime = 0;
	return bRet;
}

void AGeneralSettings::PawnPositionChanged(int32 iXPos, int32 iYPos)
{
	iPlayerSteps = FMath::Max(iPlayerSteps, iXPos - iPlayerStartXPosition);
	GetHUDWidget()->SetStepNum(iPlayerSteps);
	if (GameFieldSingle)
	{
		GameFieldSingle->PawnPositionChanged(iXPos, iYPos);
	}
	else
	{
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pGameField->PawnPositionChanged(iXPos, iYPos);
		}
		//ActiveGameField->PawnPositionChanged(iXPos, iYPos);
		AClickActor *pAClickActor = nullptr;
		for (TArray<class AGameField*>::TIterator Itr(aGameFieldSequenceClone); Itr; ++Itr)
		{
			AGameField *pGameField = *Itr;
			pAClickActor = pGameField->GetClickActor(iXPos, iYPos);
			if (pAClickActor != nullptr)
			{
				GameFieldActive = pGameField;
				break;
			}
		}
	}
}
