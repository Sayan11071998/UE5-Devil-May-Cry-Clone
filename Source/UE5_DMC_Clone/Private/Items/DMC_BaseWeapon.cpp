#include "Items/DMC_BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DamageTypes/DMC_DamageType.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Enemies/DMC_EnemyCharacterBase.h"

ADMC_BaseWeapon::ADMC_BaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RootComponent = WeaponMesh;
	
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(WeaponMesh);
	TrailComponent->bAutoActivate = false;
}

void ADMC_BaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	HandleCollisionTracing();
}

void ADMC_BaseWeapon::Equip(TObjectPtr<USceneComponent> InParent, FName InSocketName, TObjectPtr<AActor> NewOwner,
		TObjectPtr<APawn> NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
}

void ADMC_BaseWeapon::AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void ADMC_BaseWeapon::StartCollision(TSubclassOf<UDMC_DamageType> DamageType)
{
	AlreadyHitActors.Empty();
	CurrentDamageType = DamageType;
	bIsCollisionActive = true;
	
	SetActorTickEnabled(true);
	ToggleTrail(true);
}

void ADMC_BaseWeapon::EndCollision()
{
	bIsCollisionActive = false;
	
	SetActorTickEnabled(false);
	ToggleTrail(false);
}

void ADMC_BaseWeapon::ToggleTrail(bool bActivate)
{
	if (!TrailComponent || !TrailSystem || !bShowSlashTrail) return;
	
	if (bActivate)
	{
		TrailComponent->SetAsset(TrailSystem);
		TrailComponent->Activate();
	}
	else
	{
		TrailComponent->Deactivate();
	}
}

void ADMC_BaseWeapon::HandleCollisionTracing()
{
	if (bIsCollisionActive && WeaponMesh)
	{
		const FVector TraceStart = WeaponMesh->GetSocketLocation(StartSocketName);
		const FVector TraceEnd = WeaponMesh->GetSocketLocation(EndSocketName);
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		ActorsToIgnore.Add(GetOwner());
		
		TArray<FHitResult> OutHits;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			TraceRadius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true
		);
		
		if (bHit)
		{
			for (const FHitResult& Hit : OutHits)
			{
				AActor* HitActor = Hit.GetActor();
				if (IsValid(HitActor) && !AlreadyHitActors.Contains(HitActor))
				{
					// Friendly Fire Check
					if (HitActor->IsA(ADMC_PlayerCharacter::StaticClass()) && GetOwner()->IsA(ADMC_PlayerCharacter::StaticClass())) continue;
					if (HitActor->IsA(ADMC_EnemyCharacterBase::StaticClass()) && GetOwner()->IsA(ADMC_EnemyCharacterBase::StaticClass())) continue;

					AlreadyHitActors.AddUnique(HitActor);
					
					float FinalDamage = BaseDamage;
					if (ADMC_PlayerCharacter* Character = Cast<ADMC_PlayerCharacter>(GetOwner()))
					{
						if (Character->IsRaging() && Character->GetComboData())
						{
							FinalDamage *= Character->GetComboData()->RageDamageMultiplier;
						}
						Character->HitStop();
					}

					UGameplayStatics::ApplyPointDamage(
						HitActor,
						FinalDamage,
						Hit.ImpactNormal,
						Hit,
						GetInstigatorController(),
						GetOwner(),
						CurrentDamageType
					);
				}
			}
		}
	}
}