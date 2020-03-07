// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "MyAnimationBaseActor.h"




AMyAnimationBaseActor::AMyAnimationBaseActor() : AStaticMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	// Create dummy root scene component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	//GetStaticMeshComponent()->SetRelativeLocation(FVector(0, 0, 0));
	GetStaticMeshComponent()->SetupAttachment(RootSceneComponent);

	OrientationRandomStep = 180;
	ScaleRandom = true;
	ChanceToAppear = 100;
}

void AMyAnimationBaseActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<UStaticMeshComponent*> aComponents;
	GetComponents<UStaticMeshComponent>(aComponents);

	for (UStaticMeshComponent *pUStaticMeshComponent : aComponents)
	{
		if (OrientationRandomStep != 0)
		{
			FRotator oRotator = pUStaticMeshComponent->RelativeRotation;
			if (OrientationRandomStep == 90)
			{
				int32 iRand = rand() % 4;
				oRotator.Yaw += 90 * iRand;
				pUStaticMeshComponent->SetRelativeRotation(oRotator);
			}
			else if (OrientationRandomStep == 180)
			{
				int32 iRand = rand() % 2;
				oRotator.Yaw += 180 * iRand;
				GetStaticMeshComponent()->SetRelativeRotation(oRotator);
			}
		}

		if (ScaleRandom)
		{
			FVector vecScale3D = GetStaticMeshComponent()->GetComponentScale();
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
			pUStaticMeshComponent->SetRelativeScale3D(vecScale3D);
			//SetActorScale3D(vecScale3D);
		}
	}

	if (ChanceToAppear <= rand() % 100)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AMyAnimationBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}