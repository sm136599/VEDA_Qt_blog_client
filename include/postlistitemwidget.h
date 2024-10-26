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
    explicit PostListItemWidget(const int &id, const QString &title, const QString &writer, QWidget *parent = nullptr);
    ~PostListItemWidget();

    void setClickable(bool clickable);

signals:
    void clicked(int postId);
    
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::PostListItemWidget *ui;

    int id;
    QString title;
    QString writer;

    bool clickable;
};

#endif // POSTLISTITEMWIDGET_H
