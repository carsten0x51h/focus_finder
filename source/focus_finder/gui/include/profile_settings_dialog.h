#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_PROFILE_SETTINGS_DIALOG_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_PROFILE_SETTINGS_DIALOG_H_

// Qt includes
#include <QDialog>

#include "../../common/include/focus_finder_logic.h"


namespace Ui {
	class ProfileSettingsDialog;
}

/**
 *  @brief
 */
class ProfileSettingsDialogT: public QDialog {
	Q_OBJECT

public:
	/**
	 *  @brief
	 *    Constructor
	 */
	ProfileSettingsDialogT(QWidget * parent, FocusFinderLogicT & ffl);

	/**
	 *  @brief
	 *    Destructor
	 */
	virtual ~ProfileSettingsDialogT();

signals:

protected slots:
  void onAcceptedSlot();
  void onRejectedSlot();
  void onFocusMeasureSelectionChanged();

protected:
    const QScopedPointer<Ui::ProfileSettingsDialog> m_ui;

private:
  void initFocusMeasureCombobox();
  
  FocusFinderLogicT & mFfl;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_PROFILE_SETTINGS_DIALOG_H_*/
