#include "include/about_dialog.h"

#include "../common/include/logging.h"

#include "ui_about_dialog.h"

AboutDialogT::AboutDialogT(QWidget * parent) : QDialog(parent),
	m_ui(new Ui::AboutDialog)
{
    // Setup UI
    m_ui->setupUi(this);
}

AboutDialogT::~AboutDialogT()
{
}
