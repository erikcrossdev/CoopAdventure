
#include "Transporter.h"
#include "PressurePlate.h"

// Sets default values for this component's properties
UTransporter::UTransporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//set replication
	SetIsReplicatedByDefault(true); //use This version for components

	MoveTime = 3.0f;
	ActivatedTriggerCount = 0;
	ArePointSet = false;

	StartPoint = FVector(0, 0, 0);
	EndPoint = FVector::Zero();
}


// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* TA : TriggerActors) {
		APressurePlate* PressurePlate = Cast<APressurePlate>(TA);
		if (PressurePlate) {
			PressurePlate->OnActivated.AddDynamic(this, &UTransporter::OnPressurePlateActivated);
			PressurePlate->OnDeactivated.AddDynamic(this, &UTransporter::OnPressurePlateDeactivated);
		}
	}
}


// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TriggerActors.Num() > 0) {
		AllTriggerActorsTriggered = ActivatedTriggerCount >= TriggerActors.Num();
		if (AllTriggerActorsTriggered) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString("AllTriggerActorsTriggered!"));
		}
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner && MyOwner->HasAuthority() && ArePointSet) { //we will move only in the server and replicate the movement with set replicated
		FVector CurrentLocation = MyOwner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime; //Calculates the distance

		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint: StartPoint;
		if (!CurrentLocation.Equals(TargetLocation)) 
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed); //interpolate value
			MyOwner->SetActorLocation(NewLocation);
		}
	} 
}

void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if (Point1.Equals(Point2)) return;

	StartPoint = Point1;
	EndPoint = Point2;
	ArePointSet = true;
}

void UTransporter::OnPressurePlateActivated()
{
	ActivatedTriggerCount++;

	FString Msg = FString::Printf(TEXT("On Pressure Plate activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Msg);
}

void UTransporter::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;

	FString Msg = FString::Printf(TEXT("On Pressure Plate Deactivated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Msg);
}

