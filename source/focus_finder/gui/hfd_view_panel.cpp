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

#include <QWidget>

#include <memory>
#include <cmath>

#include "include/hfd_view_panel.h"
#include "include/hfd_view_widget.h"

#include "../common/include/logging.h"
#include "../common/include/hfd.h"

#include "ui_hfd_view_panel.h"


HfdViewPanelT::HfdViewPanelT(QWidget * parent) : QWidget(parent),
	m_ui(new Ui::HfdViewPanel)
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
