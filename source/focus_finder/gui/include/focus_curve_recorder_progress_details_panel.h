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

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QHBoxLayout>
#include <QProgressBar>

class FocusCurveRecorderLogicT;

class FocusCurveRecordT;

class HfdViewPanelT;

namespace Ui {
    class FocusCurveRecorderProgressDetailsPanel;
}

/**
*  @brief
*/
class FocusCurveRecorderProgressDetailsPanelT : public QWidget {
Q_OBJECT

public:
    /**
     *  @brief
     *    Constructor
     */
    FocusCurveRecorderProgressDetailsPanelT(QWidget *parent,
                                            std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic);

    /**
     *  @brief
     *    Destructor
     */
    virtual ~FocusCurveRecorderProgressDetailsPanelT();

    void reset();

    void setIteration(size_t currentIteration, size_t numTotalIterations);

    void setCurrentIterationProgress(int progressPerc);

    void setCurrentIterationProgressText(const QString &currentProgressText);

    void setTotalProgress(int progressPerc);

    void setTotalProgressText(const QString &totalProgressText);

    void setCurrentFocusCurveRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord);

    void startAnimation();

    void stopAnimation();

public slots:

    // member function that catches the frameChanged signal of the QMovie
    void setButtonIcon(int frame);

protected slots:

protected:
    const QScopedPointer<Ui::FocusCurveRecorderProgressDetailsPanel> m_ui;

private:
    void setProgress(QProgressBar *progressBar, int progressPerc);

    std::shared_ptr<FocusCurveRecorderLogicT> mFocusCurveRecorderLogic;
    QMovie *mMovie;
    HfdViewPanelT *mHfdViewPanel;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_FOCUS_CURVE_RECORDER_PROGRESS_DETAILS_PANEL_H_*/
