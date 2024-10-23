#ifndef POSTLISTITEMWIDGET_H
#define POSTLISTITEMWIDGET_H

#include <QWidget>

namespace Ui {
class PostListItemWidget;
}

class PostListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PostListItemWidget(QWidget *parent = nullptr);
    explicit PostListItemWidget(const QString &id, const QString &title, const QString &writer, QWidget *parent = nullptr);
    ~PostListItemWidget();

    

private:
    Ui::PostListItemWidget *ui;

    QString id;
    QString title;
    QString writer;
};

#endif // POSTLISTITEMWIDGET_H
