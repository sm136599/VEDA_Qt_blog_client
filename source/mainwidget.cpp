#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "postlistwidget.h"
#include "postlistitemwidget.h"
#include "postwidget.h"
#include "writepostwidget.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "httpclient.h"
#include "post.h"
#include "comment.h"

#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    postWidget = nullptr;
    postListWidget = nullptr;
    // 사용자 초기화
    username = "";
    // 레이아웃 초기화
    setLayoutForStackedWidget();
    // 게시물 리스트 위젯 생성
    postListWidget = new PostListWidget(ui->postListPage);
    // 게시물 페이지 연결
    ui->postListPage->layout()->addWidget(postListWidget);
    
    // 게시물 리스트 업데이트
    updatePostList();

    for(int i = 0; i < 10; i++) {
        postListWidget->addPostListItem(new PostListItemWidget(i, "제목", "작성자"));
    }

    // 화면 업데이트
    updateForGuest();

    // 연결 초기화
    setConnects();

}

MainWidget::~MainWidget()
{
    delete ui;
    if (postListWidget != nullptr)
        delete postListWidget;
    if (postWidget != nullptr)
        delete postWidget;
}

// public

// private
void MainWidget::setConnects() {
    connect(httpclient, &HttpClient::allPostsFetched, this, [this](QList<Post> postList) {
        postListWidget->hide();
        postListWidget->deleteLater();
        postListWidget = new PostListWidget(ui->postListPage);
        for (Post& post : postList) {
            postListWidget->addPostListItem(new PostListItemWidget(post.postNumber, post.subject, post.writer));
        }
    });
    connect(postListWidget, &PostListWidget::postClicked, this, [this](int postId) {
        httpclient->fetchPostById(postId);
    });
}


void MainWidget::setLayoutForStackedWidget() {
    ui->postListPage->setLayout(new QVBoxLayout());
    ui->postPage->setLayout(new QVBoxLayout());
    ui->writePostPage->setLayout(new QVBoxLayout());
}

void MainWidget::updateForGuest() {
    username = "";
    setUserLabel();
    ui->registerButton->show();
    ui->loginButton->show();
    ui->newPostButton->hide();
    ui->logoutButton->hide();
    ui->withdrawButton->hide();
    postListWidget->enableClickEvent();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::updateForMember() {
    setUserLabel();
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->show();
}

void MainWidget::updateForAdmin() {
    setUserLabel();
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->show();
}

void MainWidget::setUserLabel() {
    if (username == "") {
        ui->userLabel->setText("Blog");
    } else if (username == "admin") {
        ui->userLabel->setText("관리자");
    } else {
        ui->userLabel->setText(username);
    }   
}

void MainWidget::updatePostList() {
    httpclient->fetchAllPosts();
}