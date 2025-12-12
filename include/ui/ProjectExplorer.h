#pragma once
#include <QDockWidget>
#include <QTreeWidget>
#include <QFileSystemModel>

class ProjectExplorer : public QDockWidget
{
    Q_OBJECT
public:
    ProjectExplorer(QWidget* parent = nullptr) : QDockWidget("Project Explorer", parent)
    {
        _treeWidget = new QTreeWidget(this);
        _treeWidget->setHeaderLabel("Project Files");
        
        // Mock Structure
        auto root = new QTreeWidgetItem(_treeWidget);
        root->setText(0, "My Project.ofproj");
        root->setIcon(0, QIcon::fromTheme("folder"));
        
        auto viFolder = new QTreeWidgetItem(root);
        viFolder->setText(0, "VIs");
        
        auto mainVI = new QTreeWidgetItem(viFolder);
        mainVI->setText(0, "Main.vi");
        mainVI->setIcon(0, QIcon::fromTheme("application-x-executable"));
        
        auto subVI = new QTreeWidgetItem(viFolder);
        subVI->setText(0, "SubVI.vi");
        
        auto libFolder = new QTreeWidgetItem(root);
        libFolder->setText(0, "Dependencies");
        
        _treeWidget->expandAll();
        setWidget(_treeWidget);
    }

private:
    QTreeWidget* _treeWidget;
};
