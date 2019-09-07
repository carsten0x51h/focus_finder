#include <iostream>

#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QDebug>

#include "include/anim_menu_button.h"

void AnimMenuButtonT::startAnimation() {
	mOriginalIcon = icon();
	mMovie->start();
}

void AnimMenuButtonT::stopAnimation() {
	mMovie->stop();
	setIcon(mOriginalIcon);
}

void AnimMenuButtonT::toggleIfDefault(QAction* a) {
	std::cerr << "a: " << a << ", defaultAction(): " << defaultAction() << std::endl;

	if (a == defaultAction()) {
		toggle();
	}
}

AnimMenuButtonT::AnimMenuButtonT(QWidget *parent) :
    QToolButton(parent),
	mMovie(nullptr)
{
    // Enable code below to set selected item as default
    //QObject::connect(this, SIGNAL(triggered(QAction*)),
    //                 this, SLOT(setDefaultAction(QAction*)));

    mMovie = new QMovie(QStringLiteral(":/res/wait3.gif"));

    connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(setButtonIcon(int)));

    // if movie doesn't loop forever, force it to.
    // TODO: Update to new signal-slot API...
    if (mMovie->loopCount() != -1) {
    	connect(mMovie, SIGNAL(finished()), mMovie, SLOT(start()));
    }

    QObject::connect(this, & QToolButton::triggered,
    				 this, & AnimMenuButtonT::toggleIfDefault);
}


// member function that catches the frameChanged signal of the QMovie
void AnimMenuButtonT::setButtonIcon(int /*frame*/)
{
    this->setIcon(QIcon(mMovie->currentPixmap()));
}
