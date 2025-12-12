#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QTabWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include "IFrontPanelNode.h"

class TabControlModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode
{
    Q_OBJECT
public:
    TabControlModel() {
        _tabWidget = new QTabWidget();
        _tabWidget->addTab(new QWidget(), "Page 1");
        _tabWidget->addTab(new QWidget(), "Page 2");
    }
    
    QString caption() const override { return "Tab Control"; }
    QString name() const override { return "TabControl"; }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(QtNodes::PortType) const override { return 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return QtNodes::NodeDataType(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return nullptr; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}

    QWidget *embeddedWidget() override { return nullptr; }
    QWidget *frontPanelWidget() override { return _tabWidget; }

private:
    QTabWidget* _tabWidget;
};

class SplitterModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode
{
    Q_OBJECT
public:
    SplitterModel() {
        _splitter = new QSplitter(Qt::Horizontal);
        _splitter->addWidget(new QWidget()); // Placeholder 1
        _splitter->addWidget(new QWidget()); // Placeholder 2
        _splitter->setMinimumSize(200, 100);
    }
    
    QString caption() const override { return "Splitter"; }
    QString name() const override { return "Splitter"; }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(QtNodes::PortType) const override { return 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return QtNodes::NodeDataType(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return nullptr; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}

    QWidget *embeddedWidget() override { return nullptr; }
    QWidget *frontPanelWidget() override { return _splitter; }

private:
    QSplitter* _splitter;
};
