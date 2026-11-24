// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlateActor.h"


namespace
{
	// De combien la plaque s'enfonce, en cm
	constexpr float PressDepth = 5.f;

}

// Sets default values
APressurePlateActor::APressurePlateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>

	(TEXT("PlateMesh"));

	PlateMesh->SetupAttachment(Root);
	// Une boîte plate juste au-dessus de la plaque
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(50.f, 50.f, 20.f));
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void APressurePlateActor::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APressurePlateActor::OnTriggerBeginOverlap);

	Trigger->OnComponentEndOverlap.AddDynamic(this, &APressurePlateActor::OnTriggerEndOverlap);
}

// Called every frame
void APressurePlateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APressurePlateActor::UpdatePressure()
{
	CurrentMassKg = ComputeMassOnPlate();
	const bool bShouldBePressed = CurrentMassKg >= RequiredMassKg;
	// Rien à faire si l'état ne change pas
	if (bShouldBePressed == bIsPressed)
	{
		return;
	}
	bIsPressed = bShouldBePressed;
	PlateMesh->SetRelativeLocation(FVector(0.f, 0.f, bIsPressed ? -

	PressDepth : 0.f));

	if (TargetDoor)
	{
		if (bIsPressed)
		{
			TargetDoor->Open();
		}
		else
		{
			TargetDoor->Close();
		}
	}
}

float APressurePlateActor::ComputeMassOnPlate() const
{
	TArray<UPrimitiveComponent*> Overlapping;
	Trigger->GetOverlappingComponents(Overlapping);
	float TotalMass = 0.f;
	for (const UPrimitiveComponent* Component : Overlapping)
	{
		// Seuls les corps simulés pèsent
		if (Component && Component->IsSimulatingPhysics())
		{
			TotalMass += Component->GetMass();
		}
	}
	return TotalMass;
}

void APressurePlateActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UpdatePressure();
}

void APressurePlateActor::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UpdatePressure();
}
