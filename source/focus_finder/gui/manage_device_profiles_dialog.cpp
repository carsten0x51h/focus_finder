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

#include <QMessageBox>

#include <optional>
#include <utility>

#include "include/manage_device_profiles_dialog.h"
#include "include/manage_device_entry_panel.h"
#include "include/profile_settings_dialog.h"

#include "../common/include/device_manager.h"
#include "../common/include/profile_manager.h"
#include "../common/include/camera_interface.h"
#include "../common/include/focus_interface.h"
#include "../common/include/filter_interface.h"

#include "ui_manage_device_profiles_dialog.h"

DEF_Exception(ManageDeviceProfilesDialog);


int ManageDeviceProfilesDialogT::showDeleteWarningMessage(const std::string &profileName) {
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString("Profile '" + profileName + "' will be removed."));
    msgBox.setInformativeText(QString::fromStdString("Do you want to proceed ?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

void ManageDeviceProfilesDialogT::refillProfileList() {

    m_ui->cbxSelectedProfile->blockSignals(true);

    m_ui->cbxSelectedProfile->clear();

    auto profileDirectoryNames = mFfl.getProfileManager()->getProfileDirectoryNames();

    std::for_each(profileDirectoryNames.begin(), profileDirectoryNames.end(),
                  [&](const std::string &profileFilename) {
                      m_ui->cbxSelectedProfile->addItem(QString::fromStdString(profileFilename));
                  });

    // Select the currently active profile entry - BEFORE connecting the QT listener...
    std::string activeProfileDirectoryName = mFfl.getProfileManager()->getActiveProfileDirectoryName();

    int idx = m_ui->cbxSelectedProfile->findText(QString::fromStdString(activeProfileDirectoryName));

    m_ui->cbxSelectedProfile->setCurrentIndex(idx);

    m_ui->cbxSelectedProfile->blockSignals(false);
}

void ManageDeviceProfilesDialogT::onProfileListChangedSlot() {
    refillProfileList();
}

void ManageDeviceProfilesDialogT::addDevicePanel(
        ManageDeviceEntryPanelT *devicePanel) {
    m_ui->layDevices->addWidget(devicePanel);
}

void ManageDeviceProfilesDialogT::addDevices() {

    auto deviceManager = mFfl.getDeviceManager();
    auto activeProfile = mFfl.getProfileManager()->getActiveProfile();

    mCameraPanel = new ManageDeviceEntryPanelT(mFfl, "Camera",
                                               (activeProfile ? activeProfile->getCameraDeviceName() : DeviceT::NONE));

    addDevicePanel(mCameraPanel);

    mFocusPanel = new ManageDeviceEntryPanelT(mFfl, "Focus",
                                              (activeProfile ? activeProfile->getFocusDeviceName() : DeviceT::NONE));

    addDevicePanel(mFocusPanel);


    mFilterPanel = new ManageDeviceEntryPanelT(mFfl, "Filter",
                                               (activeProfile ? activeProfile->getFilterDeviceName() : DeviceT::NONE));

    addDevicePanel(mFilterPanel);
}

void ManageDeviceProfilesDialogT::onNewProfileActionTriggeredSlot() {
    LOG(debug)
        << "ManageDeviceProfilesDialogT::onNewProfileActionTriggeredSlot..."
        << std::endl;

    // TODO: Call edit dialog...
}

void ManageDeviceProfilesDialogT::onSettingsProfileActionTriggeredSlot() {
    LOG(debug)
        << "ManageDeviceProfilesDialogT::onSettingsProfileActionTriggeredSlot..."
        << std::endl;

    if (mProfileSettingsDialog->exec() == QDialog::Accepted) {
        LOG(debug) << "Profile settings updated-> TODO...update ..." << std::endl;
        //refillProfileList();
        // TODO
    }
}

void ManageDeviceProfilesDialogT::onDeleteProfileActionTriggeredSlot() {
    LOG(debug)
        << "ManageDeviceProfilesDialogT::onDeleteProfileActionTriggeredSlot..."
        << std::endl;

    auto profileManager = mFfl.getProfileManager();

    std::string activeProfileDirectoryName = profileManager->getActiveProfileDirectoryName();

    int result = showDeleteWarningMessage(activeProfileDirectoryName);

    if (result == QMessageBox::Yes) {
        // TODO: try-catch .. error handling -> UI...
        // This call results in a callback to "onActiveProfileChangedSlot" which
        // automatically updates the UI. It also triggers a callback that the
        // list of profiles has changed whic leads to the call of "onProfileListChangedSlot".
        profileManager->deleteProfile(activeProfileDirectoryName);
    }
}

void ManageDeviceProfilesDialogT::updateMenuStatus() {
    std::string activeProfileDirectoryName = mFfl.getProfileManager()->getActiveProfileDirectoryName();
    bool isProfileSelected = (!activeProfileDirectoryName.empty());

    mDeleteProfileAction->setEnabled(isProfileSelected);
    mSettingsProfileAction->setEnabled(isProfileSelected);
}

void ManageDeviceProfilesDialogT::profileToUI(std::optional<FocusFinderProfileT> profileOpt) {

    static const std::string notSet = "Not set";
    static const std::string baseTitle = "Manage FocusFinder Profiles: ";


    this->setWindowTitle(
            QString::fromStdString(baseTitle + (profileOpt.has_value() ? profileOpt.value().getName() : notSet)));

    mCameraPanel->setDeviceName(profileOpt.has_value() ? profileOpt.value().getCameraDeviceName() : notSet);
    mFocusPanel->setDeviceName(profileOpt.has_value() ? profileOpt.value().getFocusDeviceName() : notSet);
    mFilterPanel->setDeviceName(profileOpt.has_value() ? profileOpt.value().getFilterDeviceName() : notSet);


    // Select the currently active profile entry
    m_ui->cbxSelectedProfile->blockSignals(true);

    std::string activeProfileDirectoryName = mFfl.getProfileManager()->getActiveProfileDirectoryName();

    int idx = m_ui->cbxSelectedProfile->findText(QString::fromStdString(activeProfileDirectoryName));

    m_ui->cbxSelectedProfile->setCurrentIndex(idx);

    m_ui->cbxSelectedProfile->blockSignals(false);


    // Update menu status
    updateMenuStatus();
}

void ManageDeviceProfilesDialogT::onActiveProfileChangedSlot(const std::optional<FocusFinderProfileT>& oldProfile,
                                                             const std::optional<FocusFinderProfileT>& newProfile) {

    LOG(debug) << "ManageDeviceProfilesDialogT::onActiveProfileChangedSlot..." << std::endl;

    auto profileManager = mFfl.getProfileManager();
    auto activeProfileOpt = profileManager->getActiveProfile();

    profileToUI(activeProfileOpt);
}

// A new profile has been selected -> tell the backend...
void ManageDeviceProfilesDialogT::onProfileSelectionChangedSlot(
        const QString &selectedProfileNameQtStr) {

    std::string selectedProfileName = selectedProfileNameQtStr.toStdString();

    LOG(debug)
        << "ManageDeviceProfilesDialogT::onProfileSelectionChangedSlot..."
        << selectedProfileName << std::endl;

    auto profileManager = mFfl.getProfileManager();

    if (!selectedProfileName.empty()) {
        // Activate profile...
        // TODO: catch exc. -> error handling in UI...
        profileManager->activateProfile(selectedProfileName);
    } else {
        profileManager->clearActiveProfile();
    }
}

void ManageDeviceProfilesDialogT::setupProfileList() {

    refillProfileList();

    connect(m_ui->cbxSelectedProfile,
            QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            this, &ManageDeviceProfilesDialogT::onProfileSelectionChangedSlot);
}

void ManageDeviceProfilesDialogT::createManageProfilesMenu() {

    mManageProfilesMenu = new QMenu;  // TODO: Cleanup

    mNewProfileAction = new QAction("New...", this);  // TODO: Cleanup?
    mNewProfileAction->setStatusTip(tr("Create new FocusFinder profile."));
    mManageProfilesMenu->addAction(mNewProfileAction);
    connect(mNewProfileAction, &QAction::triggered, this,
            &ManageDeviceProfilesDialogT::onNewProfileActionTriggeredSlot);

    mDeleteProfileAction = new QAction("Delete", this);  // TODO: Cleanup?
    mDeleteProfileAction->setStatusTip(
            tr("Delete selected FocusFinder profile."));
    mManageProfilesMenu->addAction(mDeleteProfileAction);
    connect(mDeleteProfileAction, &QAction::triggered, this,
            &ManageDeviceProfilesDialogT::onDeleteProfileActionTriggeredSlot);

    mSettingsProfileAction = new QAction("Settings...", this); // TODO: Cleanup?
    mSettingsProfileAction->setStatusTip(
            tr("Edit settings of selected FocusFinder profile."));
    mManageProfilesMenu->addAction(mSettingsProfileAction);
    connect(mSettingsProfileAction, &QAction::triggered, this,
            &ManageDeviceProfilesDialogT::onSettingsProfileActionTriggeredSlot);

    m_ui->btnManageProfiles->setMenu(mManageProfilesMenu);

    updateMenuStatus();
}

ManageDeviceProfilesDialogT::ManageDeviceProfilesDialogT(
        FocusFinderLogicT &ffl) :
        m_ui(new Ui::ManageDeviceProfilesDialog), mFfl(ffl) {
    // Setup UI
    m_ui->setupUi(this);

    mProfileSettingsDialog = new ProfileSettingsDialogT(this, mFfl);

    createManageProfilesMenu();
    setupProfileList();
    addDevices();

    // Register at profile manager to get notified if selected profile / device changes...
    connect(this, &ManageDeviceProfilesDialogT::activeProfileChangedSignal, this,
            &ManageDeviceProfilesDialogT::onActiveProfileChangedSlot);

    mFfl.getProfileManager()->registerActiveProfileChangedListener(
            [&](std::optional<FocusFinderProfileT> oldProfile, std::optional<FocusFinderProfileT> newProfile) {
                emit activeProfileChangedSignal(std::move(oldProfile), std::move(newProfile));
            }
    );

    // Regiser at profile manager to get notified in case the list of available profiles changes.
    connect(this, &ManageDeviceProfilesDialogT::profileListChangedSignal, this,
            &ManageDeviceProfilesDialogT::onProfileListChangedSlot);

    mFfl.getProfileManager()->registerProfileListChangedListener(
            [&]() { emit profileListChangedSignal(); }
    );

    // Initially fill the fields based on the currently active profile.
    profileToUI(mFfl.getProfileManager()->getActiveProfile());
}

ManageDeviceProfilesDialogT::~ManageDeviceProfilesDialogT() {
    delete mCameraPanel;
    delete mFocusPanel;
    delete mFilterPanel;

    delete mNewProfileAction;
    delete mDeleteProfileAction;
    delete mSettingsProfileAction;

    delete mManageProfilesMenu;
    delete mProfileSettingsDialog;
}
