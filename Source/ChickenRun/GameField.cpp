// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "GameField.h"

#include "ChickenRunCharacter.h"
#include "ClickActor.h"
#include "HUDWidget.h"
#include "GeneralSettings.h"
#include "MyStaticMeshActor.h"
#include "MyLevelSequenceActor.h"

// Sets default values
AGameField::AGameField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	pName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("UTextRenderComponent0"));
	pName->SetupAttachment(DummyRoot);
	pName->SetRelativeLocation(FVector(0.f, -50.f, 0.f));
	pName->SetRelativeRotation(FRotator(90, 0, -90));
	pName->SetRelativeScale3D(FVector(2, 2, 2));
	pName->SetHorizontalAlignment(EHTA_Center);
	pName->SetText(FText::FromString("GameField"));
	pName->SetTextRenderColor(FColor(0, 100, 255));
	pName->SetHiddenInGame(true);


	// Create static mesh component
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
	UStaticMeshComponent *pBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	pBlockMesh->SetStaticMesh(PlaneMesh.Get());
	pBlockMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	pBlockMesh->SetupAttachment(DummyRoot);

	bConsoleCommandHideTiles = false;

	MapLength = 100;
	bStartMapVolumeSpawned = false;
	bFinishMapVolumeSpawned = false;
	iServiceMapLength = 0;
	iFinishVolumeNumTilesX = -1;
}

AGameField::~AGameField()
{
}

AGameField* AGameField::Clone()
{
	FActorSpawnParameters Parameters;

	Parameters.Template = this;

	AGameField* Item = GetWorld()->SpawnActor<class AGameField>(GetClass(), Parameters);

	Item->SetOwner(GetOwner());

	return Item;
}

/*FString AGameField::FindPlayerStartTile()
{
	FString sPlayerStartTileName;
	TActorIterator<APlayerStart> ActorItr = TActorIterator<APlayerStart>(GetWorld());
	if (ActorItr)
	{
		APlayerStart *pPlayerStart;
		pPlayerStart = *ActorItr;
		FHitResult HitResult;
		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, pPlayerStart->GetActorLocation() + FVector(0, 0, 1000), pPlayerStart->GetActorLocation() + FVector(0, 0, -1000), ECollisionChannel::ECC_WorldStatic);
		if (bHit)
		{
			UPrimitiveComponent *ClickedPrimitive = HitResult.Component.Get();
			AActor *pActor = ClickedPrimitive->GetOwner();
			sPlayerStartTileName = pActor->GetName();
		}
		else
		{
			UE_LOG(LogGeneral, Error, TEXT("PlayerStart tile not found"));
		}
	}
	else
	{
		UE_LOG(LogGeneral, Error, TEXT("PlayerStart actor not found"));
	}
	return sPlayerStartTileName;
}*/

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();

	//FConstPawnIterator it = GetWorld()->GetPawnIterator();
	//pPawn = Cast<AChickenRunCharacter>(it->Get());
	MyHelper::GetActorFromScene(GetWorld(), "GeneralSettings", pAGeneralSettings);

	iMapElementsWeight = 0;
	iMapElementsWeight += ChanceToAppearRiverWithLeafs;
	for (FCRMapElement &oMapElement : MapElements)
	{
		iMapElementsWeight += oMapElement.ChanceToAppear;
	}
}

// Called every frame
void AGameField::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AGameField::TickPublic(float DeltaTime)
{
	if (aDelayTiles.size())
	{
		int iCycles = aDelayTiles.size() / iFullYSize + 1;
		for (int ii = 0; ii < iCycles; ++ii)
		{
			{
				TileOfMapElement &oTileElement = aDelayTiles.front();
				InitClickActorFromPool(oTileElement);
				/*if (oTileElement.GetALevelSequenceActor() != nullptr)
				{
					ULevelSequencePlayer *pULevelSequencePlayer = GetULevelSequencePlayerFromPool(oTileElement.GetALevelSequenceActor());
					aBlocksField.back().back()->AddLevelSequencePlayer(pULevelSequencePlayer);
				}*/
				aDelayTiles.pop_front();
			}
		}
		//if (aDelayTiles.size() > (iMapVisibilityTiles - iMapDelayVisibilityTiles) * iFullYSize - iFullYSize/2)
	}
}

