// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "MyStaticMeshActor.h"




AMyStaticMeshActor::AMyStaticMeshActor() : AStaticMeshActor()
{
	OrientationRandom = true;
	ScaleRandom = true;
	ChanceToAppear = 100;

	pOwnerTile = nullptr;
}

void AMyStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

	if (EComponentMobility::Movable == GetStaticMeshComponent()->Mobility)
	{
		if (OrientationRandom)
		{
			FRotator oRotator = GetActorRotation();
			int32 iRand = rand() % 4;
			oRotator.Yaw += 90 * iRand;
			SetActorRotation(oRotator);
		}

		if (ScaleRandom)
		{
			FVector vecScale3D = GetActorScale3D();
			int32 iRand = rand() % 2;
			/*if (iRand == 1)
			{
			vecScale3D.X = -vecScale3D.X;
			}
			else if (iRand == 2)
			{
			vecScale3D.Y = -vecScale3D.Y;
			}
			else */if (iRand == 1)
			{
				vecScale3D.X = -vecScale3D.X;
				vecScale3D.Y = -vecScale3D.Y;
			}
			SetActorScale3D(vecScale3D);
		}

		if (ChanceToAppear <= rand() % 100)
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}
}
