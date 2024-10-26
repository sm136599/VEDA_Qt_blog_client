#ifndef COMMENTWIDGET_H
#define COMMENTWIDGET_H

#include <QWidget>

#include "comment.h"
namespace Ui {
class CommentWidget;
}

class CommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentWidget(QWidget *parent = nullptr);
    explicit CommentWidget(Comment comment, QString user, QWidget *parent = nullptr);
    ~CommentWidget();

signals:
    void editComment(int commentId, QString description);
    void deleteComment(int commentId);

private:
    Ui::CommentWidget *ui;

    Comment comment;
    bool isEditing;
};

#endif // COMMENTWIDGET_H