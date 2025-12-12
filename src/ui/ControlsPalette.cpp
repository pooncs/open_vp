#include "ControlsPalette.h"
#include <QVBoxLayout>
#include <QMimeData>
#include <QDrag>
#include <algorithm>

void DraggableListWidget::startDrag(Qt::DropActions supportedActions) {
    QListWidgetItem *item = currentItem();
    if (!item) return;

    QString nodeType = item->data(Qt::UserRole).toString();
    
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-qtlv-node", nodeType.toUtf8());
    mimeData->setText(nodeType); // Fallback

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    
    // Maybe set a pixmap for the drag?
    
    drag->exec(supportedActions);
}

ControlsPalette::ControlsPalette(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry, QWidget* parent) 
    : QDialog(parent), _registry(registry) {
    setWindowTitle("Controls");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    resize(250, 500);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    _searchBox = new QLineEdit(this);
    _searchBox->setPlaceholderText("Search Controls...");
    layout->addWidget(_searchBox);

    _list = new DraggableListWidget(this);
    _list->setDragEnabled(true);
    _list->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(_list);

    connect(_searchBox, &QLineEdit::textChanged, this, &ControlsPalette::onTextChanged);

    populateList();
}

void ControlsPalette::populateList() {
    if (!_registry) return;

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

    // Sort by Category then Name
    std::sort(_allNodes.begin(), _allNodes.end(),
            [](const NodeEntry &a, const NodeEntry &b) {
                if (a.category != b.category)
                    return a.category < b.category;
                return a.name < b.name;
            });

    filterList("");
}

void ControlsPalette::filterList(const QString &query) {
    _list->clear();
    QString lowerQuery = query.toLower();

    QString lastCategory = "";

    for (const auto &entry : _allNodes) {
        bool match = true;
        if (!lowerQuery.isEmpty()) {
             if (!entry.name.toLower().contains(lowerQuery) && 
                 !entry.category.toLower().contains(lowerQuery)) {
                match = false;
            }
        }

        if (match) {
            // Add Category Header if changed (and not filtering, or maybe always?)
            // If filtering, category headers might be confusing if only 1 item matches.
            // Let's only add headers if query is empty for now, or just show flat list with parens.
            
            auto item = new QListWidgetItem(entry.displayName);
            item->setData(Qt::UserRole, entry.name);
            _list->addItem(item);
        }
    }
}

void ControlsPalette::onTextChanged(const QString &text) {
    filterList(text);
}
