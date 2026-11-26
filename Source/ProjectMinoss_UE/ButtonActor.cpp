// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonActor.h"

// Sets default values
AButtonActor::AButtonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);
	
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(BaseMesh);
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(10.f, 10.f, 10.f));
	// Profil "Trigger" : ne bloque rien, détecte les chevauchements
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AButtonActor::BeginPlay()
{
	Super::BeginPlay();
	InitialRelativeLocation = ButtonMesh->GetRelativeLocation();
	
}

// Called every frame
void AButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetTranslate = bIsPressed ? PressedTranslate : 0.f;
	
	CurrentTranslate = FMath::FInterpTo(CurrentTranslate, TargetTranslate, DeltaTime,PressedSpeed);
	
	ButtonMesh->SetRelativeLocation(InitialRelativeLocation + FVector(-CurrentTranslate, 0.f, 0.f));
}

void AButtonActor::Interact_Implementation(AActor* InteractingActor)
{
	if (bIsPressed) return;
	
	SetPressed(true);
}

void AButtonActor::SetPressed(bool bNewPressed)
{
	if (bIsPressed == bNewPressed)
	{
		return;
	}

	bIsPressed = bNewPressed;
	OnStateChanged.Broadcast();
}

