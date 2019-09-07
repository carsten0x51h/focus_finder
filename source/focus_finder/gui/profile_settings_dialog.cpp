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

//	connect(m_ui->buttonBox, & QDialogButtonBox::accepted, this, & NewProfileDialogT::onAcceptedSlot);
//	connect(m_ui->buttonBox, & QDialogButtonBox::rejected, this, & NewProfileDialogT::onRejectedSlot);
}

ProfileSettingsDialogT::~ProfileSettingsDialogT() {
}

void ProfileSettingsDialogT::onAcceptedSlot() {
	LOG(debug)
	<< "ProfileSettingsDialogT::onAcceptSlot..." << std::endl;
}

void ProfileSettingsDialogT::onRejectedSlot() {
	LOG(debug)
	<< "ProfileSettingsDialogT::onRejectSlot..." << std::endl;
}
