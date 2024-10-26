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
    ~PostWidget();

    int getPostId();
    void clearPost();
    void setPost(const Post& post, QString user);
    void addComment(const Comment& comment, QString user);

signals:
    void back();
    void editPost(int postId, QString subject, QString description);
    void deletePost(int postId);
    void editComment(int commentId, QString description);
    void deleteComment(int commentId);
    void uploadComment(int postId, QString description);

private:
    Ui::PostWidget *ui;

    Post post;
    bool isEditing;

    QList<CommentWidget*> commentWidgetList;
};

#endif // POSTWIDGET_H