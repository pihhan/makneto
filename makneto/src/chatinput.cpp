/*
 * chatinput.cpp
 *
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
 */

#include "chatinput.h"

#include <Qt>
#include <QKeyEvent>

ChatInput::ChatInput(QWidget *parent)
{
	setFocusPolicy(Qt::StrongFocus);
}

void ChatInput::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Return:
		if (event->modifiers()==Qt::ControlModifier)
			QTextEdit::keyPressEvent(event);
		else
			emit send();
		break;
	default:
		QTextEdit::keyPressEvent(event);
	
	}	
}
