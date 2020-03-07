// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "ClickActor.h"
#include "ChickenRunGameMode.h"
#include "ACMGeneral.h"
#include "ACMCharacter.h"
#include "GameField.h"
#include "ChickenRunTile.h"
#include "MyLevelSequenceActor.h"
#include "MyStaticMeshActor.h"

AGameField *AClickActor::pGameField = nullptr;
AClickActor::AClickActor() : AStaticMeshActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> pCube(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	if (pCube.Object != nullptr)
	{
		// Create dummy root scene component
		DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
		RootComponent = DummyRoot;

		UStaticMeshComponent *pBlockMesh = GetStaticMeshComponent();
		pBlockMesh->SetStaticMesh(pCube.Object);
		pBlockMesh->OnClicked.AddDynamic(this, &AClickActor::BlockClicked);
		pBlockMesh->OnReleased.AddDynamic(this, &AClickActor::BlockReleased);
		pBlockMesh->OnInputTouchBegin.AddDynamic(this, &AClickActor::OnFingerPressedBlock);
		pBlockMesh->OnInputTouchEnd.AddDynamic(this, &AClickActor::OnFingerReleasedBlock);
		pBlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		pBlockMesh->SetRelativeScale3D(FVector(1, 1, 0.01f));
		pBlockMesh->SetupAttachment(DummyRoot);
		pBlockMesh->SetCastShadow(false);

	}

	SetMobility(EComponentMobility::Movable);
	eGroundType = eGroundType_None;
}

AClickActor::~AClickActor()
{
	//DeleteAll();
}

void AClickActor::DeleteAll()
{
	for (AActor *pVisibleActor : aVisibleActors)
	{
		pVisibleActor->Destroy();
	}
	aVisibleActors.clear();

	StopAnims();
	aLevelSequencePlayersStore.Empty();

	for (AActor *pSpawnedActor : aSpawnedActors)
	{
		pSpawnedActor->Destroy();
	}
	aSpawnedActors.clear();

	/*if (aSpawnedActors.size() > 0 && pLevelSequencePlayerStore->IsValidLowLevel())
	{
		aSpawnedActors.clear();
		pLevelSequencePlayerStore = nullptr;
		GetWorld()->ForceGarbageCollection(true);
	}*/
}