//static const FName NAME_ClickableTrace("ClickableTrace");
void AGameField::Restart(int32 iPreviousMapLength)
{
	if (nullptr != pAGeneralSettings->GetHUDWidget())
	{
		pAGeneralSettings->GetHUDWidget()->SetStepNum(0);
		pAGeneralSettings->GetHUDWidget()->ClearCoins();
	}

	//APlayerController *pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	{ // clear game field
		for (int32 iX = 0; iX < aBlocksField.size(); ++iX)
		{
			for (int32 iY = 0; iY < aBlocksField[iX].size(); ++iY)
			{
				if (aBlocksField[iX][iY] != nullptr)
				{
					DestroyClickActor(iX, iY);
					//bool res = aBlocksField[iX][iY]->Destroy();
					//check(res);
				}
			}
		}
		aBlocksField.clear();
	}
	aBlocksField.resize(iPreviousMapLength+1);
	iMultiMapStartX = iPreviousMapLength;
	aDelayTiles.clear();
	bFinishMapVolumeSpawned = false;
	bStartMapVolumeSpawned = false;
}

bool AGameField::RestartFirstMap(int32 &iPlayerStartXPos, int32 &iPlayerStartYPos)
{
	iPlayerStartXPos = -1;
	iPlayerStartYPos = -1;

	// spawn invisible blocks
	for (int32 iX = 0; iX < 20; iX++)
	{
		for (size_t iY = 0; iY < iFullYSize; ++iY)
		{
			InitClickActorFromPool(TileOfMapElement::CreateInvisibleBlock());
		}
	}
	iServiceMapLength = aBlocksField.size();
	// find StartMapTriggerVolume
	if (nullptr == StartVolume)
	{
		UE_LOG(LogGeneral, Error, TEXT("StartMap TriggerVolume not found"));
		return false;
	}

	iPlayerStartXPos_SpawnMapElement = -1;
	iPlayerStartYPos_SpawnMapElement = -1;
	iCurrentPawnXPos = aBlocksField.size() - 1;
	if (!SpawnMapElement(StartVolume))
	{
		return false;
	}
	bStartMapVolumeSpawned = true;
	iPlayerStartXPos = iPlayerStartXPos_SpawnMapElement;
	iPlayerStartYPos = iPlayerStartYPos_SpawnMapElement;

	if (iPlayerStartXPos == -1)
	{
		UE_LOG(LogGeneral, Error, TEXT("Can't place player to the game field"));
		return false;
	}

	PawnPositionChanged(iPlayerStartXPos, iPlayerStartYPos);
	return true;
}

