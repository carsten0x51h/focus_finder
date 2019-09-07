/*
 * customtoolbutton.h
 *
 *  Created on: Apr 11, 2019
 *      Author: devnull
 */

#ifndef SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_
#define SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_

#include <memory>

#include <QWidget>

#include "../../common/include/logging.h"
#include "../../common/include/mapper_function.h"

class AbstractMapperPanelT : public QWidget {
	Q_OBJECT

public:
	AbstractMapperPanelT(QWidget * parent) : QWidget(parent) {}
    virtual ~AbstractMapperPanelT() {};

    virtual std::shared_ptr<MapperFunctionT> getMapperFunction() const = 0;
    virtual void notifyNewImage() = 0;

signals:
	void valueChangedSignal();
};

#endif /* SOURCE_FOCUS_FINDER_GUI_INCLUDE_ABSTRACT_MAPPER_PANEL_H_ */
