// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

typedef std::vector<class AClickActor*> tBlockVector;


#include "TileOfMapElement.h"

#include "GameField.generated.h"

USTRUCT()
struct FCRMapElement
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Blueprints")
	ATriggerVolume *MapElement;
	UPROPERTY(EditAnywhere, Category = "Blueprints")
	uint8 ChanceToAppear;
};

UCLASS()
class CHICKENRUN_API AGameField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameField();
	virtual ~AGameField();

	AGameField* Clone();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick( float DeltaTime) override;

	void TickPublic(float DeltaTime);

	void Restart(int32 iPreviousMapLength); // return true - success
	bool RestartFirstMap(int32 &iPlayerStartXPos, int32 &iPlayerStartYPos); // return true - success
	void PawnPositionChanged(int32 iXPos, int32 iYPos);
	bool IsConsoleCommandHideTiles() const;
	void ConsoleCommandHideTiles();
	void ConsoleCommandStopAnims();
	void ConsoleCommandDisableTilePool();
	void ConsoleCommandEnableTilePool();

	bool IsAnimPoolDisabled() const { return true; }

	bool bConsoleCommandHideTiles;
	//const std::vector< tBlockVector >& GetGameField() const;
	class AClickActor* GetClickActor(int32 iXPos, int32 iYPos);

	UClass* GetCoinClass() const { return CoinClass; }
	int32 GetMapLength() const { return MapLength + iServiceMapLength; }
	void SetMapLength(int32 iNewMapLength) { MapLength = iNewMapLength; }
	int32 GetMultimapStartX() const { return iMultiMapStartX; }
	void BurnRow(int32 iRow);

	bool IsSimpleControl() const { return SimpleControl; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> Log;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> CarClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> CoinClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> Fire;

//	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
protected:
	bool IsClassInArray(TSubclassOf<class AActor> pActor, const TArray<TSubclassOf<class AActor>> &aActorArray);
	TSubclassOf<class AActor> GetRandomArrayElement(const TArray<TSubclassOf<class AActor>> &aActorArray);
	//FString FindPlayerStartTile();

	const int32 iSideInvisibleBlocksYSize = 7;
	const int32 iSideBlocksYSize = 1;
	const int32 iActiveZoneYSize = 9;
	const int32 iFullYSize = iSideInvisibleBlocksYSize * 2 + iSideBlocksYSize * 2 + iActiveZoneYSize;
	const int32 iFirstSideBlockTileY = iSideInvisibleBlocksYSize;
	const int32 iFirstPlayTileY = iSideInvisibleBlocksYSize + iSideBlocksYSize;
	const int32 iMiddleY = iSideInvisibleBlocksYSize + iSideBlocksYSize + (iActiveZoneYSize - 1) / 2;
	const int32 iLastPlayTileY = iSideInvisibleBlocksYSize + iSideBlocksYSize + iActiveZoneYSize - 1;
	const int32 iLastSideBlockTileY = iSideInvisibleBlocksYSize + iSideBlocksYSize * 2 + iActiveZoneYSize - 1;

	const int32 iMapVisibilityTiles = 15;
	const int32 iMapDelayVisibilityTiles = 14;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameField)
	bool SimpleControl;
	/* Center tile coordinate */
	UPROPERTY(Category = GameField, VisibleAnywhere, BlueprintReadOnly)
	int32 GameFieldCenterY = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TArray<TSubclassOf<class AActor>> BackgroundElements;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TArray<TSubclassOf<class AActor>> GroundRandom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> GroundLeaf;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TArray<TSubclassOf<class AActor>> BlockRandom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TArray<TSubclassOf<class AActor>> BlockLowRandom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprints")
	TSubclassOf<class AActor> Water;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGeneration")
	int32 MapLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGeneration")
	ATriggerVolume *StartVolume;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGeneration")
	ATriggerVolume *FinishVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGenerationGrass", meta = (UIMin = "0", UIMax = "100"))
	uint8 ChanceToAppearBlockOnGround;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGenerationGrass", meta = (UIMin = "0", UIMax = "100"))
	uint8 ChanceToAppearSmallBlockOnGround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapGeneration", meta = (UIMin = "0", UIMax = "100"))
	uint8 ChanceToAppearRiverWithLeafs;
	UPROPERTY(EditAnywhere, Category = "MapGeneration")
	TArray<FCRMapElement> MapElements;

	/** Dummy root component */
	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent *pName;


private:
	void SpawnRiverWithLeafs();
	void SpawnGrassTile(int32 iY);
	void SpawnGrassElement();
	bool SpawnMapElement(ATriggerVolume *pTriggerVolume);
	void InitClickActorFromPoolDelayed(const TileOfMapElement &oTileOfMapElement);
	void InitClickActorFromPool(const TileOfMapElement &oTileOfMapElement);
	void InitClickActorFromPool(int32 iX, int32 iY, const TileOfMapElement &oTileOfMapElement);
	//void SmoothInitClickActor()
	void DestroyClickActor(int32 iX, int32 iY);
	//ULevelSequencePlayer* GetULevelSequencePlayerFromPool(ALevelSequenceActor *pALevelSequenceActor);
	//void AddULevelSequencePlayerToPool(ULevelSequencePlayer *pULevelSequencePlayer);
	bool IsFinishVolumeShouldBeSpawned();

	//class AChickenRunCharacter *pPawn;
	std::vector< tBlockVector > aBlocksField;

	int32 iCurrentPawnXPos;
	int32 iPlayerStartXPos_SpawnMapElement, iPlayerStartYPos_SpawnMapElement;
	
	std::map<ULevelSequence*, std::vector<ULevelSequencePlayer*> > aLevelSequencePlayers;

	UPROPERTY()
	TSet<ULevelSequencePlayer*> aLevelSequencePlayersStore;

	//UPROPERTY()
	//class UDecalComponent* CursorToWorld;

	tTileOfMapElementList aDelayTiles;
	class AGeneralSettings *pAGeneralSettings;

	bool bStartMapVolumeSpawned;
	bool bFinishMapVolumeSpawned;
	int32 iMultiMapStartX;
	int32 iServiceMapLength;

	int32 iMapElementsWeight;
	int iFinishVolumeNumTilesX;
};
