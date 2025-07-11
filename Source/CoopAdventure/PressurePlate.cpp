// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Activated = false;
	//Set replication
	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh -> SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);
	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>
		(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder")); //Find Shape into the mesh
	if (TriggerMeshAsset.Succeeded()) { ///if it was found, set it
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3f, 3.3f, 0.2f)); //change scale
		TriggerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>
		(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08")); //Find Shape into the mesh
	if (MeshAsset.Succeeded()) { ///if it was found, set it
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 0.5f)); //change scale
		Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 7.2f));
	}

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));
	Transporter->MoveTime = 0.25f;
	Transporter->OwnerIsTriggerActor = true;
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));

	FVector Point2 = GetActorLocation() + FVector(0.0f, 0.0f, -10.0f); //10cm lower
	Transporter->SetPoints(GetActorLocation(), Point2);
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority()) { //only call on server
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;
		TriggerMesh->GetOverlappingActors(OverlappingActors); //fill OverlappingActors array with all overlapping actors
		for (int ActorIdx = 0; ActorIdx < OverlappingActors.Num(); ++ActorIdx) {
			AActor* A = OverlappingActors[ActorIdx];
			if (A->ActorHasTag("TriggerActor")) {
				TriggerActor = A;
				break;
			}
			//FString Msg = FString::Printf(TEXT("Name: %s"), *A->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Msg);
		}

		if (TriggerActor) {
			if(!Activated) { 
				Activated = true; 
				FString Msg = FString::Printf(TEXT("Activated: %s"), Activated ? TEXT("true") : TEXT("false"));
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Msg);
				OnActivated.Broadcast();
			}			
		}
		else {
			if (Activated) { 
				Activated = false; 
				FString Msg = FString::Printf(TEXT("Activated: %s"), Activated ? TEXT("true") : TEXT("false"));
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Msg);
				OnDeactivated.Broadcast();
			}
			
		}
	}
}

