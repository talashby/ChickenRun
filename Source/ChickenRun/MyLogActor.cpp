// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "MyLogActor.h"




AMyLogActor::AMyLogActor() : AMyAnimationBaseActor()
{
	UStaticMeshComponent *pBlockMesh = GetStaticMeshComponent();
	if (pBlockMesh)
	{
		pBlockMesh->SetGenerateOverlapEvents(true);
		pBlockMesh->SetCollisionObjectType(ECC_GameTraceChannel2);
	}
}
