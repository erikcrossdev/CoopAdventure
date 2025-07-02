// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableActor.h"

// Sets default values
AMovableActor::AMovableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	//set replicated movement
	bReplicates = true;
	SetReplicateMovement(true);

	//Create components
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));

	Point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point1"));
	Point1->SetupAttachment(RootComp);
	Point1->SetRelativeLocation(FVector::Zero());

	Point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point2"));
	Point2->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	Point2->SetupAttachment(RootComp);
}


void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	FVector StartPoint = GetActorLocation() + Point1->GetRelativeLocation(); //get world location

	FVector EndPoint = GetActorLocation() + Point2->GetRelativeLocation(); //get world location

	Transporter->SetPoints(StartPoint, EndPoint);
	
}

// Called every frame
void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

