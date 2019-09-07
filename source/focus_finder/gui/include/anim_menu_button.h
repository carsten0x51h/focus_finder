/*
 * customtoolbutton.h
 *
 *  Created on: Apr 11, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_ANIM_MENU_BUTTON_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_ANIM_MENU_BUTTON_H_

#include <QToolButton>
#include <QDebug>
#include <QMovie>
#include <QIcon>

class AnimMenuButtonT : public QToolButton
{
    Q_OBJECT
public:
    explicit AnimMenuButtonT(QWidget *parent = 0);

    void startAnimation();
    void stopAnimation();


public slots:
   // member function that catches the frameChanged signal of the QMovie
   void setButtonIcon(int frame);


private:
   void toggleIfDefault(QAction* a);

   QMovie * mMovie;
   QIcon mOriginalIcon;
};

#endif /* SOURCE_FOCUS_FINDER_GUI_INCLUDE_ANIM_MENU_BUTTON_H_ */
