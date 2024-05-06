// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreMinimal.h"

#include "Misc/AutomationTest.h"
#include "TimelineObject/TimelineObject.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTimelineObjectTests,
	"DreamCore.TimelineObject",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::HighPriority);

bool FTimelineObjectTests::RunTest(const FString& Parameters)
{
	const UWorld* World = nullptr;
	// if (GEditor && GEditor->GetWorldContexts().Num() && GEditor->GetWorldContexts()[0].World())
	// {
	// 	World = GEditor->GetWorldContexts()[0].World();
	// }
	if (GEngine && GEngine->GetWorldContexts().Num() && GEngine->GetWorldContexts()[0].World())
	{
		World =  GEngine->GetWorldContexts()[0].World();
	}
	// if (GEditor)
	// {
	// 	return FAutomationEditorCommonUtils::CreateNewMap();
	// }
	
	UCurveFloat* Curve = NewObject<UCurveFloat>();
	Curve->FloatCurve.AddKey(0.f, 0.f);
	Curve->FloatCurve.AddKey(1.f, 1.f);

	FTimelineObjectDelegate UpdateDelegate;
	FTimelineObjectDelegate FinishDelegate;
	
	UTimelineObject* TimelineObject = UTimelineObject::CreateTimeline(
		World, Curve,
		false, false,
		UpdateDelegate, FinishDelegate);
	
	TestTrue("Test if timeline was created", TimelineObject != nullptr);

	// Test changing direction
	{
		TimelineObject->SetNewDirection(ETimelineDirection::Type::Forward);
		TestTrueExpr(TimelineObject->GetDirection() == ETimelineDirection::Type::Forward);
		
		TimelineObject->SetNewDirection(ETimelineDirection::Type::Backward);
		TestTrueExpr(TimelineObject->GetDirection() == ETimelineDirection::Type::Backward);
		
		TimelineObject->SetNewDirection(ETimelineDirection::Type::Forward);
		TimelineObject->ChangeDirection();
		TestTrueExpr(TimelineObject->GetDirection() == ETimelineDirection::Type::Backward);
		
		TimelineObject->SetNewDirection(ETimelineDirection::Type::Backward);
		TimelineObject->ChangeDirection();
		TestTrueExpr(TimelineObject->GetDirection() == ETimelineDirection::Type::Forward);
	}
	
	return true;
}