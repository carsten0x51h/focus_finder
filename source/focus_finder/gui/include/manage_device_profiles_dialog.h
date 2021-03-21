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
	void activeProfileChangedSignal(std::optional<FocusFinderProfileT> oldProfile, std::optional<FocusFinderProfileT> newProfile);
	void profileListChangedSignal();

protected slots:
	void onNewProfileActionTriggeredSlot();
	void onDeleteProfileActionTriggeredSlot();
	void onSettingsProfileActionTriggeredSlot();

	void onProfileSelectionChangedSlot(const QString & selectedProfileNameQtStr);
	void onActiveProfileChangedSlot(std::optional<FocusFinderProfileT> oldProfile, std::optional<FocusFinderProfileT> newProfile);
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
