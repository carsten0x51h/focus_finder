/*****************************************************************************
 *
 *  FoFi - A free, automatic telescope focus finder.
 *
 *  Copyright(C) 2019 Carsten Schmitt <c.schmitt51h@gmail.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

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

void AnimMenuButtonT::toggleIfDefault(QAction *a) {
    std::cerr << "a: " << a << ", defaultAction(): " << defaultAction() << std::endl;

    if (a == defaultAction()) {
        toggle();
    }
}

AnimMenuButtonT::AnimMenuButtonT(QWidget *parent) :
        QToolButton(parent),
        mMovie(nullptr) {
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

    QObject::connect(this, &QToolButton::triggered,
                     this, &AnimMenuButtonT::toggleIfDefault);
}

AnimMenuButtonT::~AnimMenuButtonT() {
    delete mMovie;
}


// member function that catches the frameChanged signal of the QMovie
void AnimMenuButtonT::setButtonIcon(int /*frame*/) {
    this->setIcon(QIcon(mMovie->currentPixmap()));
}
