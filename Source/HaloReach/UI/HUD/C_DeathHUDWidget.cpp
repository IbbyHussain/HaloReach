// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_DeathHUDWidget.h"
#include "Components/Image.h"


UC_DeathHUDWidget::UC_DeathHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_DeathHUDWidget::NativeConstruct()
{
	// Black fade in
	StoreWidgetAnimation();

	FadeInAnimation = GetAnimationByName(TEXT("FadeIn"));
	FadeOutAnimation = GetAnimationByName(TEXT("FadeOut"));

	BlackImage->SetRenderOpacity(0.0f);
}

# pragma region Black Fade In Animation

void UC_DeathHUDWidget::StoreWidgetAnimation()
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
					GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, AnimName.ToString());
					AnimationsMap.Add(AnimName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UC_DeathHUDWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);

	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}

	return nullptr;
}

void UC_DeathHUDWidget::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UC_DeathHUDWidget::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

# pragma endregion