// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Radar/C_RadarIcon.h"
#include "Components/Throbber.h"
#include "Kismet/GameplayStatics.h"
#include "Templates/Tuple.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/UI/Radar/C_Radar.h"
#include "HaloReach/Components/C_RadarIconComponent.h"

UC_RadarIcon::UC_RadarIcon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_RadarIcon::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimation();

	FadeInAnimation = GetAnimationByName(TEXT("FadeIn"));
	FadeOutAnimation = GetAnimationByName(TEXT("FadeOut"));
}

void UC_RadarIcon::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
}

FVector2D UC_RadarIcon::GetPOILocation(UC_Radar* MiniMap)
{
	float rety = Returny();
	float retx = Returnx();

	float retz = MiniMap->CalculateZoom();

	float compx = retx / retz;

	float compy = rety / retz;

	FVector2D ReturnVector2D = FVector2D(compx, compy);

	float Magnitude = ReturnVector2D.Size();

	float Theeta = FindTheeta(FVector2D(0.0f, 0.0f), ReturnVector2D);

	FVector2D Cord = FindCoordinate(Magnitude, Theeta);

	SetRenderTranslation(Cord);

	// When icon is no longer on the minimap, make it hidden. mapsize = 300, 130 || mapsize = 200, 100
	ReturnVector2D.Size() >= 100.0f ? IconImage->SetVisibility(ESlateVisibility::Hidden) : IconImage->SetVisibility(ESlateVisibility::Visible); // Default was 130.0f

	return ReturnVector2D;
}

void UC_RadarIcon::UpdateIconImage(UC_RadarIconComponent* POIComponent)
{
	if (POIComponent && POIComponent->RadarIconImage)
	{
		IconImage->SetBrushFromTexture(POIComponent->RadarIconImage);
		DefaultImage->SetVisibility(ESlateVisibility::Hidden);
	}

	else
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UC_RadarIcon::StoreWidgetAnimation()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	while (Prop)
	{
		// only evaluate object properties, skip rest
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// only get back properties that are of type widget animation
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				// only get back properties that are of type widget animation
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);
				// if casting worked update map with new animation
				if (WidgetAnimation && WidgetAnimation->MovieScene)
				{
					FName AnimName = WidgetAnimation->MovieScene->GetFName();
					//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, AnimName.ToString());
					AnimationsMap.Add(AnimName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UC_RadarIcon::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);

	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}

	return nullptr;
}

void UC_RadarIcon::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UC_RadarIcon::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UC_RadarIcon::StopFadeOutAnimation()
{
	if(IsAnimationPlaying(FadeOutAnimation))
	{
		StopAnimation(FadeOutAnimation);
	}
}

float UC_RadarIcon::FindTheeta(FVector2D PointA, FVector2D PointB)
{
	float X = PointA.X - PointB.X;
	float Y = PointA.Y - PointB.Y;

	float InverseTan = UKismetMathLibrary::DegAtan2(Y, X);

	return InverseTan;
}

FVector2D UC_RadarIcon::FindCoordinate(float Radius, float Degress)
{
	DegCosY = UKismetMathLibrary::DegCos(Degress);
	DegCosY *= Radius;
	DegCosY *= -1.0f;

	DegSinX = UKismetMathLibrary::DegSin(Degress);
	DegSinX *= Radius;
	DegSinX *= -1.0f;

	FVector2D Param = FVector2D(DegSinX, DegCosY);

	return Param;
}

TTuple<float, float> UC_RadarIcon::ActorLocation()
{
	if (UGameplayStatics::GetPlayerCharacter(this, 0) && Owner)
	{
		float PlayerX = UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation().X;
		float PlayerY = UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation().Y;


		float OwnerX = Owner->GetActorLocation().X;
		float OwnerY = Owner->GetActorLocation().Y;


		float Totalx = PlayerX - OwnerX;
		float Totaly = (PlayerY - OwnerY) * -1.0f;

		return MakeTuple(Totalx, Totaly);
	}

	return MakeTuple(0.0f, 0.0f);
}

float UC_RadarIcon::Returnx()
{
	TTuple<float, float>Getx = ActorLocation();
	return Getx.Get<0>();
}

float UC_RadarIcon::Returny()
{
	TTuple<float, float>Gety = ActorLocation();
	return Gety.Get<1>();
}
