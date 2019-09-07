#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_WIDGET_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_WIDGET_H_

// std includes

// Qt includes
#include <QWidget>
#include <QPixmap>
#include <QLabel>

#include "../../common/include/hfd.h"
#include "../../common/include/image.h"


/**
*  @brief
*/
class HfdViewWidgetT : public QLabel
{
Q_OBJECT

public:
    /**
    *  @brief
    *    Constructor
    */
	HfdViewWidgetT(QWidget * parent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~HfdViewWidgetT();

	void reset();
	void setHfd(const HfdT & hfd);

	void paintEvent(QPaintEvent * event) override;

signals:

protected slots:

protected:

private:
	QPixmap convertToPixmap(const ImageT & img);

	HfdT mHfd;
	QPixmap mStarPixmap;
};

#endif /*SOURCE_FOCUS_FINDER_GUI_INCLUDE_HFD_VIEW_WIDGET_H_*/
