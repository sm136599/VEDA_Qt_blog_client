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

    // for(int i = 0; i < 10; i++) {
    //     postListWidget->addPostListItem(new PostListItemWidget(i, "제목", "작성자"));
    // }


    // 연결 초기화
    setConnects();

    // TODO : 로그인, 회원가입 dialog 연결
    //        새 글 작성 구현
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
    // 모든 게시물 받아오기
    connect(httpclient, &HttpClient::allPostsFetched, this, [this](QList<Post> postList) {
        postListWidget->hide();
        postListWidget->deleteLater();
        delete postListWidget;
        postListWidget = new PostListWidget(ui->postListPage);
        for (Post& post : postList) {
            postListWidget->addPostListItem(new PostListItemWidget(post.postNumber, post.subject, post.writer));
        }
        connect(this->postListWidget, &PostListWidget::postClicked, this, [this](int postId) {
            httpclient->fetchPostById(postId);
        });
        ui->postListPage->layout()->addWidget(postListWidget);

        if (this->username == "") {
            updateForGuest();
        } else if (this->username == "admin") {
            updateForAdmin();
        } else {
            updateForMember();
        }
    });
    // 게시물 받아오기
    connect(httpclient, &HttpClient::postFetched, this, [this](Post post) {
        if (postWidget != nullptr) {
            postWidget->hide();
            postWidget->deleteLater();
        }
        postWidget = new PostWidget(post, username);
        ui->postPage->layout()->addWidget(postWidget);
        ui->stackedWidget->setCurrentIndex(1);
    });
    // 게시물 뒤로가기
    connect(postWidget, &PostWidget::back, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    // 게시물 수정
    connect(postWidget, &PostWidget::editPost, [this](int postId, QString subject, QString description) {
        httpclient->editPost(postId, subject, description);
    });
    // 게시물 수정 완료 -> 게시물 리스트, 게시물 업데이트
    connect(httpclient, &HttpClient::editPostResponse, this, [this](QByteArray data) {
        updatePostList();
        httpclient->fetchPostById(this->postWidget->getPostId());
    });
    // 게시물 삭제
    connect(postWidget, &PostWidget::deletePost, [this](int postId) {
        httpclient->deletePost(postId);
    });
    // 게시물 삭제 완료 -> 게시물 리스트, 게시물 페이지
    connect(httpclient, &HttpClient::deletePostResponse, this, [this](QByteArray data) {
        updatePostList();
        ui->stackedWidget->setCurrentIndex(0);
    });
    // 댓글 수정
    connect(postWidget, &PostWidget::editComment, [this](int commentId, QString description) {
        httpclient->editComment(commentId, description);
    });
    // 댓글 수정 완료 -> 게시물 다시 받아오기
    connect(httpclient, &HttpClient::editCommentResponse, this, [this](QByteArray data) {
        httpclient->fetchPostById(this->postWidget->getPostId());
    });
    // 댓글 삭제
    connect(postWidget, &PostWidget::deleteComment, [this](int commentId) {
        httpclient->deleteComment(commentId);
    });
    // 댓글 삭제 완료 -> 게시물 다시 받아오기
    connect(httpclient, &HttpClient::deleteCommentResponse, this, [this](QByteArray data) {
        httpclient->fetchPostById(this->postWidget->getPostId());
    });
    // 로그인 dialog 연결
    connect(ui->loginButton, &QPushButton::clicked, this, [this]() {
        LoginDialog* loginDialog = new LoginDialog();
        connect(loginDialog, &LoginDialog::loginSucceed, this, [this](QString username) {
            qDebug() << username << "로그인 성공";
            this->username = username;
            if (username == "admin") {
                updateForAdmin();
            } else {
                updateForMember();
            }
        });
        loginDialog->exec();
    });

    //회원가입 dialog 연결
    connect(ui->registerButton, &QPushButton::clicked, this, [this]() {
        RegisterDialog registerDialog;
        registerDialog.exec();
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
    postListWidget->disableClickEvent();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::updateForMember() {
    setUserLabel();
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->show();

    postListWidget->enableClickEvent();
}

void MainWidget::updateForAdmin() {
    setUserLabel();
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->show();

    postListWidget->enableClickEvent();
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