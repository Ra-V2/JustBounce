/*
 * Copyright 2020, Rafal Bernacki <rav.bernacki@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "JustBounce.h"

#include <Alignment.h>
#include <LayoutBuilder.h>
#include <Slider.h>
#include <String.h>
#include <StringView.h>
#include <Window.h>

#define ARRAY_SIZE(a) \
	(sizeof(a) / sizeof(a[0]))

//informations
static const BString kName = "JustBounce";
static const BString kAuthor = "by RaV";

//black and white
static const rgb_color kBlack = {0, 0, 0, 255};
static const rgb_color kWhite = {255, 255, 255, 255};

static const char* kText = "HAIKU";

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
	fColor(GetColor())
{
}

JustBounce::~JustBounce()
{
}

void
JustBounce::StartConfig(BView* view)
{
	BWindow* win = view->Window();
	if(win)
		win->AddHandler(this);

	BStringView* v1 = new BStringView("name", kName);
	v1->SetFont(be_bold_font);
	BStringView* v2 = new BStringView("author", kAuthor);

	BLayoutBuilder::Group<>(view, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.SetExplicitAlignment(BAlignment(B_ALIGN_HORIZONTAL_CENTER, B_ALIGN_TOP))
		.AddGroup(B_HORIZONTAL)
			.Add(v1)
			.Add(v2)
			.AddGlue()
		.End();
}

status_t
JustBounce::StartSaver(BView* view, bool preview)
{
	srandom(time(NULL));

	if(preview) {
		fTextStartX = 20;
		fTextStartY = 30;
	}

	fViewX = view->Bounds().Width();
	fViewY = view->Bounds().Height();

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
	BRect rect;
	font.GetBoundingBoxesForStrings(&kText, 1, B_SCREEN_METRIC, &delta, &rect);
	fTextWidth = rect.Width();
	fTextHeight = rect.Height();

	//set left bottom
	fTextStartX = random() % (fViewX - fTextWidth);
	fTextStartY = random() % (fViewY - fTextHeight) + fTextHeight;

	//set right top
	fTextEndX = fTextStartX + fTextWidth;
	fTextEndY = fTextStartY - fTextHeight;

	SetTickSize(4000);

	return B_OK;
}

void
JustBounce::Draw(BView* view, int32 frame)
{
	if (frame == 0) {
		view->SetDrawingMode(B_OP_ALPHA);
		view->SetLowColor(kBlack);
		view->FillRect(view->Bounds(), B_SOLID_LOW);
	} else
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
	view->DrawString(kText, BPoint(fTextStartX, fTextStartY));
}

//pick next color
rgb_color
JustBounce::GetColor() {
	rgb_color nextColor;
	static const rgb_color colors[] = {
		{255, 255, 255, 255},
		{255, 0, 0, 255},
		{0, 0, 255, 255},
		{0, 255, 0, 255},
		{255, 255, 0, 255},
		{0, 255, 255, 255},
		{255, 0, 255, 255}
	};
	
	do {
		nextColor = colors[random() % ARRAY_SIZE(colors)];
	} while (fColor == nextColor);
	
	return nextColor;
}