void AGameField::PawnPositionChanged(int32 iXPos, int32 iYPos)
{
	iCurrentPawnXPos = iXPos;

	/*if (aBlocksField.size() > iXPos && aBlocksField[iXPos].size() > iYPos && aBlocksField[iXPos][iYPos]->IsCoin())
	{
	}*/
	int32 iAlreadyGeneratedLines = aBlocksField.size() + aDelayTiles.size() / iFullYSize;

	if (!bFinishMapVolumeSpawned)
	{
		if (IsFinishVolumeShouldBeSpawned())
		{
			check(!bFinishMapVolumeSpawned);
			SpawnMapElement(FinishVolume);
			iAlreadyGeneratedLines = aBlocksField.size() + aDelayTiles.size() / iFullYSize;
			check(GetMultimapStartX() + GetMapLength() <= iAlreadyGeneratedLines);
			bFinishMapVolumeSpawned = true;
		}
		if (aBlocksField.size() > 0 && iAlreadyGeneratedLines < iXPos + iMapVisibilityTiles)
		{
			for (int32 iX = iAlreadyGeneratedLines; iX < iXPos + iMapVisibilityTiles; iX = aBlocksField.size() + aDelayTiles.size() / iFullYSize)
			{
				if (!bStartMapVolumeSpawned)
				{
					if (StartVolume)
					{
						SpawnMapElement(StartVolume);
					}
					bStartMapVolumeSpawned = true;
					continue;
				}
				int32 iRand = MT19937::get(iMapElementsWeight);
				int32 iRandSum = 0;
				iRandSum += ChanceToAppearRiverWithLeafs;
				if (iRand < iRandSum)
				{
					SpawnRiverWithLeafs();
				}
				else
				{
					for (FCRMapElement &oMapElement : MapElements)
					{
						iRandSum += oMapElement.ChanceToAppear;
						if (iRand < iRandSum)
						{
							UE_LOG(LogGeneral, Error, TEXT("Random number %d. Total weight %d"), iRand, iMapElementsWeight);
							SpawnMapElement(oMapElement.MapElement);
							break;
						}
					}
				}
				if (iX == aBlocksField.size() + aDelayTiles.size() / iFullYSize) // nothing spawned
				{
					if (IsFinishVolumeShouldBeSpawned())
					{
						check(!bFinishMapVolumeSpawned);
						SpawnMapElement(FinishVolume);
						iAlreadyGeneratedLines = aBlocksField.size() + aDelayTiles.size() / iFullYSize;
						check(GetMultimapStartX() + GetMapLength() <= iAlreadyGeneratedLines);
						bFinishMapVolumeSpawned = true;
					}
					else
					{
						SpawnGrassElement();
					}
				}
			}
		}
	}

	const int32 iDestroyBackTiles = 8;
	if (iXPos >= iDestroyBackTiles && aBlocksField.size() > iXPos - iDestroyBackTiles && aBlocksField[iXPos - iDestroyBackTiles].size() > iMiddleY &&
		aBlocksField[iXPos - iDestroyBackTiles][iMiddleY] != nullptr &&
		aBlocksField[iXPos - iDestroyBackTiles][iMiddleY]->GetGroundType() != AClickActor::eGroundType_Block)
	{
		{
			const int32 iX = iXPos - iDestroyBackTiles;
			/*for (int32 iY = 0; iY < aBlocksField[iX].size(); ++iY)
			{
				if (iY >= iFirstSideBlockTileY && iY <= iLastSideBlockTileY)
				{
					InitClickActorFromPool(iX, iY, AClickActor::eGroundType_Block, GetRandomArrayElement(BlockRandom), GetRandomArrayElement(GroundRandom));
				}
				else
				{
					InitClickActorFromPool(iX, iY, AClickActor::eGroundType_Block, nullptr, nullptr, AClickActor::eBlockType_Invisible);
				}
			}*/
		}
		/*if (iXPos > iDestroyBackTiles)
		{
			const int32 iX = iXPos - (iDestroyBackTiles + 1);
			for (int32 iY = iFirstPlayTileY; iY <= iLastPlayTileY; ++iY)
			{
				aBlocksField[iX][iY]->Init(iX, iY, AClickActor::eGroundType_Block);
			}
		}
		if (iXPos > iDestroyBackTiles)
		{
			const int32 iX = iXPos - (iDestroyBackTiles + 1);
			for (int32 iY = iFirstSideBlockTileY; iY <= iLastSideBlockTileY; ++iY)
			{
				aBlocksField[iX][iY]->Init(iX, iY, AClickActor::eGroundType_Block);
			}
		}*/
		/*if (iXPos > iDestroyBackTiles)
		{
			const int32 iX = iXPos - (iDestroyBackTiles + 1);
			for (int32 iY = 0; iY < aBlocksField[iX].size(); ++iY)
			{
				InitClickActorFromPool(iX, iY, AClickActor::eGroundType_Block, nullptr, nullptr, AClickActor::eBlockType_Invisible);
			}
		}*/
		if (iXPos > iDestroyBackTiles)
		{
			const int32 iX = iXPos - iDestroyBackTiles;
			for (int32 iY = 0; iY < aBlocksField[iX].size(); ++iY)
			{
				DestroyClickActor(iX, iY);
			}
		}
	}
}

