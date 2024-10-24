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

signals:
   void back();
   void uploadPost(QString title, QString description);

private:
    Ui::WritePostWidget *ui;
};

#endif // WRITEPOSTWIDGET_H