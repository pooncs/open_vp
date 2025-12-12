#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QtNodes/NodeDelegateModelRegistry>
#include <memory>

class QuickDropDialog : public QDialog {
  Q_OBJECT
public:
  explicit QuickDropDialog(
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry,
      QWidget *parent = nullptr);

  QString selectedNodeName() const;

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
  void onTextChanged(const QString &text);
  void onItemActivated(QListWidgetItem *item);

private:
  void populateList();
  void filterList(const QString &query);

private:
  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> _registry;
  QLineEdit *_searchBox;
  QListWidget *_resultsList;
  QString _selectedName;
  
  // Cache of all available node names and their categories
  struct NodeEntry {
      QString name;
      QString category;
      QString displayName; // "Name (Category)"
  };
  std::vector<NodeEntry> _allNodes;
};
