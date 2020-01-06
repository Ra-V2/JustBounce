/*
 * Copyright 2020, Rafal Bernacki <rav.bernacki@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "JustBounce.h"

#include <stdlib.h>

#include <Alignment.h>
#include <Alert.h>
#include <LayoutBuilder.h>
#include <Slider.h>
#include <String.h>
#include <StringView.h>
#include <Window.h>

#define ARRAY_SIZE(a) \
	(sizeof(a) / sizeof(a[0]))

static const int32 kChangeSpeed = 'chgs';
static const int32 kChangeText = 'chgt';

//informations
static const BString kName = "JustBounce";
static const BString kAuthor = "by RaV";

//black
static const rgb_color kBlack = {0, 0, 0, 255};

extern "C" BScreenSaver*
instantiate_screen_saver(BMessage* msg, image_id id)
{
	return new JustBounce(msg, id);
}

JustBounce::JustBounce(BMessage* archive, image_id id)
	:
	BScreenSaver(archive, id),
	fTextChangeX(1),
	fTextChangeY(1),
	fSpeed(1000),
	fColor(GetColor()),
	fRestart(false),
	fPreview(false),
	fText("HAIKU")
{
	fSpeed = archive->GetInt32("fSpeed", fSpeed);
	fText = archive->GetString("fText", fText);
}

JustBounce::~JustBounce()
{
}

void
JustBounce::StartConfig(BView* view)
{
	BWindow* win = view->Window();
	if (win)
		win->AddHandler(this);

	BStringView* v1 = new BStringView("name", kName);
	v1->SetFont(be_bold_font);
	BStringView* v2 = new BStringView("author", kAuthor);
	
	fTextControl = new BTextControl("text", "Text", "", NULL);
	fTextControl->SetText(fText);
	fTextControl->SetTarget(this);
	fTextControl->SetModificationMessage(new BMessage(kChangeText));
	
	BSlider* s1;
	
	s1 = new BSlider("speed", "Speed", new BMessage(kChangeSpeed), 500, 5000, B_HORIZONTAL);
	s1->SetValue(fSpeed);
	s1->SetTarget(this);
	s1->SetLimitLabels("Slow", "Fast");

	BLayoutBuilder::Group<>(view, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.SetExplicitAlignment(BAlignment(B_ALIGN_HORIZONTAL_CENTER, B_ALIGN_TOP))
		.AddGroup(B_HORIZONTAL)
			.Add(v1)
			.Add(v2)
			.AddGlue()
			.End()
		.Add(fTextControl)
		.Add(s1)
		.AddGlue()
		.End();
}

status_t
JustBounce::StartSaver(BView* view, bool preview)
{
	srandom(time(NULL));

	if (preview) {
		fTextStartX = 20;
		fTextStartY = 30;
		fPreview = true;
	}
	else
		fPreview = false;

	fViewX = view->Bounds().Width();
	fViewY = view->Bounds().Height();

	Restart(view);
	
	return B_OK;
}

void
JustBounce::Draw(BView* view, int32 frame)
{
	if (frame == 0 || fRestart) {
		Restart(view);
	}
	
	view->FillRect(view->Bounds(), B_SOLID_LOW);
	
	//change postition
	fTextStartX += fTextChangeX;
	fTextStartY += fTextChangeY;

	fTextEndX += fTextChangeX;
	fTextEndY += fTextChangeY;

	//check borders
	if (fTextStartX <= view->Bounds().left
		|| fTextEndX >= view->Bounds().right) {

		fTextChangeX = -fTextChangeX;
		fColor = GetColor();
	}

	if (fTextEndY <= view->Bounds().top
		|| fTextStartY >= view->Bounds().bottom) {

		fTextChangeY = -fTextChangeY;
		fColor = GetColor();
	}

	//draw
	view->SetHighColor(fColor);
	view->DrawString(fText, BPoint(fTextStartX, fTextStartY));
}

void
JustBounce::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case kChangeSpeed:
		fSpeed = msg->GetInt32("be:value", fSpeed);
		fRestart = true;
		break;
	case kChangeText:
		ChangeText();
		fRestart = true;
	default:
		BHandler::MessageReceived(msg);
	}
}

status_t
JustBounce::SaveState(BMessage* into) const
{
	into->AddInt32("fSpeed", fSpeed);
	into->AddString("fText", fText);
	return B_OK;
}

void
JustBounce::Restart(BView* view)
{
	fRestart = false;
	
	if (fPreview)
		SetTickSize((5500 - fSpeed) * 10);
	else
		SetTickSize(5500 - fSpeed);
	
	//set font
	BFont font;
	view->GetFont(&font);
	font.SetSize(fViewY / 10);
	font.SetFace(B_BOLD_FACE);
	view->SetFont(&font);

	escapement_delta delta;
	delta.nonspace = 0;
	delta.space = 0;

	//get width and height
	const char* text = fText;
	BRect rect;
	font.GetBoundingBoxesForStrings(&text, 1, B_SCREEN_METRIC, &delta, &rect);
	fTextWidth = rect.Width();
	fTextHeight = rect.Height();

	//set left bottom
	fTextStartX = random() % (fViewX - fTextWidth);
	fTextStartY = random() % (fViewY - fTextHeight) + fTextHeight;

	//set right top
	fTextEndX = fTextStartX + fTextWidth;
	fTextEndY = fTextStartY - fTextHeight;

	view->SetDrawingMode(B_OP_ALPHA);
	view->SetLowColor(kBlack);
	view->FillRect(view->Bounds(), B_SOLID_LOW);
}

void
JustBounce::ChangeText()
{
	BString text = fTextControl->Text();
	int textLength = text.Length();
	if (textLength <= 15) {
		fText = text;
	}
}

//pick next color
rgb_color
JustBounce::GetColor() {
	rgb_color nextColor;
	static const rgb_color colors[] = {
		{0, 116, 217, 255},
		{127, 219, 255, 255},
		{57, 204, 204, 255},
		{61, 153, 112, 255},
		{46, 204, 64, 255},
		{1, 255, 112, 255},
		{255, 220, 0, 255},
		{255, 113, 27, 255},
		{255, 65, 54, 255},
		{133, 20, 75, 255},
		{177, 13, 201, 255},
		{221, 221, 221, 255},
	};
	
	do {
		nextColor = colors[random() % ARRAY_SIZE(colors)];
	} while (fColor == nextColor);
	
	return nextColor;
}