void AClickActor::InitInternal(int32 iX, int32 iY, const TileOfMapElement &oTileOfMapElement)
{
	check(pGameField != nullptr);
	if (eGroundType != eGroundType_None)
	{
		check(false);
		return;
	}

	iXPos = iX;
	iYPos = iY;
	eGroundType = (GroundType)oTileOfMapElement.GetGroundType();
	eGroundTypeSecond = oTileOfMapElement.GetGroundTypeSecond();
	
//	FConstPawnIterator itPawn = GetWorld()->GetPawnIterator();
//	AChickenRunCharacter *pPawn = Cast<AChickenRunCharacter>(itPawn->Get());
	{
		UStaticMeshComponent *pBlockMesh = GetStaticMeshComponent();
		if (pBlockMesh)
		{
			if (eGroundType_Water != eGroundType)
			{
				pBlockMesh->SetCollisionObjectType(ECC_GameTraceChannel2);
			}
			else
			{
				pBlockMesh->SetCollisionObjectType(ECC_WorldStatic);
			}
		}
	}

	const std::vector<ActorClassAndPosition> &aActorClasses = oTileOfMapElement.GetActorClasses();
	for (const ActorClassAndPosition &oActorClassAndPosition : aActorClasses)
	{
		if (!pGameField->IsConsoleCommandHideTiles())
		{
			AActor *pActor = GetWorld()->SpawnActor<AActor>(oActorClassAndPosition.pActorClass, GetActorLocation() + oActorClassAndPosition.vecActorLocation, oActorClassAndPosition.vecActorRotation);
			if (oActorClassAndPosition.vecActorScale3D != FVector(0, 0 ,0))
			{
				pActor->SetActorScale3D(oActorClassAndPosition.vecActorScale3D);
			}
			AMyStaticMeshActor *pAMyStaticMeshActor = Cast<AMyStaticMeshActor>(pActor);
			if (pAMyStaticMeshActor)
			{
				pAMyStaticMeshActor->pOwnerTile = this;
			}
			aVisibleActors.push_back(pActor);
		}
	}

	float fRandomStartPosition = static_cast <float> (rand());
	bool bDirectionRandom = 0 != rand() % 2;
	const std::vector<ALevelSequenceActor*>& aALevelSequenceActors = oTileOfMapElement.GetALevelSequenceActors();
	for (ALevelSequenceActor *pALevelSequenceActor : aALevelSequenceActors)
	{
		ULevelSequence *pULevelSequence = pALevelSequenceActor->GetSequence();
		if (!pULevelSequence)
		{
			check(false);
			continue;
		}
		AMyLevelSequenceActor *pAMyLevelSequenceActor = Cast<AMyLevelSequenceActor>(pALevelSequenceActor);

		bool bReverse = false;
		FMovieSceneSequencePlaybackSettings oPlaybackSettings = pALevelSequenceActor->PlaybackSettings;
		if (pAMyLevelSequenceActor && pAMyLevelSequenceActor->DirectionRandom)
		{
			bReverse = bDirectionRandom;
		}
		else
		{
			if (oPlaybackSettings.PlayRate < 0)
			{
				oPlaybackSettings.PlayRate = -oPlaybackSettings.PlayRate;
				bReverse = true;
			}
		}
		ALevelSequenceActor *dummy = nullptr;
		ULevelSequencePlayer *pULevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), pULevelSequence, oPlaybackSettings, dummy);
		if (bReverse)
		{
			pULevelSequencePlayer->PlayReverse();
		}
		else
		{
			pULevelSequencePlayer->Play();
		}

		float fDelayRandom = pAMyLevelSequenceActor->StartDelayMin;
		if (pAMyLevelSequenceActor->StartDelayMin < pAMyLevelSequenceActor->StartDelayMax)
		{
			fDelayRandom = FMath::RandRange(pAMyLevelSequenceActor->StartDelayMin, pAMyLevelSequenceActor->StartDelayMax);
		}
		if (pALevelSequenceActor->PlaybackSettings.LoopCount.Value == -1)
		{
			float fRand = fRandomStartPosition / (static_cast <float> (RAND_MAX / pULevelSequencePlayer->GetDuration().AsSeconds())) + fDelayRandom * pULevelSequencePlayer->GetDuration().AsSeconds();
			if (fRand > pULevelSequencePlayer->GetDuration().AsSeconds())
			{
				fRand -= pULevelSequencePlayer->GetDuration().AsSeconds();
			}
			FFrameTime frameTime = FFrameTime::FromDecimal(fRand);
			pULevelSequencePlayer->PlayToFrame(frameTime);
		}
		else
		{
			FFrameTime frameTime = FFrameTime::FromDecimal(fDelayRandom);
			pULevelSequencePlayer->PlayToFrame(frameTime);
		}
		AddLevelSequencePlayer(pULevelSequencePlayer);
	}
}

void AClickActor::UpdatePosition(int32 iX, int32 iY, FVector vecNewPos)
{
	iXPos = iX;
	iYPos = iY;
	SetActorLocation(vecNewPos);
	for (AActor *pVisibleActor : aVisibleActors)
	{
		pVisibleActor->SetActorLocation(vecNewPos);
	}
}

bool AClickActor::Destroy(bool bNetForce /*= false*/, bool bShouldModifyLevel /*= true*/)
{
	DeleteAll();

	return Super::Destroy(bNetForce, bShouldModifyLevel);
}