bool AGameField::IsConsoleCommandHideTiles() const
{
	return bConsoleCommandHideTiles;
}

void AGameField::ConsoleCommandHideTiles()
{
	bConsoleCommandHideTiles = true;
}

void AGameField::ConsoleCommandStopAnims()
{
	{ // clear game field
		for (int32 iX = 0; iX < aBlocksField.size(); ++iX)
		{
			for (int32 iY = 0; iY < aBlocksField[iX].size(); ++iY)
			{
				if (aBlocksField[iX][iY] != nullptr)
				{
					aBlocksField[iX][iY]->StopAnims();
				}
			}
		}
	}

	//for (FCRMapElement oMapElement : MapElements)
	for (auto &itMap : aLevelSequencePlayers)
	{
		for (ULevelSequencePlayer *pULevelSequencePlayer : itMap.second)
		{
			pULevelSequencePlayer->Stop();
		}
	}
	aLevelSequencePlayers.clear();
}

/*
const std::vector< tBlockVector >& AGameField::GetGameField() const
{
	return aBlocksField;
}*/

class AClickActor* AGameField::GetClickActor(int32 iXPos, int32 iYPos)
{
	if (iXPos <0 || iXPos >= aBlocksField.size())
	{
		return nullptr;
	}
	if (iYPos < 0 || iYPos >= aBlocksField[iXPos].size())
	{
		return nullptr;
	}
	return aBlocksField[iXPos][iYPos];
}

void AGameField::BurnRow(int32 iRow)
{
	for (int32 iY = 0; iY < aBlocksField[iRow].size(); ++iY)
	{
		aBlocksField[iRow][iY]->Burn();
	}
}

bool AGameField::IsClassInArray(TSubclassOf<class AActor> pActor, const TArray<TSubclassOf<class AActor>> &aActorArray)
{
	for (TArray<TSubclassOf<class AActor>>::TConstIterator Itr(aActorArray); Itr; ++Itr)
	{
		if (pActor == *Itr)
		{
			return true;
		}
	}
	return false;
}

TSubclassOf<class AActor> AGameField::GetRandomArrayElement(const TArray<TSubclassOf<class AActor>> &aActorArray)
{
	if (!aActorArray.Num())
	{
		return nullptr;
	}
	int32 iRand = rand() % aActorArray.Num();
	return aActorArray[iRand];
}

void AGameField::SpawnRiverWithLeafs()
{
	int32 iX = aBlocksField.size();
	for (int32 iY = 0; iY < 25; ++iY)
	{
		if (iY < iFirstPlayTileY || iY > iLastPlayTileY)
		{
			InitClickActorFromPoolDelayed(TileOfMapElement::CreateWater());
		}
		else
		{
			int32 iRand = rand() % 4;
			if (iRand == 0 || iY == iMiddleY)
			{
				TileOfMapElement oTileOfMapElement;
				oTileOfMapElement.AddUClass(GroundLeaf, AClickActor::eGroundType_Grass, AClickActor::eGrassType_LilyLeaf);
				InitClickActorFromPoolDelayed(oTileOfMapElement);
			}
			else
			{
				InitClickActorFromPoolDelayed(TileOfMapElement::CreateWater());
			}
		}
	}
}

