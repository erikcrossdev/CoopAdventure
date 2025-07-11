// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Transporter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPADVENTURE_API UTransporter : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTransporter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector StartPoint; 
	FVector EndPoint;

	bool ArePointSet; //just move if the points are set

	UPROPERTY(EditAnywhere)
	bool OwnerIsTriggerActor;

	UPROPERTY(EditAnywhere)
	float MoveTime; 

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggerActors; //Pressure plates to activate transporter. set on the editor

	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount; //check how many pressure plates are active. 

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool AllTriggerActorsTriggered; //when all pressure plates are active

	UFUNCTION(BlueprintCallable)
	void SetPoints(FVector Point1, FVector Point2);
	
	UFUNCTION()
	void OnTriggerActorActivated();

	UFUNCTION()
	void OnTriggerActorDeactivated();
};