void AClickActor::AddLevelSequencePlayer(ULevelSequencePlayer *pULevelSequencePlayer)
{
	if (nullptr == pULevelSequencePlayer)
	{
		check(false);
		return;
	}
	aLevelSequencePlayersStore.Push(pULevelSequencePlayer);
	UMovieSceneSequence *pLevelSequence = pULevelSequencePlayer->GetSequence();
	UMovieScene* MovieScene = pLevelSequence->GetMovieScene();
	if (MovieScene != nullptr)
	{
		for (int ii = 0; ii < MovieScene->GetSpawnableCount(); ++ii)
		{
			FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(ii);
			TArrayView<TWeakObjectPtr<UObject>> OutObjects;
			OutObjects = pULevelSequencePlayer->FindBoundObjects(Spawnable.GetGuid(), FMovieSceneSequenceID(0));
			if (OutObjects.Num() > 0)
			{
				AActor *pSpawnedActor = Cast<AActor>(OutObjects[0].Get());
				if (pSpawnedActor)
				{
					pSpawnedActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
					aSpawnedActors.push_back(pSpawnedActor);
					pSpawnedActor->SetActorHiddenInGame(false);
				}
			}
		}
	}
}
/*
void AClickActor::PlayLevelSequenceOld(ALevelSequenceActor *pLevelSequenceActor)
{
	if (nullptr == pLevelSequenceActor)
	{
		return;
	}
	check(pLevelSequencePlayerStore == nullptr);
	ULevelSequence *pLevelSequence = pLevelSequenceActor->GetSequence();
	if (pLevelSequence != nullptr)
	{
		ULevelSequencePlayer *pULevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), pLevelSequence, pLevelSequenceActor->PlaybackSettings);
		pULevelSequencePlayer->Play();
		if (pLevelSequenceActor->PlaybackSettings.LoopCount == -1)
		{
			float fRand = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / pULevelSequencePlayer->GetLength()));
			pULevelSequencePlayer->SetPlaybackPosition(fRand);
		}
		pLevelSequencePlayerStore = pULevelSequencePlayer;

		UMovieScene* MovieScene = pLevelSequence->GetMovieScene();
		if (MovieScene != nullptr)
		{
			for (int ii = 0; ii < MovieScene->GetSpawnableCount(); ++ii)
			{
				FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(ii);
				TArray<TWeakObjectPtr<UObject>> OutObjects;
				((IMovieScenePlayer*)pULevelSequencePlayer)->GetRuntimeObjects(((IMovieScenePlayer*)pULevelSequencePlayer)->GetRootMovieSceneSequenceInstance(), Spawnable.GetGuid(), OutObjects);
				if (OutObjects.Num() > 0)
				{
					AActor *pSpawnedActor = Cast<AActor>(OutObjects[0].Get());
					if (pSpawnedActor)
					{
						pSpawnedActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
						//aSpawnedActors.push_back(pSpawnedActor);
					}
				}
			}
		}
	}
}*/

void AClickActor::SetGameField(class AGameField *pGameField_)
{
	pGameField = pGameField_;
}

