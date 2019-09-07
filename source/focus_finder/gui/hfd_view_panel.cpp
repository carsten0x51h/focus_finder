#include <QWidget>

#include <memory>
#include <cmath>

#include "include/hfd_view_panel.h"
#include "include/hfd_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/hfd.h"

#include "ui_hfd_view_panel.h"


HfdViewPanelT::HfdViewPanelT(QWidget * parent, FocusFinderLogicT & ffl) : QWidget(parent),
	m_ui(new Ui::HfdViewPanel),
	mFfl(ffl)
{
    // Setup UI
    m_ui->setupUi(this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(100);

	mHfdWidget = new HfdViewWidgetT(m_ui->widget);
	mHfdWidget->setSizePolicy(sizePolicy);
	m_ui->layHfdViewWidget->addWidget(mHfdWidget, 0/*row*/, 0/*col*/, 1/*rowspan*/, 1/*colspan*/);

    reset();
}

HfdViewPanelT::~HfdViewPanelT()
{
	// TODO: Cleanup required??
	//delete mHfdWidget;
}

void HfdViewPanelT::reset() {
	mHfdWidget->reset();
}

void HfdViewPanelT::setHfd(const HfdT & hfd) {
	LOG(debug) << "HfdViewPanelT::setHfd...mHfd valid? " << hfd.valid() << std::endl;

	mHfdWidget->setHfd(hfd);

	// Set HFD text
	std::string text = "-";

	if (hfd.valid()) {
		std::ostringstream oss;
		oss.precision(4);
		oss << hfd.getValue();
		text = oss.str();
	}

	m_ui->lblHfdValue->setText(QString::fromStdString(text));
}
