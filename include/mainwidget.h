#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class PostListWidget;
class PostWidget;
class WritePostWidget;

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
    WritePostWidget *writePostWidget;

    void setConnects();
    void setPages();
    void updateMode();
    void updateForGuest();
    void updateForMember();
    void updateForAdmin();
    void setUserLabel();
};

#endif // MAINWIDGET_H
