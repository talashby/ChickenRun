// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "TileOfMapElement.h"

#include "ClickActor.generated.h"
/**
 * 
 */
UCLASS()
class CHICKENRUN_API AClickActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AClickActor();
	virtual ~AClickActor();
	enum GroundType
	{
		eGroundType_None = 0,
		eGroundType_Grass,
		eGroundType_Water,
		eGroundType_SmallBlock,
		eGroundType_Block
	};
	
	// second type
	enum GroundTypeSecond
	{
		eGroundTypeSecond_Default = 0
	};

	enum GrassTypeSecond
	{
		eGrassType_Default = eGroundTypeSecond_Default,
		eGrassType_LilyLeaf,
		eGrassType_Fire
	};

	enum BlockTypeSecond
	{
		eBlockType_Default = eGroundTypeSecond_Default,
		eBlockType_Invisible
	};

	void InitInternal(int32 iX, int32 iY, const TileOfMapElement &oTileOfMapElement);
	void UpdatePosition(int32 iX, int32 iY, FVector vecNewPos);

	GroundType GetGroundType() const { return eGroundType; }
	int32 GetGroundTypeSecond() const { return eGroundTypeSecond; }

	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = true);
	void AddLevelSequencePlayer(ULevelSequencePlayer *pULevelSequencePlayer);
	//void PlayLevelSequenceOld(ALevelSequenceActor *pLevelSequenceActor);
	static void SetGameField(class AGameField *pGameField_);
	bool IsInvisibleBlock() const;
	void StopAnims();
	//ULevelSequencePlayer* GetLevelSequencePlayer() const;
	void DestroyCoin(AActor *pActor);

	//void Hide();
	//void Show();

	void AnimLanding();
	void AnimLandingEnd();

	void Burn();

protected:
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	UFUNCTION()
	void BlockReleased(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnFingerReleasedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void HandleClicked();
	void HandleReleased();

	/** Dummy root component */
	UPROPERTY(Category = ClickActor, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

private:

	int32 iXPos;
	int32 iYPos;
	GroundType eGroundType;
	int32 eGroundTypeSecond;
	//UPROPERTY()
	std::vector<AActor*> aVisibleActors;
	std::vector<AActor*> aSpawnedActors;
	void DeleteAll();

	UPROPERTY()
	TArray<ULevelSequencePlayer*> aLevelSequencePlayersStore;

	//std::vector<AActor*> aSpawnedActors;
	
	static class AGameField *pGameField;
};
