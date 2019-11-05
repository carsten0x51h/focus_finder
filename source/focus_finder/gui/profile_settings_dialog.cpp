#include <QPushButton>

//#include <algorithm>

#include "include/profile_settings_dialog.h"

#include "../common/include/logging.h"
//#include "../common/include/profile_manager.h"

#include "ui_profile_settings_dialog.h"


//IDEE: Späääter, im cfg dialog hinter jedes Feld ein blaues Info Icon mit Mouse-over text zum Erklären der FUnktion.....?!
//IDEE: Später: Im cfg. Dialog einige Dinge "visualisieren" - z.B. Star window mit Grafik und Stern, das sich je nach Window size verändert....


ProfileSettingsDialogT::ProfileSettingsDialogT(QWidget * parent, FocusFinderLogicT & ffl) : QDialog(parent),
m_ui(new Ui::ProfileSettingsDialog),
mFfl(ffl)
{
	// Setup UI
	m_ui->setupUi(this);

	initFocusMeasureCombobox();
//	connect(m_ui->buttonBox, & QDialogButtonBox::accepted, this, & NewProfileDialogT::onAcceptedSlot);
//	connect(m_ui->buttonBox, & QDialogButtonBox::rejected, this, & NewProfileDialogT::onRejectedSlot);
}

ProfileSettingsDialogT::~ProfileSettingsDialogT() {
}

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