void AGameField::SpawnGrassTile(int32 iY)
{
	if (iY != iMiddleY)
	{
		//AClickActor::bTmp = false;
		//aBlocksField.back().push_back(nullptr);
		//continue;
	}
	if (iY < iFirstSideBlockTileY || iY > iLastSideBlockTileY)
	{
		InitClickActorFromPoolDelayed(TileOfMapElement::CreateInvisibleBlock());
	}
	else if (iY < iFirstPlayTileY || iY > iLastPlayTileY)
	{
		InitClickActorFromPoolDelayed(TileOfMapElement::CreateBlock(GetRandomArrayElement(BlockRandom), GetRandomArrayElement(GroundRandom)));
	}
	else
	{
		int32 iRand = rand() % 100;
		int32 iRandCoin = rand() % 100;
		if (iRand < ChanceToAppearBlockOnGround && iY != iMiddleY)
		{
			InitClickActorFromPoolDelayed(TileOfMapElement::CreateBlock(GetRandomArrayElement(BlockRandom), GetRandomArrayElement(GroundRandom)));
		}
		else if (iRand < ChanceToAppearBlockOnGround + ChanceToAppearSmallBlockOnGround)
		{
			InitClickActorFromPoolDelayed(TileOfMapElement::CreateSmallBlock(GetRandomArrayElement(BlockLowRandom), GetRandomArrayElement(GroundRandom)));
		}
		else
		{
			InitClickActorFromPoolDelayed(TileOfMapElement::CreateGrass(GetRandomArrayElement(GroundRandom)));
		}
	}
}

void AGameField::SpawnGrassElement()
{
	for (int32 iY = 0; iY < 25; ++iY)
	{
		SpawnGrassTile(iY);
	}
}