bool AClickActor::IsInvisibleBlock() const
{
	if (eGroundType == eGroundType_Block && aVisibleActors.size()==1)
	{
		UStaticMeshComponent *pMesh = Cast<UStaticMeshComponent>(aVisibleActors.back()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (pMesh && !pMesh->IsVisible())
		{
			return true;
		}
	}
	return false;
}

void AClickActor::StopAnims()
{
	for (ULevelSequencePlayer *pULevelSequencePlayer : aLevelSequencePlayersStore)
	{
		pULevelSequencePlayer->Stop();
	}
}

/*ULevelSequencePlayer* AClickActor::GetLevelSequencePlayer() const
{
	return pLevelSequencePlayerStore;
}*/

void AClickActor::DestroyCoin(AActor *pActor)
{
	for (std::vector<AActor*>::iterator itCoinActor = aVisibleActors.begin(); itCoinActor != aVisibleActors.end(); ++itCoinActor)
	{
		if (*itCoinActor == pActor)
		{
			pActor->Destroy();
			aVisibleActors.erase(itCoinActor);
			break;
		}
	}
}
/*
void AClickActor::Hide()
{
	if (pLevelSequencePlayerStore != nullptr)
	{
		if (!pGameField->IsAnimPoolDisabled())
		{
			ULevelSequence *pLevelSequence = pLevelSequencePlayerStore->GetLevelSequence();
			UMovieScene* MovieScene = pLevelSequence->GetMovieScene();
			if (MovieScene != nullptr)
			{
				for (int ii = 0; ii < MovieScene->GetSpawnableCount(); ++ii)
				{
					FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(ii);
					TArray<TWeakObjectPtr<UObject>> OutObjects;
					((IMovieScenePlayer*)pLevelSequencePlayerStore)->GetRuntimeObjects(((IMovieScenePlayer*)pLevelSequencePlayerStore)->GetRootMovieSceneSequenceInstance(), Spawnable.GetGuid(), OutObjects);
					if (OutObjects.Num() > 0)
					{
						AActor *pSpawnedActor = Cast<AActor>(OutObjects[0].Get());
						if (pSpawnedActor)
						{
							pSpawnedActor->SetActorHiddenInGame(true);
							pSpawnedActor->SetActorLocation(FVector(-10000, 0, 0)); // far away
						}
					}
				}
			}
			//for (AActor *pSpawnedActor : aSpawnedActors)
			//{
			//	pSpawnedActor->SetActorHiddenInGame(true);
			//	pSpawnedActor->SetActorLocation(FVector(-10000, 0, 0)); // far away
			//}
			aSpawnedActors.clear();
		}
		pLevelSequencePlayerStore = nullptr;
	}

	//SetActorLocation(FVector(-10000, 0, 0)); // far away
	for (AActor *pVisibleActor : aVisibleActors)
	{
		pVisibleActor->SetActorHiddenInGame(true);
		pVisibleActor->SetActorLocation(FVector(-10000, 0, 0)); // far away
	}
	UStaticMeshComponent *pBlockMesh = GetStaticMeshComponent();
	if (pBlockMesh)
	{
		pBlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		ECollisionChannel eChannel = pBlockMesh->GetCollisionObjectType();
		if (eChannel == ECC_GameTraceChannel2)
		{ // temporary disable movement base
			pBlockMesh->SetCollisionObjectType(ECC_Destructible);
		}
	}
}

void AClickActor::Show()
{
	for (AActor *pVisibleActor : aVisibleActors)
	{
		if (pGameField->IsConsoleCommandHideTiles())
		{
			pVisibleActor->Destroy();
		}
		else
		{
			pVisibleActor->SetActorHiddenInGame(false);
		}
	}
	if (pGameField->IsConsoleCommandHideTiles())
	{
		aVisibleActors.clear();
	}

	UStaticMeshComponent *pBlockMesh = GetStaticMeshComponent();
	if (pBlockMesh)
	{
		pBlockMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		ECollisionChannel eChannel = pBlockMesh->GetCollisionObjectType();
		if (eChannel == ECC_Destructible)
		{ // enable movement base
			pBlockMesh->SetCollisionObjectType(ECC_GameTraceChannel2);
		}
	}
}
*/
void AClickActor::AnimLanding()
{
	for (AActor *pVisibleActor : aVisibleActors)
	{
		AChickenRunTile *pAChickenRunTile = Cast<AChickenRunTile>(pVisibleActor);
		if (nullptr != pAChickenRunTile)
		{
			pAChickenRunTile->AnimLanding();
		}
	}
}

void AClickActor::AnimLandingEnd()
{
	for (AActor *pVisibleActor : aVisibleActors)
	{
		AChickenRunTile *pAChickenRunTile = Cast<AChickenRunTile>(pVisibleActor);
		if (nullptr != pAChickenRunTile)
		{
			pAChickenRunTile->AnimLandingEnd();
		}
	}
}

void AClickActor::Burn()
{
	AActor *pActor = GetWorld()->SpawnActor<AActor>(pGameField->Fire, GetActorLocation(), FRotator(0, 0, 0));
	aVisibleActors.push_back(pActor);
	eGroundType = eGroundType_Grass;
	eGroundTypeSecond = eGrassType_Fire;
}

void AClickActor::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleClicked();
}

void AClickActor::BlockReleased(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleReleased();
}

void AClickActor::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void AClickActor::OnFingerReleasedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleReleased();
}

void AClickActor::HandleClicked()
{
	/*FConstPawnIterator it = GetWorld()->GetPawnIterator();
	AChickenRunCharacter *pPawn = Cast<AChickenRunCharacter>(it->Get());

	SP_ACMGeneral spACMGeneral = pPawn->GetACMGeneral();
	if (spACMGeneral.Get())
	{
		SP_ACMCharacter spACMCharacter = spACMGeneral->GetACMCharacter();
		if (spACMCharacter.Get())
		{
			spACMCharacter->ClickedCell(iXPos, iYPos);
		}
	}*/
}

void AClickActor::HandleReleased()
{
	AGameModeBase *pGameMode = GetWorld()->GetAuthGameMode();
	AChickenRunGameMode *pCRGameMode = Cast<AChickenRunGameMode>(pGameMode);
	SP_ACMGeneral spACMGeneral = pCRGameMode->GetACMGeneral();
	if (spACMGeneral.Get())
	{
		SP_ACMCharacter spACMCharacter = spACMGeneral->GetACMCharacter();
		if (spACMCharacter.Get())
		{
			spACMCharacter->ReleasedCell(iXPos, iYPos);
		}
	}
}
