// Fill out your copyright notice in the Description page of Project Settings.


#include "GetPayloadTypeNode.h"
#include "EdGraphSchema_K2.h"
#include "DCMessageBus/Public/MessageBusTypes.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "DCMessageBus/Public/DefaultAttributePayloads.h"
#include "EdGraphCompilerUtilities.h"

#define LOCTEXT_NAMESPACE "K2Node_GetPayloadTypeNode"

namespace GetPayloadTypeNodeHelper
{

	const FName PayloadPinName = "PayloadContainer";
	const FName PayloadTypeName = "Type";
	const FName ResultPinName = "Payload";
	const FName ErrorPinName = "Error";
}

UK2Node_GetPayloadTypeNode::UK2Node_GetPayloadTypeNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Attempts to retrieve a Payload by type from container");
}

void UK2Node_GetPayloadTypeNode::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	RowFoundPin->PinFriendlyName = LOCTEXT("Failed to convert Payload", "Got Payload");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetPayloadTypeNodeHelper::ErrorPinName);

	// Add Container pin
	UEdGraphPin* PayloadContainerIn = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FMessagePayloadContainer::StaticStruct(), GetPayloadTypeNodeHelper::PayloadPinName);
	SetPinToolTip(*PayloadContainerIn, LOCTEXT("PayloadPinDescription", "Payload for convert"));

	// Payload Type
	UEdGraphPin* PayloadTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UScriptStruct::StaticClass(), GetPayloadTypeNodeHelper::PayloadTypeName);
	SetPinToolTip(*PayloadTypePin, LOCTEXT("TypePinDescription", "Type to convert payload to"));

	// Result pin
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	ResultPin->PinFriendlyName = LOCTEXT("Converted Payload", "Out");
	SetPinToolTip(*ResultPin, LOCTEXT("ResultPinDescription", "The returned converted Payload, if succeeded"));

	Super::AllocateDefaultPins();
}

FSlateIcon UK2Node_GetPayloadTypeNode::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon();
}

void UK2Node_GetPayloadTypeNode::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshOutputPinType();
}

FText UK2Node_GetPayloadTypeNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Get Payload Type");
	}
	else if (UEdGraphPin* DataTablePin = GetStructPin())
	{
		if (DataTablePin->LinkedTo.Num() > 0)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_Unknown", "Get Payload Type");
		}
		else if (DataTablePin->DefaultObject == nullptr)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_None", "Get Payload Type NONE");
		}
		else if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("Type"), FText::FromString(DataTablePin->DefaultObject->GetName()));

			FText LocFormat = NSLOCTEXT("K2Node", "DataTable", "Get Payload Type {Type}");
			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitle.SetCachedText(FText::Format(LocFormat, Args), this);
		}
	}
	else
	{
		return NSLOCTEXT("K2Node", "DataTable_Title_None", "Get Data Table Row NONE");
	}
	return CachedNodeTitle;
}

void UK2Node_GetPayloadTypeNode::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin && ChangedPin->PinName == GetPayloadTypeNodeHelper::PayloadTypeName)
	{
		RefreshOutputPinType();
	}
}


void UK2Node_GetPayloadTypeNode::SetReturnTypeForStruct(UScriptStruct* NewRowStruct)
{
	UScriptStruct* OldRowStruct = (UScriptStruct*)(GetResultPin()->PinType.PinSubCategoryObject.Get());
	if (NewRowStruct != OldRowStruct)
	{
		UEdGraphPin* ResultPin = GetResultPin();

		if (ResultPin->SubPins.Num() > 0)
		{
			GetSchema()->RecombinePin(ResultPin);
		}

		// NOTE: purposefully not disconnecting the ResultPin (even though it changed type)... we want the user to see the old
		//       connections, and incompatible connections will produce an error (plus, some super-struct connections may still be valid)
		ResultPin->PinType.PinSubCategoryObject = NewRowStruct;
		ResultPin->PinType.PinCategory = (NewRowStruct == nullptr) ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;

		CachedNodeTitle.Clear();
	}
}

void UK2Node_GetPayloadTypeNode::RefreshOutputPinType()
{
	UScriptStruct* OutputType = GetInputStruct();
	SetReturnTypeForStruct(OutputType);
}

UScriptStruct* UK2Node_GetPayloadTypeNode::GetInputStruct() const
{
	UScriptStruct* StructType = nullptr;

	if (const UEdGraphPin* TypePin = GetStructPin())
	{
		StructType =  Cast<UScriptStruct>(TypePin->DefaultObject);
	}

	return StructType;
}

UEdGraphPin* UK2Node_GetPayloadTypeNode::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetPayloadTypeNode::GetErrorPin() const
{
	UEdGraphPin* Pin = FindPinChecked(GetPayloadTypeNodeHelper::ErrorPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_GetPayloadTypeNode::GetResultPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

void UK2Node_GetPayloadTypeNode::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

UEdGraphPin* UK2Node_GetPayloadTypeNode::GetStructPin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == GetPayloadTypeNodeHelper::PayloadTypeName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}


void UK2Node_GetPayloadTypeNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	const UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_GetPayloadTypeNode::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
}

UEdGraphPin* UK2Node_GetPayloadTypeNode::GetStructDataPin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == GetPayloadTypeNodeHelper::PayloadPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

void UK2Node_GetPayloadTypeNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode( CompilerContext, SourceGraph );
	
	UEdGraphPin* OriginalStructInPin = GetStructDataPin();
	if(nullptr == OriginalStructInPin)// || nullptr == OriginalStructInPin->DefaultObject
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("GetPayloadTypeNoStruct_Error", "GetPayloadTypeNode must have a type specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UDefaultPayloadsLibrary, GetPayloadFromPayloadContainer);
	UK2Node_CallFunction* GetPayloadFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	GetPayloadFunction->FunctionReference.SetExternalMember(FunctionName, UDefaultPayloadsLibrary::StaticClass());
	GetPayloadFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(GetPayloadFunction->GetExecPin()));

	// Connect the input of our GetDataTableRow to the Input of our Function pin
	UEdGraphPin* PailoadContainerInPin = GetPayloadFunction->FindPinChecked(TEXT("Container"));
	if(OriginalStructInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalStructInPin, *PailoadContainerInPin);
	}
	else
	{
		// Copy literal
		PailoadContainerInPin->DefaultObject = OriginalStructInPin->DefaultObject;
	}
	
	// Get some pins to work with
	UEdGraphPin* OriginalOutRowPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowPin = GetPayloadFunction->FindPinChecked(TEXT("Result"));
	UEdGraphPin* FunctionReturnPin = GetPayloadFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = GetPayloadFunction->GetThenPin();

	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	//BRANCH NODE
	UK2Node_IfThenElse* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	BranchNode->AllocateDefaultPins();
	// Hook up inputs to branch
	FunctionThenPin->MakeLinkTo(BranchNode->GetExecPin());
	FunctionReturnPin->MakeLinkTo(BranchNode->GetConditionPin());

	// Hook up outputs
	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *(BranchNode->GetThenPin()));
	CompilerContext.MovePinLinksToIntermediate(*GetErrorPin(), *(BranchNode->GetElsePin()));
	CompilerContext.MovePinLinksToIntermediate(*OriginalOutRowPin, *FunctionOutRowPin);
	
	BreakAllNodeLinks( );
}

#undef LOCTEXT_NAMESPACE
