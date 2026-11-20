// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyHandler.h"

#include "InventoryActorComponent.h"

// Sets default values
AKeyHandler::AKeyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(Root);
	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>

	(TEXT("DoorMesh"));

	KeyMesh->SetupAttachment(Hinge);
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(150.f, 150.f, 100.f));
	// Profil "Trigger" : ne bloque rien, détecte les chevauchements
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

void AKeyHandler::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!KeyData)
	{
		return;
	}

	if (KeyData->Mesh)
	{
		KeyMesh->SetStaticMesh(KeyData->Mesh);
	}
	
	// Copie modifiable du matériau, propre à cette clé
	if (UMaterialInstanceDynamic* Material = KeyMesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), KeyData->Color);
	}
}

// Called when the game starts or when spawned
void AKeyHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKeyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyHandler::Interact_Implementation(AActor* InteractingActor)
{
	UInventoryActorComponent* Inventory = UInventoryActorComponent::FindInventory(InteractingActor);

	if (!Inventory)
	{
		UE_LOGFMT(LogTemp, Warning, "{0} : {1} n'a pas d'inventaire",

		GetName(), GetNameSafe(InteractingActor));

		return;
	}
	Inventory->AddKey(KeyData->Tag);
	
	Destroy();
}

