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

#include <memory>

#include <QWidget>
#include <QMovie>

#include "include/focus_curve_recorder_progress_details_panel.h"
#include "include/hfd_view_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_recorder_logic.h"
#include "../common/include/focus_curve_record.h"

#include "ui_focus_curve_recorder_progress_details_panel.h"

FocusCurveRecorderProgressDetailsPanelT::FocusCurveRecorderProgressDetailsPanelT(QWidget * parent, std::shared_ptr<FocusCurveRecorderLogicT> focusCurveRecorderLogic) : QWidget(parent),
                                                                                        m_ui(new Ui::FocusCurveRecorderProgressDetailsPanel),
																					mFocusCurveRecorderLogic(focusCurveRecorderLogic),
	mMovie(nullptr)
{
    // Setup UI
    m_ui->setupUi(this);

//    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(100);
//    sizePolicy.setVerticalStretch(100);
//
//    mFocusCurveWidget = new FocusCurveViewWidgetT(m_ui->widget);
//    mFocusCurveWidget->setSizePolicy(sizePolicy);
//    m_ui->layFocusCurveViewWidget->addWidget(mFocusCurveWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    // Add the focus curve viewer panel
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(100);
    sizePolicy.setVerticalStretch(100);

    // mFocusCurveViewPanel = new FocusCurveViewPanelT(m_ui->widget, mFocusCurveRecorderLogic);
    // mFocusCurveViewPanel->setSizePolicy(sizePolicy);
    // m_ui->layFocusCurveViewPanel->addWidget(mFocusCurveViewPanel, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    mMovie = new QMovie(QStringLiteral(":/res/telescope_focusing.gif"));

    connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(setButtonIcon(int)));

    // if movie doesn't loop forever, force it to.
    // TODO: Update to new signal-slot API...
    if (mMovie->loopCount() != -1) {
    	connect(mMovie, SIGNAL(finished()), mMovie, SLOT(start()));
    }


    // Add focus measure panel
    // TODO: Not only HFD in the future.... displayed view depends on selected focus measure...
    mHfdViewPanel = new HfdViewPanelT(m_ui->widgetFocusMeasure);
    
    
    reset();
}

FocusCurveRecorderProgressDetailsPanelT::~FocusCurveRecorderProgressDetailsPanelT()
{
  delete mMovie;
}

void FocusCurveRecorderProgressDetailsPanelT::reset()
{
  m_ui->grpProgressIteration->setTitle(QStringLiteral("Progress iteration"));
  m_ui->pgbCurrentIterationProgress->setValue(0);
  m_ui->lblCurrentIterationProgressText->setText(QStringLiteral("Idle."));
  m_ui->pgbTotalProgress->setValue(0);
  m_ui->lblTotalProgressText->setText(QStringLiteral("Idle."));
  //  m_ui->lblFocusingAnimation->clear();
}

void FocusCurveRecorderProgressDetailsPanelT::setIteration(size_t currentIteration, size_t numTotalIterations)
{
  std::stringstream ss;
  ss << "Progress iteration " << currentIteration << "/" << numTotalIterations;
  m_ui->grpProgressIteration->setTitle(QString::fromStdString(ss.str()));
}

void FocusCurveRecorderProgressDetailsPanelT::setProgress(QProgressBar * progressBar, int progressPerc) {
  if (progressPerc == -1) {
    // If there is no progress info, just display a "busy" animation
    // See: https://forum.qt.io/topic/53791/stoping-qprogressbar-animation/2
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setValue(0);
  } else {
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(progressPerc);
  }
}

void FocusCurveRecorderProgressDetailsPanelT::setCurrentIterationProgress(int progressPerc)
{
  setProgress(m_ui->pgbCurrentIterationProgress, progressPerc);
}

void FocusCurveRecorderProgressDetailsPanelT::setCurrentIterationProgressText(const QString & currentProgressText)
{
  m_ui->lblCurrentIterationProgressText->setText(currentProgressText);
}

void FocusCurveRecorderProgressDetailsPanelT::setTotalProgress(int progressPerc)
{
  setProgress(m_ui->pgbTotalProgress, progressPerc);
}

void FocusCurveRecorderProgressDetailsPanelT::setTotalProgressText(const QString & totalProgressText)
{
  m_ui->lblTotalProgressText->setText(totalProgressText);
}


// QObject::connect: No such slot FocusCurveRecorderProgressDetailsPanelT::setButtonIcon(int) in /home/devnull/workspace/fofi_git/fofi/source/focus_finder/gui/focus_curve_recorder_progress_details_panel.cpp:40
// QObject::connect:  (receiver name: 'FocusCurveRecorderProgressDetailsPanel')
// [2019-11-06 14:54:30.130344]: FocusCurveRecorderProgressDetailsPanelT::startAnimation...


void FocusCurveRecorderProgressDetailsPanelT::startAnimation()
{
  LOG(debug) << "FocusCurveRecorderProgressDetailsPanelT::startAnimation..." << std::endl;
  mMovie->start();
}

void FocusCurveRecorderProgressDetailsPanelT::stopAnimation()
{
  LOG(debug) << "FocusCurveRecorderProgressDetailsPanelT::stopAnimation..." << std::endl;
  mMovie->stop();
}

void FocusCurveRecorderProgressDetailsPanelT::setButtonIcon(int /*frame*/)
{
  m_ui->lblFocusingAnimation->setPixmap(mMovie->currentPixmap());
}

// TODO
void FocusCurveRecorderProgressDetailsPanelT::setCurrentFocusCurveRecord(std::shared_ptr<FocusCurveRecordT> focusCurveRecord)
{
  // For FWHM, show both FWHMs and the star image. For HFD just show the "all-in-one HFD" panel...
  // A representation which combines all would be a 3D rep.
  // TODO: Not only HFD in the future.... displayed view depends on selected focus measure...
  if (focusCurveRecord != nullptr) {
    mHfdViewPanel->setHfd(focusCurveRecord->getHfd());
  }
}

