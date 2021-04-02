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

#include <QPushButton>

#include "include/profile_settings_dialog.h"
#include "ui_profile_settings_dialog.h"


/**
 * IDEA: Later, add in the CFG dialog for each field an "info icon" with mouse-over text to explain the fields function.
 * IDEA: Later, add in the CFG dialog some visualizations - e.g. "star window" with picture of star which changes depending on the star window.
 */

ProfileSettingsDialogT::ProfileSettingsDialogT(QWidget *parent, FocusFinderLogicT &ffl) : QDialog(parent),
                                                                                          m_ui(new Ui::ProfileSettingsDialog),
                                                                                          mFfl(ffl) {
    // Setup UI
    m_ui->setupUi(this);

    initFocusMeasureCombobox();
//	connect(m_ui->buttonBox, & QDialogButtonBox::accepted, this, & NewProfileDialogT::onAcceptedSlot);
//	connect(m_ui->buttonBox, & QDialogButtonBox::rejected, this, & NewProfileDialogT::onRejectedSlot);
}

ProfileSettingsDialogT::~ProfileSettingsDialogT() = default;

void ProfileSettingsDialogT::onFocusMeasureSelectionChanged() {

    // QVariant cbxData = m_ui->cbxFocusMeasure->currentData();
    // auto focusMeasureType = cbxData.value<FocusMeasureTypeT::TypeE>();

    // LOG(debug) << "FocusCurveRecorderPanelT::onFocusMeasureChanged...focusMeasureType=" << FocusMeasureTypeT::asStr(focusMeasureType) << std::endl;

    // // TODO...
}

void ProfileSettingsDialogT::initFocusMeasureCombobox() {

    // TODO: Implement...
    // for (size_t i = 0; i < FocusMeasureTypeT::_Count; ++i) {

    //   FocusMeasureTypeT::TypeE idx = static_cast<FocusMeasureTypeT::TypeE>(i);

    //   QVariant data;
    //   data.setValue(idx);

    //   m_ui->cbxFocusMeasure->addItem(
    //        QString::fromStdString(FocusMeasureTypeT::asStr(idx)),
    // 	 data
    //   );
    // }

    // connect(m_ui->cbxFocusMeasure, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //   		this, &FocusCurveRecorderPanelT::onFocusMeasureSelectionChanged);
}

void ProfileSettingsDialogT::onAcceptedSlot() {
    LOG(debug)
        << "ProfileSettingsDialogT::onAcceptSlot..." << std::endl;
}

void ProfileSettingsDialogT::onRejectedSlot() {
    LOG(debug)
        << "ProfileSettingsDialogT::onRejectSlot..." << std::endl;
}
