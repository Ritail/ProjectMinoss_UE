// Fill out your copyright notice in the Description page of Project Settings.


#include "CrateActor.h"

// Sets default values
ACrateActor::ACrateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh); // le composant qui simule DOIT être la racine
	// Object type PhysicsBody, bloque le monde et les pawns
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetLinearDamping(0.2f);
	Mesh->SetAngularDamping(1.f);
	// Pour la plaque de pression (étape 4)
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetNotifyRigidBodyCollision(true);
}

void ACrateActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	Mesh->SetMassOverrideInKg(NAME_None, MassKg, true);
}

// Called when the game starts or when spawned
void ACrateActor::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &ACrateActor::OnMeshHit);
}

void ACrateActor::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float Impulse = NormalImpulse.Size();
	// Pour régler les seuils : décommentez, posez, lâchez et lancez une caisse

	// UE_LOGFMT(LogTemp, Log, "{0} : impulsion {1}", GetName(),Impulse);

	if (Impulse < MinImpactImpulse)
	{

		return; // une caisse posée produit des hits faibles à chaque frame
	}
	// Un délai entre deux sons, sinon un choc en plusieurs contactssonne plusieurs fois

	const float Now = GetWorld()->GetTimeSeconds();
	if (ImpactSound && Now - LastSoundTime > SoundCooldown)
	{
		const float Volume = FMath::GetMappedRangeValueClamped(FVector2D(MinImpactImpulse, MaxImpactImpulse),
			FVector2D(0.2f, 1.f), Impulse);
		
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound,Hit.ImpactPoint, Volume);

		LastSoundTime = Now;
	}
	
	// Sans UHealthComponent sur l'autre acteur, l'appel ne fait rien
	if (OtherActor && OtherActor != this)
	{

		UGameplayStatics::ApplyDamage(OtherActor, Impulse *

		DamagePerImpulse, nullptr, this, nullptr);
	}
	if (bFragile && Impulse >= BreakImpulse)
	{

		Destroy();
	}
}

// Called every frame
void ACrateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

