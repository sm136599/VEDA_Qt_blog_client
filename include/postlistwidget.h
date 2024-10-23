#ifndef POSTLISTWIDGET_H
#define POSTLISTWIDGET_H

#include <QWidget>

class PostListItemWidget;

namespace Ui {
class PostListWidget;
}

class PostListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PostListWidget(QWidget *parent = nullptr);
    ~PostListWidget();

    void addPostListItem(PostListItemWidget *postListItemWidget);

private:
    Ui::PostListWidget *ui;
};

#endif // POSTLISTWIDGET_H
