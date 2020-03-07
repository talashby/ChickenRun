#pragma once

struct ActorClassAndPosition
{
	ActorClassAndPosition(UClass *pActorClass_, FVector vecActorLoacation_, FRotator vecActorRotation_, FVector vecActorScale3D_)
	{
		pActorClass = pActorClass_;
		vecActorLocation = vecActorLoacation_;
		vecActorRotation = vecActorRotation_;
		vecActorScale3D = vecActorScale3D_;
	}
	UClass* pActorClass;
	FVector vecActorLocation;
	FRotator vecActorRotation;
	FVector vecActorScale3D;
};

class TileOfMapElement
{
public:
	TileOfMapElement();

	void AddUClass(UClass *pUClass, int32 eGroundType_, int32 eGroundTypeSecond_ = -1);
	void AddActor(AActor *pActor, int32 eGroundType_ = -1, int32 eGroundTypeSecond_ = -1, FVector vecLocalLocation=FVector(0,0,0));
	void AddLevelSequenceActor(ALevelSequenceActor *pALevelSequenceActor_);

	int32 GetGroundType() const { return eGroundType; }
	int32 GetGroundTypeSecond() const { return eGroundTypeSecond; }
	const std::vector<ALevelSequenceActor*>& GetALevelSequenceActors() const { return aALevelSequenceActors; }

	const std::vector<ActorClassAndPosition>& GetActorClasses() const { return aActorClasses; }

	static TileOfMapElement CreateInvisibleBlock();
	static TileOfMapElement CreateBlock(UClass *pUClassBlock, UClass *pUClassGrass);
	static TileOfMapElement CreateSmallBlock(UClass *pUClassBlock, UClass *pUClassGrass);
	static TileOfMapElement CreateWater();
	static TileOfMapElement CreateGrass(UClass *pUClassGrass);
private:
	void SetTileTypes(int32 eGroundType_, int32 eGroundTypeSecond_ = -1);

	int32 eGroundType;
	int eGroundTypeSecond;
	std::vector<ActorClassAndPosition> aActorClasses;
	std::vector<ALevelSequenceActor*> aALevelSequenceActors;
};

typedef std::vector<TileOfMapElement> tTileOfMapElementVector;
typedef std::list<TileOfMapElement> tTileOfMapElementList;

