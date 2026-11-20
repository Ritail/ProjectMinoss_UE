// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "CrateActor.generated.h"

class UStaticMeshComponent;
class USoundBase;

UCLASS()
class PROJECTMINOSS_UE_API ACrateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrateActor();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Crate")
	bool bFragile = false;
	// Grisé tant que la caisse n'est pas fragile
	UPROPERTY(EditAnywhere, Category = "Crate", meta = (EditCondition =

	"bFragile", ClampMin = "0"))
	float BreakImpulse = 25000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crate|Sound")
	TObjectPtr<USoundBase> ImpactSound;
	// En dessous : contact au repos, rien ne se passe
	UPROPERTY(EditDefaultsOnly, Category = "Crate|Sound", meta =(ClampMin = "0"))
	float MinImpactImpulse = 3000.f;
	// Au-dessus : volume maximal
	UPROPERTY(EditDefaultsOnly, Category = "Crate|Sound", meta =(ClampMin = "0"))
	float MaxImpactImpulse = 40000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crate|Sound", meta =

	(ClampMin = "0", Units = "s"))
	float SoundCooldown = 0.2f;
	// Utilisé seulement si l'acteur touché a un UHealthComponent (TP 5 bis, Défi 4)

	UPROPERTY(EditDefaultsOnly, Category = "Crate|Damage", meta = (ClampMin = "0"))
	float DamagePerImpulse = 0.001f;

private:

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const

	FHitResult& Hit);
	float LastSoundTime = -1000.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Crate")
	TObjectPtr<UStaticMeshComponent> Mesh;
	// Chaque caisse du niveau peut avoir sa propre masse
	UPROPERTY(EditAnywhere, Category = "Crate", meta = (ClampMin = "0.1",Units = "kg"))

	float MassKg = 20.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
