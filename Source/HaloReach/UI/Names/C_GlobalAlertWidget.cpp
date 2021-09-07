// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Names/C_GlobalAlertWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

UC_GlobalAlertWidget::UC_GlobalAlertWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_GlobalAlertWidget::NativeConstruct()
{
	// Black fade in
	StoreWidgetAnimation();

	FadeInAnimation = GetAnimationByName(TEXT("FadeIn"));
	FadeOutAnimation = GetAnimationByName(TEXT("FadeOut"));

	AlertText->SetRenderOpacity(0.0f);

	PlayFadeInAnimation();

	EndDelegate.BindDynamic(this, &UC_GlobalAlertWidget::AnimationFinished);
	BindToAnimationFinished(FadeOutAnimation, EndDelegate);

	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &UC_GlobalAlertWidget::DestroyWidget, 3.0f, false);
}

void UC_GlobalAlertWidget::SetAlertText(const FString UpdatedAlertText)
{
	AlertText->SetText(FText::FromString(UpdatedAlertText));
}

# pragma region Widget Animation

void UC_GlobalAlertWidget::StoreWidgetAnimation()
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

UWidgetAnimation* UC_GlobalAlertWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);

	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}

	return nullptr;
}

void UC_GlobalAlertWidget::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UC_GlobalAlertWidget::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

# pragma endregion

void UC_GlobalAlertWidget::DestroyWidget()
{
	PlayFadeOutAnimation();
}

void UC_GlobalAlertWidget::AnimationFinished()
{
	RemoveFromParent();
}
