#include "Enemies/DMC_EnemyCharacterBase.h"
#include "Items/DMC_BaseWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "UI/DMC_EnemyHealthBar.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"

ADMC_EnemyCharacterBase::ADMC_EnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Health = MaxHealth;
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("CombatBuffer"));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetMesh());
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawAtDesiredSize(false);
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
}

void ADMC_EnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetDrawSize(FVector2D(HealthBarDrawSize_X, HealthBarDrawSize_Y));
		
		if (UDMC_EnemyHealthBar* HealthBar = Cast<UDMC_EnemyHealthBar>(HealthBarWidget->GetUserWidgetObject()))
		{
			HealthBar->SetOwnerActor(this);
			HealthBar->SetHealthPercent(Health / MaxHealth);
		}
	}
}

void ADMC_EnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthBarWidget)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		UDMC_EnemyHealthBar* HealthBar = Cast<UDMC_EnemyHealthBar>(HealthBarWidget->GetUserWidgetObject());
		
		if (PlayerPawn && HealthBar && !bDead)
		{
			float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
			bool bShouldBeVisible = Distance <= HealthBar->GetDisplayRange();
			
			if (HealthBarWidget->IsVisible() != bShouldBeVisible)
			{
				HealthBarWidget->SetVisibility(bShouldBeVisible);
			}
		}
		else if (HealthBarWidget->IsVisible())
		{
			HealthBarWidget->SetVisibility(false);
		}
	}
}

bool ADMC_EnemyCharacterBase::CanBeFinished() const
{
	if (bDead) return false;
	return (Health / MaxHealth) <= 0.1f;
}

void ADMC_EnemyCharacterBase::OnFinished(TObjectPtr<AActor> Attacker)
{
	if (bDead) return;

	Health = 0.f;

	if (FinishedMontage)
	{
		PlayAnimMontage(FinishedMontage);
		
		// Delay Death() until after the Finished montage completes
		float MontageLength = FinishedMontage->GetPlayLength();
		FTimerHandle FinishedTimerHandle;
		GetWorldTimerManager().SetTimer(FinishedTimerHandle, [this]()
		{
			Death(true);
		}, MontageLength, false);
	}
	else
	{
		Death();
	}
}

float ADMC_EnemyCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead) return 0.f;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		SpawnHitFX(DamageCauser, PointDamageEvent->HitInfo);
	}

	if (DamageCauser)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
		SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

	if (HealthBarWidget)
	{
		if (UDMC_EnemyHealthBar* HealthBar = Cast<UDMC_EnemyHealthBar>(HealthBarWidget->GetUserWidgetObject()))
		{
			HealthBar->SetHealthPercent(Health / MaxHealth);
		}
	}

	if (Health <= 0.f)
	{
		Death();
	}
	else if (DamageEvent.DamageTypeClass)
	{
		UDMC_DamageType* DamageTypeObject = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DamageTypeObject)
		{
			PlayHitReaction(DamageTypeObject->DamageType);
		}
	}
	
	return DamageAmount;
}

void ADMC_EnemyCharacterBase::SpawnHitFX(TObjectPtr<AActor> DamageCauser, const FHitResult& HitResult)
{
	if (!HitVFX || !DamageCauser) return;
	
	FRotator YawRot = UKismetMathLibrary::FindLookAtRotation(DamageCauser->GetActorLocation(), GetActorLocation());
	FRotator TiltRot = UKismetMathLibrary::FindLookAtRotation(HitResult.ImpactPoint, HitResult.TraceEnd);
	FRotator FinalRot = FRotator(TiltRot.Pitch, YawRot.Yaw + 90.f, TiltRot.Roll); 
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFX, HitResult.ImpactPoint, FinalRot);
}

void ADMC_EnemyCharacterBase::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	if (HitReactionMap.Contains(DamageDirection))
	{
		const FDMC_HitReactionData& Data = HitReactionMap[DamageDirection];
		
		BufferComponent->StopBuffer();
		BufferComponent->StartBuffer(Data.PushbackAmount);
		if (Data.HitReactMontage)
		{
			PlayAnimMontage(Data.HitReactMontage);
		}
	}
}

void ADMC_EnemyCharacterBase::Death(bool bIsFinisher)
{
	if (bDead) return;
	bDead = true;
	
	if (bIsFinisher)
	{
		// If finished, we don't play death montage and destroy immediately (or very soon)
		// We can also trigger VXF/SFX here if needed
		Destroy();
		return;
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	SetLifeSpan(5.0f);
}

void ADMC_EnemyCharacterBase::StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType)
{
	if (EquippedWeapon) EquippedWeapon->StartCollision(DamageType);
}

void ADMC_EnemyCharacterBase::EndWeaponCollision()
{
	if (EquippedWeapon) EquippedWeapon->EndCollision();
}

float ADMC_EnemyCharacterBase::PerformAttack(int32 AttackIndex)
{
	if (bDead || AttackMontages.Num() == 0 || bIsAttacking) return 0.f;

	int32 SelectedIndex = (AttackIndex >= 0 && AttackIndex < AttackMontages.Num()) 
		? AttackIndex 
		: FMath::RandRange(0, AttackMontages.Num() - 1);

	if (UAnimMontage* SelectedMontage = AttackMontages[SelectedIndex])
	{
		float Duration = PlayAnimMontage(SelectedMontage);
		if (Duration > 0.f)
		{
			bIsAttacking = true;
			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ADMC_EnemyCharacterBase::ResetAttackState, Duration, false);
			return Duration;
		}
	}
	return 0.f;
}

void ADMC_EnemyCharacterBase::ResetAttackState()
{
	bIsAttacking = false;
}

void ADMC_EnemyCharacterBase::EquipWeapon()
{
	if (!WeaponClass || EquippedWeapon) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	EquippedWeapon = GetWorld()->SpawnActor<ADMC_BaseWeapon>(WeaponClass, SpawnParams);
	if (EquippedWeapon)
	{
		EquippedWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
	}
}