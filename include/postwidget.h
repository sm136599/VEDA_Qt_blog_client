#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>

#include "post.h"
namespace Ui {
class PostWidget;
}
class CommentWidget;

class PostWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PostWidget(QWidget *parent = nullptr);
    explicit PostWidget(Post post, QString user, QWidget *parent = nullptr);
    ~PostWidget();

    int getPostId();

signals:
    void back();
    void editPost(int postId, QString subject, QString description);
    void deletePost(int postId);
    void editComment(int commentId, QString description);
    void deleteComment(int commentId);

private:
    Ui::PostWidget *ui;

    Post post;
    bool isEditing;
};

#endif // POSTWIDGET_H