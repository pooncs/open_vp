#include "ContextHelp.h"
#include <QVBoxLayout>

ContextHelp::ContextHelp(QWidget *parent) : QDockWidget("Context Help", parent) {
  auto container = new QWidget();
  auto layout = new QVBoxLayout(container);
  layout->setAlignment(Qt::AlignTop);

  _titleLabel = new QLabel("<b>No Selection</b>");
  _titleLabel->setStyleSheet("font-size: 14px; margin-bottom: 5px;");
  layout->addWidget(_titleLabel);

  _descLabel = new QTextEdit();
  _descLabel->setReadOnly(true);
  _descLabel->setFrameStyle(QFrame::NoFrame);
  _descLabel->setText("Hover over a node to see help.");
  layout->addWidget(_descLabel);

  setWidget(container);
}

void ContextHelp::setHelpText(const QString &title, const QString &description) {
  _titleLabel->setText("<b>" + title + "</b>");
  _descLabel->setText(description);
}

void ContextHelp::clearHelp() {
  _titleLabel->setText("<b>No Selection</b>");
  _descLabel->setText("Hover over a node to see help.");
}