bool AGameField::SpawnMapElement(ATriggerVolume *pTriggerVolume)
{
	FVector vecOrigin, vecBoxExtent;
	pTriggerVolume->GetActorBounds(false, vecOrigin, vecBoxExtent);
	int iMapNumTilesX = (int)(vecBoxExtent.X + 0.5f) * 2;
	if (vecBoxExtent.X == 0 || vecBoxExtent.X * 2 != (float)iMapNumTilesX || iMapNumTilesX % pAGeneralSettings->GetTileSize())
	{
		UE_LOG(LogGeneral, Error, TEXT("%s has wrong size"), *pTriggerVolume->GetName());
		return false;
	}
	// make vecOrigin points to the left upper corner
	vecOrigin.X += -vecBoxExtent.X + pAGeneralSettings->GetTileSize() / 2;
	vecOrigin.Y += -vecBoxExtent.Y + pAGeneralSettings->GetTileSize() / 2;
/*
	if ((int)vecOrigin.X % 100 != 0 || (int)vecOrigin.Y % 100 != 0)
	{
		UE_LOG(LogGeneral, Error, TEXT("%s has wrong position"), *pTriggerVolume->GetName());
		return false;
	}*/
	//int iVisibleTileStartXPos = aBlocksField.size();
	// Start visible map. But spawn invisible tiles first.
	std::vector<tTileOfMapElementVector> aTileMapElements;
	for (int32 iX = 0; iX < iMapNumTilesX / pAGeneralSettings->GetTileSize(); iX++)
	{
		aTileMapElements.push_back(tTileOfMapElementVector(iFullYSize));
	}

	// init start visible map
	TSet<AActor*> aOverlapActors;
	FVector vecScale = pTriggerVolume->GetActorScale3D();
	pTriggerVolume->SetActorScale3D(vecScale - FVector(0.1f, 0, 0)); // prevent overlap neighbor actors
	pTriggerVolume->UpdateOverlaps();
	pTriggerVolume->GetOverlappingActors(aOverlapActors);
	pTriggerVolume->SetActorScale3D(vecScale);

	for (TSet<AActor*>::TConstIterator Itr(aOverlapActors); Itr; ++Itr)
	{
		AActor *pActor = *Itr;
		AMyStaticMeshActor *pAMyStaticMeshActor = Cast<AMyStaticMeshActor>(pActor);
		if (pAMyStaticMeshActor && pAMyStaticMeshActor->ChanceToAppear <= rand()%100)
		{
			continue;
		}
		FVector vecActorLoc = pActor->GetActorLocation();
		int32 iXPosDelimoe = FPlatformMath::RoundToInt(vecActorLoc.X - vecOrigin.X);
		int32 iXPos = (iXPosDelimoe + pAGeneralSettings->GetTileSize()/2) / pAGeneralSettings->GetTileSize();
/*
		if (iXPos*pAGeneralSettings->GetTileSize() != iXPosDelimoe)
		{
			UE_LOG(LogGeneral, Error, TEXT("%s has wrong X position"), *pActor->GetName());
			return false;
		}*/
		int32 iYPosDelimoe = FPlatformMath::RoundToInt(vecActorLoc.Y - GameFieldCenterY);
		int32 iYPos = iMiddleY + (iYPosDelimoe + pAGeneralSettings->GetTileSize() / 2) / pAGeneralSettings->GetTileSize();
/*
		if ((iYPos - iMiddleY)*pAGeneralSettings->GetTileSize() != iYPosDelimoe)
		{
			UE_LOG(LogGeneral, Error, TEXT("%s has wrong Y position"), *pActor->GetName());
			//return false;
		}*/
		if (iXPos < 0 || iXPos >= aTileMapElements.size() || iYPos < 0 || iYPos >= aTileMapElements[iXPos].size())
		{
			// ignore elements that bigger than our trigger box
			continue;
		}

		FVector vecLocalLocation(iXPosDelimoe - iXPos*pAGeneralSettings->GetTileSize(), iYPosDelimoe - (iYPos - iMiddleY)*pAGeneralSettings->GetTileSize(), vecActorLoc.Z);
		TSubclassOf<class AActor> pActorClass = pActor->GetClass();
		if (IsClassInArray(pActorClass, GroundRandom))
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, AClickActor::eGroundType_Grass, -1, vecLocalLocation);
		}
		else if (IsClassInArray(pActorClass, BlockRandom))
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, AClickActor::eGroundType_Block, -1, vecLocalLocation);
		}
		else if (IsClassInArray(pActorClass, BlockLowRandom))
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, AClickActor::eGroundType_SmallBlock, -1, vecLocalLocation);
		}
		else if (pActorClass == Water)
		{
			aTileMapElements[iXPos][iYPos].AddActor(nullptr, AClickActor::eGroundType_Water, -1, vecLocalLocation);
		}
		else if (pActorClass == Fire)
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, AClickActor::eGroundType_Grass, AClickActor::eGrassType_Fire, vecLocalLocation);
		}
		else if (pActorClass == GroundLeaf)
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, AClickActor::eGroundType_Grass, AClickActor::eGrassType_LilyLeaf, vecLocalLocation);
		}
		else if (pActorClass->IsChildOf(ALevelSequenceActor::StaticClass()))
		{
			aTileMapElements[iXPos][iYPos].AddLevelSequenceActor(Cast<ALevelSequenceActor>(pActor));
		}
		else if (IsClassInArray(pActorClass, BackgroundElements))
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, -1, -1, vecLocalLocation);
		}
		else if (pActorClass->IsChildOf(CoinClass))
		{
			aTileMapElements[iXPos][iYPos].AddActor(pActor, -1, -1, vecLocalLocation);
		}
		else if (pActorClass == APlayerStart::StaticClass())
		{
			iPlayerStartXPos_SpawnMapElement = aBlocksField.size() + iXPos;
			iPlayerStartYPos_SpawnMapElement = iYPos;
		}
		else
		{
			continue;
		}
	}

	for (int32 iX = 0; iX < aTileMapElements.size(); ++iX)
	{
		if (FinishVolume && pTriggerVolume != FinishVolume)
		{
			if (IsFinishVolumeShouldBeSpawned())
			{
				return false;
			}
		}
		for (int32 iY = 0; iY < aTileMapElements[iX].size(); ++iY)
		{
			TileOfMapElement &oTileOfMapElement = aTileMapElements[iX][iY];
			int32 eGroundType = oTileOfMapElement.GetGroundType();
			int eGroundTypeSecond = oTileOfMapElement.GetGroundTypeSecond();
			if (eGroundType == AClickActor::eGroundType_None)
			{
				//SpawnGrassTile(iY);
				oTileOfMapElement.AddActor(nullptr, AClickActor::eGroundType_Water);
				InitClickActorFromPoolDelayed(oTileOfMapElement);
				//eGroundType = AClickActor::eGroundType_Block;
				//eGroundTypeSecond = AClickActor::eBlockType_Invisible;
			}
			else
			{
				InitClickActorFromPoolDelayed(oTileOfMapElement);
			}
		}
	}

	return true;
}


