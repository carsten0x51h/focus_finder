#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_PROFILES_DIALOG_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_PROFILES_DIALOG_H_

// Qt includes
#include <QDialog>
#include <QToolButton>
#include <QMenu>
#include <QAction>

#include <optional>

#include "anim_menu_button.h"

#include "../../common/include/focus_finder_logic.h"
#include "../../common/include/focus_finder_profile.h"

namespace Ui {
    class ManageDeviceProfilesDialog;
}

class ManageDeviceEntryPanelT;
class NewProfileDialogT;
class RenameProfileDialogT;
class ProfileSettingsDialogT;

/**
*  @brief
*/
class ManageDeviceProfilesDialogT : public QDialog
{
    Q_OBJECT


public:
    /**
    *  @brief
    *    Constructor
    */
	ManageDeviceProfilesDialogT(FocusFinderLogicT & ffl);

    /**
    *  @brief
    *    Destructor
    */
	virtual ~ManageDeviceProfilesDialogT();

signals:
	void activeProfileChangedSignal();
	void profileListChangedSignal();

protected slots:
	void onNewProfileActionTriggeredSlot();
	void onDeleteProfileActionTriggeredSlot();
	void onSettingsProfileActionTriggeredSlot();

	void onProfileSelectionChangedSlot(const QString & selectedProfileNameQtStr);
	void onActiveProfileChangedSlot();
	void onProfileListChangedSlot();

protected:
    const QScopedPointer<Ui::ManageDeviceProfilesDialog> m_ui;

private:
    void updateMenuStatus();
    int showDeleteWarningMessage(const std::string & profileName) const;
    void profileToUI(std::optional<FocusFinderProfileT> profileOpt);

    void refillProfileList();
    void setupProfileList();
    void createManageProfilesMenu();


    void addDevicePanel(ManageDeviceEntryPanelT * devicePanel);
    void addDevices();

    FocusFinderLogicT & mFfl;

    ManageDeviceEntryPanelT * mCameraPanel;
    ManageDeviceEntryPanelT * mFocusPanel;
    ManageDeviceEntryPanelT * mFilterPanel;

    QAction * mNewProfileAction;
    QAction * mDeleteProfileAction;
    QAction * mSettingsProfileAction;
    QMenu * mManageProfilesMenu;

    NewProfileDialogT * mNewProfileDialog;
    ProfileSettingsDialogT * mProfileSettingsDialog;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_MANAGE_DEVICE_PROFILES_DIALOG_H_*/
