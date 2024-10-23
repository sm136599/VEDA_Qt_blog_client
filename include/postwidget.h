#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>

namespace Ui {
class PostWidget;
}
class CommentWidget;

class PostWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PostWidget(QWidget *parent = nullptr);
    ~PostWidget();

private:
    Ui::PostWidget *ui;

    int id;
    QString title;
    QString writer;
    QString description;
    QVector<CommentWidget> comments; // Each comment has a commentNumber and description
};

#endif // POSTWIDGET_H