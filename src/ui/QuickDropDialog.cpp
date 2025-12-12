#include "QuickDropDialog.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include <algorithm>

QuickDropDialog::QuickDropDialog(
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry,
    QWidget *parent)
    : QDialog(parent, Qt::Popup | Qt::FramelessWindowHint), _registry(registry) {
  
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(5, 5, 5, 5);
  layout->setSpacing(5);

  _searchBox = new QLineEdit(this);
  _searchBox->setPlaceholderText("Quick Drop (Ctrl+Space)...");
  _searchBox->installEventFilter(this);
  layout->addWidget(_searchBox);

  _resultsList = new QListWidget(this);
  _resultsList->installEventFilter(this);
  layout->addWidget(_resultsList);

  connect(_searchBox, &QLineEdit::textChanged, this,
          &QuickDropDialog::onTextChanged);
  connect(_resultsList, &QListWidget::itemActivated, this,
          &QuickDropDialog::onItemActivated);
  connect(_resultsList, &QListWidget::itemClicked, this,
          &QuickDropDialog::onItemActivated);

  populateList();
  
  resize(300, 400);
}

QString QuickDropDialog::selectedNodeName() const { return _selectedName; }

void QuickDropDialog::populateList() {
  if (!_registry)
    return;

  auto creators = _registry->registeredModelCreators();
  auto categories = _registry->registeredModelsCategoryAssociation();

  for (const auto &pair : creators) {
    QString name = pair.first;
    QString category = categories.count(name) ? categories.at(name) : "Unknown";
    
    NodeEntry entry;
    entry.name = name;
    entry.category = category;
    entry.displayName = QString("%1 (%2)").arg(name, category);
    _allNodes.push_back(entry);
  }

  // Sort alphabetically
  std::sort(_allNodes.begin(), _allNodes.end(),
            [](const NodeEntry &a, const NodeEntry &b) {
              return a.name < b.name;
            });

  filterList("");
}

void QuickDropDialog::filterList(const QString &query) {
  _resultsList->clear();
  QString lowerQuery = query.toLower();

  for (const auto &entry : _allNodes) {
    bool match = true;
    if (!lowerQuery.isEmpty()) {
      // Fuzzy-ish: check if name contains query characters in order?
      // For now, simple "contains" is safer and faster.
      // We check Name and Category.
      if (!entry.name.toLower().contains(lowerQuery) && 
          !entry.category.toLower().contains(lowerQuery)) {
        match = false;
      }
    }

    if (match) {
      auto item = new QListWidgetItem(entry.displayName);
      item->setData(Qt::UserRole, entry.name);
      _resultsList->addItem(item);
    }
  }

  if (_resultsList->count() > 0) {
    _resultsList->setCurrentRow(0);
  }
}

void QuickDropDialog::onTextChanged(const QString &text) { filterList(text); }

void QuickDropDialog::onItemActivated(QListWidgetItem *item) {
  if (item) {
    _selectedName = item->data(Qt::UserRole).toString();
    accept();
  }
}

bool QuickDropDialog::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Down) {
      if (obj == _searchBox) {
        int row = _resultsList->currentRow();
        if (row < _resultsList->count() - 1)
          _resultsList->setCurrentRow(row + 1);
        return true;
      }
    } else if (keyEvent->key() == Qt::Key_Up) {
      if (obj == _searchBox) {
        int row = _resultsList->currentRow();
        if (row > 0)
          _resultsList->setCurrentRow(row - 1);
        return true;
      }
    } else if (keyEvent->key() == Qt::Key_Enter ||
               keyEvent->key() == Qt::Key_Return) {
        if (_resultsList->currentItem()) {
            onItemActivated(_resultsList->currentItem());
            return true;
        }
    } else if (keyEvent->key() == Qt::Key_Escape) {
        reject();
        return true;
    }
  }
  return QDialog::eventFilter(obj, event);
}
