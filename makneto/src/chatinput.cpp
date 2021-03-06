/*
 * chatinput.cpp
 *
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
 */

#include "chatinput.h"

#include <Qt>
#include <QKeyEvent>

ChatInput::ChatInput(QWidget *parent)
	: QTextEdit(parent)
{
	setFocusPolicy(Qt::StrongFocus);
    setAcceptRichText(false);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
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
