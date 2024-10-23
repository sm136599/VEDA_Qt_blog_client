#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class PostListWidget;
class PostWidget;

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    
    QString username;

    PostListWidget *postListWidget;
    PostWidget *postWidget;

    void setConnects();
    void setLayoutForStackedWidget();
    void updateForGuest();
    void updateForMember();
    void updateForAdmin();
    void setUserLabel();
    void updatePostList();
};

#endif // MAINWIDGET_H
