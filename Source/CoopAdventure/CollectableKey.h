// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include"Components/AudioComponent.h"
#include "CollectableKeyHolder.h"
#include "CollectableKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCollectableKeyOnCollected);

UCLASS()
class COOPADVENTURE_API ACollectableKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectableKey();

	//create a replicated variable 
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	UCapsuleComponent* Capsule;

	UPROPERTY(ReplicatedUsing = OnRep_IsCollected, BlueprintReadWrite, VisibleAnywhere)
	float IsCollected; //replicated variable!

	UFUNCTION(BlueprintCallable)
	void OnRep_IsCollected();

	UPROPERTY(EditAnywhere)
	float RotationSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UAudioComponent * CollectAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACollectableKeyHolder * KeyHolderRef;

	UPROPERTY(BlueprintAssignable)
	FCollectableKeyOnCollected OnCollected;
};
