#include <QWidget>

#include <memory>
#include <cmath>

#include "include/fwhm_view_panel.h"
#include "include/fwhm_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/fwhm.h"

#include "ui_fwhm_view_panel.h"


FwhmViewPanelT::FwhmViewPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
	m_ui(new Ui::FwhmViewPanel),
	mFfl(ffl)
{
    // Setup UI
    m_ui->setupUi(this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	mFwhmWidget = new FwhmViewWidgetT(m_ui->widget);
	mFwhmWidget->setSizePolicy(sizePolicy);
	m_ui->layFwhmViewWidget->addWidget(mFwhmWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    reset();
}

FwhmViewPanelT::~FwhmViewPanelT()
{
	// TODO: Cleanup required??
	//delete mFwhmWidget;
}

void FwhmViewPanelT::reset() {
	mFwhmWidget->reset();
}

void FwhmViewPanelT::setFwhm(const FwhmT & fwhm) {
	LOG(debug) << "FwhmViewPanelT::setFwhm...mFwhm valid? " << fwhm.valid() << std::endl;

	mFwhmWidget->setFwhm(fwhm);

	// Set Fwhm text
	std::string text = "-";

	if (fwhm.valid()) {
		std::ostringstream oss;
		oss.precision(4);
		oss << fwhm.getValue();
		text = oss.str();
	}

	m_ui->lblFwhmValue->setText(QString::fromStdString(text));
}
