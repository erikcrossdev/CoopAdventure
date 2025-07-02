// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Transporter.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MovableActor.generated.h"

UCLASS()
class COOPADVENTURE_API AMovableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* RootComp; //root of the obj

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UArrowComponent *Point1; //arrows in the editor to set start point

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UArrowComponent *Point2;//arrows in the editor to set end point

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTransporter* Transporter;
};
