#ifndef POSTLISTWIDGET_H
#define POSTLISTWIDGET_H

#include <QWidget>
#include "post.h"

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
    void addPostListItem(const Post& post);
    void addPostListItemList(const QList<Post>& postList);
    void clearPostList();

    void enableClickEvent();
    void disableClickEvent();

signals:
    void postClicked(int postId);

private:
    Ui::PostListWidget *ui;

    QList<PostListItemWidget*> postListItemWidgetList;
};

#endif // POSTLISTWIDGET_H
