#ifndef WRITEPOSTWIDGET_H
#define WRITEPOSTWIDGET_H

#include <QWidget>

namespace Ui {
class WritePostWidget;
}

class WritePostWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WritePostWidget(QWidget *parent = nullptr);
    ~WritePostWidget();

private:
    Ui::WritePostWidget *ui;
};

#endif // WRITEPOSTWIDGET_H