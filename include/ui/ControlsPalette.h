#pragma once
#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QtNodes/NodeDelegateModelRegistry>
#include <memory>

class ControlsPalette : public QDialog {
    Q_OBJECT
public:
    explicit ControlsPalette(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry, QWidget* parent = nullptr);

private slots:
    void onTextChanged(const QString &text);

private:
    void populateList();
    void filterList(const QString &query);

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> _registry;
    QLineEdit* _searchBox;
    QListWidget* _list;

    struct NodeEntry {
        QString name;
        QString category;
        QString displayName;
    };
    std::vector<NodeEntry> _allNodes;
};

// Custom ListWidget to handle Drag Start
class DraggableListWidget : public QListWidget {
public:
    using QListWidget::QListWidget;
protected:
    void startDrag(Qt::DropActions supportedActions) override;
};