void AGameField::InitClickActorFromPoolDelayed(const TileOfMapElement &oTileOfMapElement)
{
	if (aDelayTiles.size() || aBlocksField.size() > 0 && aBlocksField.size() >= iCurrentPawnXPos + iMapDelayVisibilityTiles)
	{
		aDelayTiles.push_back(oTileOfMapElement);
	}
	else
	{
		InitClickActorFromPool(oTileOfMapElement);
		/*if (oTileOfMapElement.GetALevelSequenceActor() != nullptr)
		{
			ULevelSequencePlayer *pULevelSequencePlayer = GetULevelSequencePlayerFromPool(oTileOfMapElement.GetALevelSequenceActor());
			aBlocksField.back().back()->AddLevelSequencePlayer(pULevelSequencePlayer);
		}*/
	}
}

void AGameField::InitClickActorFromPool(const TileOfMapElement &oTileOfMapElement)
{
	int iX = 0;
	if (aBlocksField.size() > 0)
	{
		iX = aBlocksField.size() - 1;
	}
	int iY = 0;
	if (aBlocksField.size() > 0)
	{
		iY = aBlocksField.back().size();
	}
	if (iY >= iFullYSize)
	{
		iY = 0;
		iX += 1;
	}
	InitClickActorFromPool(iX, iY, oTileOfMapElement);
}

void AGameField::InitClickActorFromPool(int32 iX, int32 iY, const TileOfMapElement &oTileOfMapElement)
{
	if (iX >= aBlocksField.size())
	{
		aBlocksField.push_back(tBlockVector());
		if (iX >= aBlocksField.size())
		{
			check(false);
			return;
		}
	}
	if (iY >= aBlocksField[iX].size())
	{
		aBlocksField[iX].push_back(nullptr);
		if (iY >= aBlocksField[iX].size())
		{
			check(false);
			return;
		}
	}
	check(iX < aBlocksField.size());
	check(iY < aBlocksField[iX].size());

	if (aBlocksField[iX][iY] != nullptr)
	{
		DestroyClickActor(iX, iY);
	}
	check((aBlocksField[iX][iY] == nullptr));

	{
		// Spawn click block
		AClickActor *pNewBlock = GetWorld()->SpawnActor<AClickActor>(FVector(pAGeneralSettings->iMapStartX + 100 * iX, pAGeneralSettings->iMapStartY + 100 * iY, 0), FRotator(0, 0, 0));
		//aBlocksField.back().push_back(pNewBlock);
		pNewBlock->SetActorHiddenInGame(true);
		UStaticMeshComponent *pBlockMesh = pNewBlock->GetStaticMeshComponent();
		pBlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		pNewBlock->InitInternal(iX, iY, oTileOfMapElement);
		aBlocksField[iX][iY] = pNewBlock;
	}
}

