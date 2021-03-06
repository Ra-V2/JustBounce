/*
 * Copyright 2020, Rafal Bernacki <rav.bernacki@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef Just_Bounce_H
#define Just_Bounce_H

#include <Handler.h>
#include <ScreenSaver.h>
#include <ObjectList.h>
#include <String.h>
#include <TextControl.h>

class JustBounce : public BScreenSaver, public BHandler {
public:
						JustBounce(BMessage* archive, image_id id);
						~JustBounce();

	virtual void		StartConfig(BView* view);

	virtual status_t	StartSaver(BView* view, bool preview);
	virtual void		Draw(BView* view, int32 frame);
	
	virtual void		MessageReceived(BMessage* msg);
	virtual status_t	SaveState(BMessage* into) const;

private:
	rgb_color					GetColor();
	void						Restart(BView* view);
	void						ChangeText();
	
	int						fTextStartX;
	int						fTextStartY;
	
	int						fTextEndX;
	int						fTextEndY;
	
	int						fTextChangeX;
	int						fTextChangeY;
	
	int						fTextHeight;
	int						fTextWidth;
	
	int						fViewX;
	int						fViewY;

	int						fSpeed;
	rgb_color					fColor;
	
	bool						fRestart;
	bool						fPreview;
	
	BString						fText;
	BTextControl*					fTextControl;
	
};

#endif
