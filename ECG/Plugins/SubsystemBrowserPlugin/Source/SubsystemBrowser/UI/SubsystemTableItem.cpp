// Copyright 2022, Aquanox.

#include "UI/SubsystemTableItem.h"
#include "Model/SubsystemBrowserModel.h"
#include "UI/SubsystemBrowserPanel.h"
#include "UI/SubsystemTableItemTooltip.h"
#include "SlateOptMacros.h"
#include "SubsystemBrowserFlags.h"
#include "SubsystemBrowserSettings.h"
#include "SubsystemBrowserStyle.h"

#define LOCTEXT_NAMESPACE "SubsystemBrowser"

void SSubsystemTableItem::Construct(const FArguments& InArgs, TSharedRef<STableViewBase> OwnerTableView)
{
	Model = InArgs._InModel;
	Item = InArgs._InItemModel;
	Browser = InArgs._InBrowser;
	IsItemExpanded = InArgs._IsItemExpanded;
	HighlightText = InArgs._HighlightText;

	SetToolTip(SNew(SSubsystemTableItemTooltip).SubsystemTableItem(SharedThis(this)));

	FSuperRowType::FArguments Args = FSuperRowType::FArguments();
	Super::Construct(Args, OwnerTableView);
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
TSharedRef<SWidget> SSubsystemTableItem::GenerateWidgetForColumn(const FName& ColumnID)
{
	if (!Item.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	TSharedPtr<SWidget> TableRowContent = SNullWidget::NullWidget;

	SubsystemColumnPtr Column = Model->FindTableColumn(ColumnID);
	if (Column.IsValid() && Model->ShouldShowColumn(Column) )
	{
		TableRowContent = Column->GenerateColumnWidget(Item.ToSharedRef(), SharedThis(this));
	}

	return TableRowContent.ToSharedRef();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

const FSlateBrush* SSubsystemTableItem::GetItemIconBrush() const
{
#if SINCE_UE_VERSION(5, 0, 0)
	static const FName FolderOpenName(TEXT("Icons.FolderOpen"));
	static const FName FolderClosedName(TEXT("Icons.FolderClosed"));
#else
	static const FName FolderOpenName(TEXT("WorldBrowser.FolderOpen"));
	static const FName FolderClosedName(TEXT("WorldBrowser.FolderClosed"));
#endif

	if (Item.IsValid() && Item->CanHaveChildren())
	{
		if (Item->GetNumChildren() > 0 && Item->bExpanded)
		{
			return FStyleHelper::GetBrush(FolderOpenName);
		}

		return FStyleHelper::GetBrush(FolderClosedName);
	}

	return nullptr;
}

bool SSubsystemTableItem::IsColoringEnabled() const
{
	return USubsystemBrowserSettings::Get()->IsColoringEnabled();
}

#undef LOCTEXT_NAMESPACE
