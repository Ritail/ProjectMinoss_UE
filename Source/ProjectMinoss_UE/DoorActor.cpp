// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorActor.h"

#include "InventoryActorComponent.h"

// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	// Root -> Hinge -> DoorMesh, comme Capsule -> SpringArm -> Camera chez Megan

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(Root);
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>

	(TEXT("DoorMesh"));

	DoorMesh->SetupAttachment(Hinge);
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(150.f, 150.f, 100.f));
	// Profil "Trigger" : ne bloque rien, détecte les chevauchements
	Trigger->SetCollisionProfileName(TEXT("Trigger"));

	InteractionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InteractionText"));
	InteractionText->SetupAttachment(Root);

	InteractionText->SetText(FText::GetEmpty());
	InteractionText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	InteractionText->VerticalAlignment = EVRTA_TextCenter;
}

void ADoorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!KeyData)
	{
		return;
	}
	
	RequiredKey = KeyData->Tag; 
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&ADoorActor::OnInteractionTriggerBegin
	);

	Trigger->OnComponentEndOverlap.AddDynamic(
		this,
		&ADoorActor::OnInteractionTriggerEnd
	);
}

// Called every frame
void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rapproche CurrentYaw de sa cible un peu plus à chaque frame
	const float TargetYaw = bIsOpen ? OpenAngle : 0.f;
	// Interpolation de rotation avec un smooth à l'arrêt
	CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime,

	OpenSpeed);

	// FRotator(Pitch, Yaw, Roll) : on tourne autour de Z
	Hinge->SetRelativeRotation(FRotator(0.f, CurrentYaw, 0.f));
}

void ADoorActor::Interact_Implementation(AActor* InteractingActor)
{
	if (bIsPressurePlate)
	{
		return;
	}
	
	if (RequiredKey.IsValid())
	{
		UInventoryActorComponent* Inventory = UInventoryActorComponent::FindInventory(InteractingActor);

		if (!Inventory)
		{
			UE_LOGFMT(LogTemp, Warning, "{0} : {1} n'a pas d'inventaire",

			GetName(), GetNameSafe(InteractingActor));

			return;
		}
		
		if (Inventory->HasKey(RequiredKey))
		{
			bIsOpen = !bIsOpen;
			
			if (bIsUsedKey)
			{
				Inventory->UseKey(RequiredKey);
			}
		}
	}
	else
	{
		bIsOpen = !bIsOpen; // E ouvre, E referme, on intégre l'input action plus bas
	}
}

FText ADoorActor::GetInteractionText_Implementation() const
{
	if (RequiredKey.IsValid())
	{
		if (!bIsOpen)
		{
			return FText::Format(
			FText::FromString(TEXT("Verrouillée ({0})")),
			 FText::FromName(RequiredKey.GetTagLeafName()));
		}

		return FText::FromString(TEXT("Fermer"));
	}

	if (bIsPressurePlate)
	{
		return FText::FromString(bIsOpen ? TEXT("Fermer") : TEXT("Verrouillée par une plaque"));
	}
	
	return FText::FromString(bIsOpen ? TEXT("Fermer") : TEXT("Ouvrir"));
}

void ADoorActor::OnInteractionTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UInventoryActorComponent* Inventory = UInventoryActorComponent::FindInventory(OtherActor);

	if (!Inventory)
	{
		UE_LOGFMT(LogTemp, Warning, "{0} : {1} n'a pas d'inventaire",

		GetName(), GetNameSafe(OtherActor));

		return;
	}
	
	InteractionText->SetText(
			Execute_GetInteractionText(this)
		);
}

void ADoorActor::OnInteractionTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UInventoryActorComponent* Inventory = UInventoryActorComponent::FindInventory(OtherActor);

	if (!Inventory)
	{
		UE_LOGFMT(LogTemp, Warning, "{0} : {1} n'a pas d'inventaire",

		GetName(), GetNameSafe(OtherActor));

		return;
	}
	
	InteractionText->SetText(
			IInteractable::Execute_GetInteractionText(this)
		);
}

void ADoorActor::Open()
{
	if (bOpensWithKey)
		return;
	
	bIsOpen = true;
}

void ADoorActor::Close()
{
	if (bOpensWithKey)
		return;
	
	bIsOpen = false;
}

