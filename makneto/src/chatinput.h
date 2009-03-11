/*
 * chatinput.h
 *
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
 */

#ifndef CHATINPUT_H
#define CHATINPUT_H

#include <QtGui/QTextEdit>

class ChatInput: public QTextEdit
{
	Q_OBJECT

public:
	ChatInput(QWidget *parent = 0);
	~ChatInput() {};
	
protected:
	void keyPressEvent(QKeyEvent *event);
	
signals:
        void send();
};

#endif // CHATINPUT_H
