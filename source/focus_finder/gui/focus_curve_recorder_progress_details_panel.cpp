#include <memory>

#include <QWidget>
#include <QMovie>

#include "include/focus_curve_recorder_progress_details_panel.h"

#include "../common/include/logging.h"
#include "../common/include/focus_curve_recorder_logic.h"

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

    
    reset();
}

FocusCurveRecorderProgressDetailsPanelT::~FocusCurveRecorderProgressDetailsPanelT()
{
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


void FocusCurveRecorderProgressDetailsPanelT::setCurrentIterationProgress(int progressPerc)
{
  m_ui->pgbCurrentIterationProgress->setValue(progressPerc);
}

void FocusCurveRecorderProgressDetailsPanelT::setCurrentIterationProgressText(const std::string & currentProgressText)
{
  m_ui->lblCurrentIterationProgressText->setText(QString::fromStdString(currentProgressText));
}

void FocusCurveRecorderProgressDetailsPanelT::setTotalProgress(int progressPerc)
{
  m_ui->pgbTotalProgress->setValue(progressPerc);
}

void FocusCurveRecorderProgressDetailsPanelT::setTotalProgressText(const std::string & totalProgressText)
{
  m_ui->lblTotalProgressText->setText(QString::fromStdString(totalProgressText));
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