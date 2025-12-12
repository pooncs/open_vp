#pragma once
#include <QDockWidget>
#include <QLabel>
#include <QTextEdit>

class ContextHelp : public QDockWidget {
  Q_OBJECT
public:
  explicit ContextHelp(QWidget *parent = nullptr);

  void setHelpText(const QString &title, const QString &description);
  void clearHelp();

private:
  QLabel *_titleLabel;
  QTextEdit *_descLabel;
};
