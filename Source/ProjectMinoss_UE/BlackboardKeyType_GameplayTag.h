// BlackboardKeyType_GameplayTag.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "GameplayTagContainer.h"
#include "BlackboardKeyType_GameplayTag.generated.h"

UCLASS(EditInlineNew, CollapseCategories, meta=(DisplayName="Gameplay Tag"))
class PROJECTMINOSS_UE_API UBlackboardKeyType_GameplayTag : public UBlackboardKeyType
{
	GENERATED_BODY()

public:
	UBlackboardKeyType_GameplayTag(const FObjectInitializer& ObjectInitializer);

	/** Required for UBlackboardComponent::GetValue<T> / SetValue<T> templates. */
	using FDataType = FGameplayTag;

	/** Returned by GetValue when the key is unset. Must be a static member. */
	inline static const FDataType InvalidValue = FGameplayTag();

	// --- Static API used by UBlackboardComponent templates ---
	// Note: first parameter is the key-type pointer (matches engine pattern, e.g. UBlackboardKeyType_Object).
	static FGameplayTag GetValue(const UBlackboardKeyType* KeyOb, const uint8* RawData);
	static bool SetValue(UBlackboardKeyType* KeyOb, uint8* RawData, const FGameplayTag& Value);

	virtual EBlackboardCompare::Type CompareValues(
		const UBlackboardComponent& OwnerComp,
		const uint8* MemoryBlock,
		const UBlackboardKeyType* OtherKeyOb,
		const uint8* OtherMemoryBlock) const override;

protected:
	virtual void InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual void FreeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual FString DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const override;
	virtual bool TestBasicOperation(
		const UBlackboardComponent& OwnerComp,
		const uint8* MemoryBlock,
		EBasicKeyOperation::Type Op) const override;
};