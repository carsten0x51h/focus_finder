#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABOUT_DIALOG_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABOUT_DIALOG_H_

// Qt includes
#include <QDialog>


namespace Ui {
	class AboutDialog;
}

/**
 *  @brief
 */
class AboutDialogT: public QDialog {
	Q_OBJECT

public:
	/**
	 *  @brief
	 *    Constructor
	 */
	AboutDialogT(QWidget * parent);

	/**
	 *  @brief
	 *    Destructor
	 */
	virtual ~AboutDialogT();

	signals:

protected slots:

protected:
    const QScopedPointer<Ui::AboutDialog> m_ui;

private:
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABOUT_DIALOG_H_*/