void AGameField::DestroyClickActor(int32 iX, int32 iY)
{
	check(iX < aBlocksField.size());
	check(iY < aBlocksField[iX].size());
	if (nullptr == aBlocksField[iX][iY])
	{
		return;
	}
	/*ULevelSequencePlayer *pULevelSequencePlayer = aBlocksField[iX][iY]->GetLevelSequencePlayer();
	if (pULevelSequencePlayer != nullptr)
	{
		pULevelSequencePlayer->Stop();
	}*/
	aBlocksField[iX][iY]->Destroy();
	aBlocksField[iX][iY] = nullptr;
}
/*
ULevelSequencePlayer* AGameField::GetULevelSequencePlayerFromPool(ALevelSequenceActor *pALevelSequenceActor)
{
	ULevelSequence *pULevelSequence = pALevelSequenceActor->GetSequence();
	if (!pULevelSequence)
	{
		check(false);
		return nullptr;
	}
	AMyLevelSequenceActor *pAMyLevelSequenceActor = Cast<AMyLevelSequenceActor>(pALevelSequenceActor);

	ULevelSequencePlayer *pULevelSequencePlayer = nullptr;
	std::vector<ULevelSequencePlayer*> &aVectorOfPlayers = aLevelSequencePlayers[pULevelSequence];
	if (aVectorOfPlayers.size() != 0 && !IsAnimPoolDisabled())
	{
		pULevelSequencePlayer = aVectorOfPlayers.back();
		pULevelSequencePlayer->PlayLooping(pALevelSequenceActor->PlaybackSettings.LoopCount);
		aVectorOfPlayers.pop_back();
	}
	else
	{
		bool bReverse = false;
		FLevelSequencePlaybackSettings oPlaybackSettings = pALevelSequenceActor->PlaybackSettings;
		if (pAMyLevelSequenceActor && pAMyLevelSequenceActor->DirectionRandom)
		{
			bReverse = 0 != rand() % 2;
		}
		else
		{
			if (oPlaybackSettings.PlayRate < 0)
			{
				oPlaybackSettings.PlayRate = -oPlaybackSettings.PlayRate;
				bReverse = true;
			}
		}
		pULevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), pULevelSequence, oPlaybackSettings);
		if (bReverse)
		{
			pULevelSequencePlayer->PlayReverse();
		}
		else
		{
			pULevelSequencePlayer->Play();
		}
	}
	check(pULevelSequencePlayer != nullptr);
	
	float fDelayRandom = 0;
	if (pAMyLevelSequenceActor->StartDelayMin < pAMyLevelSequenceActor->StartDelayMax)
	{
		fDelayRandom = FMath::RandRange(pAMyLevelSequenceActor->StartDelayMin, pAMyLevelSequenceActor->StartDelayMax);
	}
	if (pALevelSequenceActor->PlaybackSettings.LoopCount == -1)
	{
		float fRand = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / pULevelSequencePlayer->GetLength())) + fDelayRandom * pULevelSequencePlayer->GetLength();
		if (fRand > pULevelSequencePlayer->GetLength())
		{
			fRand -= pULevelSequencePlayer->GetLength();
		}
		pULevelSequencePlayer->SetPlaybackPosition(fRand);
	}
	else
	{
		pULevelSequencePlayer->SetPlaybackPosition(fDelayRandom);
	}
	return pULevelSequencePlayer;
}

void AGameField::AddULevelSequencePlayerToPool(ULevelSequencePlayer *pULevelSequencePlayer)
{
	if (IsAnimPoolDisabled())
	{
		pULevelSequencePlayer->Stop();
	}
	else
	{
		pULevelSequencePlayer->Pause();
		ULevelSequence *pULevelSequence = pULevelSequencePlayer->GetLevelSequence();
		if (!pULevelSequence)
		{
			check(false);
			return;
		}

		aLevelSequencePlayers[pULevelSequence].push_back(pULevelSequencePlayer);
		aLevelSequencePlayersStore.Add(pULevelSequencePlayer);
	}
}*/

bool AGameField::IsFinishVolumeShouldBeSpawned()
{
	if (FinishVolume == nullptr)
	{
		return false;
	}

	if (iFinishVolumeNumTilesX == -1)
	{
		FVector vecOrigin2, vecBoxExtent2;
		FinishVolume->GetActorBounds(false, vecOrigin2, vecBoxExtent2);
		iFinishVolumeNumTilesX = ((int)(vecBoxExtent2.X + 0.5f) * 2) / pAGeneralSettings->GetTileSize();
	}
	int32 iAlreadyGeneratedLines = aBlocksField.size() + aDelayTiles.size() / iFullYSize;
	if (GetMultimapStartX() + GetMapLength() <= iAlreadyGeneratedLines + iFinishVolumeNumTilesX)
	{
		return true;
	}
	return false;
}


