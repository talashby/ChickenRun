
#include "ChickenRun.h"
#include "TileOfMapElement.h"
#include "ClickActor.h"
#include "MyStaticMeshActor.h"

TileOfMapElement::TileOfMapElement()
{
	eGroundType = AClickActor::eGroundType_None;
	eGroundTypeSecond = AClickActor::eGroundTypeSecond_Default;
}

void TileOfMapElement::AddUClass(UClass *pUClass, int32 eGroundType_, int32 eGroundTypeSecond_ /*= -1*/)
{
	check(pUClass != nullptr);
	aActorClasses.push_back(ActorClassAndPosition(pUClass, FVector(0, 0, 0), FRotator(0, 0, 0), FVector(0,0,0)));
	if (eGroundType_ != -1)
	{
		SetTileTypes(eGroundType_, eGroundTypeSecond_);
	}
}

void TileOfMapElement::AddActor(AActor *pActor, int32 eGroundType_, int32 eGroundTypeSecond_, FVector vecLocalLocation)
{
	if (pActor != nullptr)
	{
		FRotator oRotator = pActor->GetActorRotation();
		FVector vecScale3D = pActor->GetActorScale3D();
		{ // Orientation random
			AMyStaticMeshActor *pAMyStaticMeshActor = Cast<AMyStaticMeshActor>(pActor);
			if (pAMyStaticMeshActor)
			{
				if (pAMyStaticMeshActor->OrientationRandom)
				{
					int32 iRand = rand() % 4;
					oRotator = FRotator(0, 90 * iRand, 0);
				}
			}
		}
		{ // Scale random
			AMyStaticMeshActor *pAMyStaticMeshActor = Cast<AMyStaticMeshActor>(pActor);
			if (pAMyStaticMeshActor)
			{
				if (pAMyStaticMeshActor->ScaleRandom)
				{
					int32 iRand = rand() % 4;
					if (iRand == 1)
					{
						vecScale3D.X = -vecScale3D.X;
					}
					else if (iRand == 2)
					{
						vecScale3D.Y = -vecScale3D.Y;
					}
					else if (iRand == 3)
					{
						vecScale3D.X = -vecScale3D.X;
						vecScale3D.Y = -vecScale3D.Y;
					}
				}
			}
		}
		aActorClasses.push_back(ActorClassAndPosition(pActor->GetClass(), vecLocalLocation, oRotator, vecScale3D));
	}

	if (eGroundType_ != -1)
	{
		SetTileTypes(eGroundType_, eGroundTypeSecond_);
	}
}

void TileOfMapElement::AddLevelSequenceActor(ALevelSequenceActor *pALevelSequenceActor_)
{
	aALevelSequenceActors.push_back(pALevelSequenceActor_);
}

TileOfMapElement TileOfMapElement::CreateInvisibleBlock()
{
	TileOfMapElement oTileOfMapElement;
	oTileOfMapElement.AddActor(nullptr, AClickActor::eGroundType_Block, AClickActor::eBlockType_Invisible);
	return oTileOfMapElement;
}

TileOfMapElement TileOfMapElement::CreateBlock(UClass *pUClassBlock, UClass *pUClassGrass)
{
	TileOfMapElement oTileOfMapElement;
	oTileOfMapElement.AddUClass(pUClassBlock, AClickActor::eGroundType_Block);
	oTileOfMapElement.AddUClass(pUClassGrass, AClickActor::eGroundType_Grass);
	return oTileOfMapElement;
}

TileOfMapElement TileOfMapElement::CreateSmallBlock(UClass *pUClassBlock, UClass *pUClassGrass)
{
	TileOfMapElement oTileOfMapElement;
	oTileOfMapElement.AddUClass(pUClassBlock, AClickActor::eGroundType_SmallBlock);
	oTileOfMapElement.AddUClass(pUClassGrass, AClickActor::eGroundType_Grass);
	return oTileOfMapElement;
}

TileOfMapElement TileOfMapElement::CreateWater()
{
	TileOfMapElement oTileOfMapElement;
	oTileOfMapElement.AddActor(nullptr, AClickActor::eGroundType_Water);
	return oTileOfMapElement;
}

TileOfMapElement TileOfMapElement::CreateGrass(UClass *pUClassGrass)
{
	TileOfMapElement oTileOfMapElement;
	oTileOfMapElement.AddUClass(pUClassGrass, AClickActor::eGroundType_Grass);
	return oTileOfMapElement;
}

void TileOfMapElement::SetTileTypes(int32 eGroundType_, int32 eGroundTypeSecond_ /*= -1*/)
{
	if (-1 != eGroundType)
	{
		if ((eGroundType_ == AClickActor::eGroundType_Block || eGroundType_ == AClickActor::eGroundType_SmallBlock) && eGroundType == AClickActor::eGroundType_Grass)
		{
			eGroundType = eGroundType_;
		}
		else if (eGroundType_ == AClickActor::eGroundType_Grass && (eGroundType == AClickActor::eGroundType_Block || eGroundType == AClickActor::eGroundType_SmallBlock))
		{
			//do nothing
		}
		else
		{
			eGroundType = eGroundType_;
		}
	}


	if (-1 != eGroundTypeSecond_)
	{
		if (eGroundTypeSecond != AClickActor::eGroundTypeSecond_Default)
		{
			UE_LOG(LogGeneral, Error, TEXT("Second tile type added twice"));
		}
		eGroundTypeSecond = eGroundTypeSecond_;
	}
}
