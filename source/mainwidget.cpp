#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "postlistwidget.h"
#include "postwidget.h"
#include "writepostwidget.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "httpclient.h"
#include "post.h"
#include "comment.h"

#include <QDebug>
#include <QMessageBox>
#include <QTimer>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    // 페이지 초기화
    setPages();

    // 사용자 초기화
    username = "";
    updateMode();

    // 게시물 리스트 업데이트
    httpclient->fetchAllPosts();

    // 연결 초기화
    setConnects();
}

MainWidget::~MainWidget()
{
    delete ui;
    postListWidget->deleteLater();
    postWidget->deleteLater();
    writePostWidget->deleteLater();
}

// public

// private
void MainWidget::setConnects() {
    // 게시물 목록----------------------------------------------------------
    // 모든 게시물 받아오기
    connect(httpclient, &HttpClient::allPostsFetched, this, [=](QList<Post> postList) {
        postListWidget->clearPostList();
        postListWidget->addPostListItemList(postList);      

        updateMode();
    });
    connect(this->postListWidget, &PostListWidget::postClicked, this, [this](int postId) {
        QTimer::singleShot(200, this, [=]() {
            httpclient->fetchPostById(postId);
        });
    });
    // 게시물--------------------------------------------------------------
    // 게시물 받아오기
    connect(httpclient, &HttpClient::postFetched, this, [=](Post post) {
        postWidget->clearPost();
        postWidget->setPost(post, this->username);
        ui->stackedWidget->setCurrentIndex(1);
    });
    // 게시물 뒤로가기
    connect(postWidget, &PostWidget::back, this, [=]() {
        this->ui->stackedWidget->setCurrentIndex(0);
    });
    // 게시물 수정
    connect(postWidget, &PostWidget::editPost, [=](int postId, QString subject, QString description) {
        qDebug() << postId << "번 게시물 수정";
        QTimer::singleShot(200, this, [=]() {
            httpclient->editPost(postId, subject, description);
        });
    });
    // 게시물 삭제
    connect(postWidget, &PostWidget::deletePost, [=](int postId) {
        QTimer::singleShot(200, this, [=]() {
            httpclient->deletePost(postId);
        });
    });
    // 댓글 업로드
    connect(postWidget, &PostWidget::uploadComment, [=](int postId, QString description) {
        QTimer::singleShot(200, this, [=]() {
            httpclient->uploadComment(postId, this->username, description);
        });
    });
    // 댓글 수정
    connect(postWidget, &PostWidget::editComment, [=](int commentId, QString description) {
        qDebug() << commentId << "번 댓글 수정";
        QTimer::singleShot(200, this, [=]() {
            httpclient->editComment(commentId, description);
        });
    });
    // 댓글 삭제
    connect(postWidget, &PostWidget::deleteComment, [=](int commentId) {
        QTimer::singleShot(200, this, [=]() {
            httpclient->deleteComment(commentId);
        });
    });
    // 게시물 수정 완료 -> 게시물 리스트, 게시물 업데이트
    connect(httpclient, &HttpClient::editPostResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchPostById(this->postWidget->getPostId());
            QTimer::singleShot(200, this, [this]() {
                httpclient->fetchAllPosts();
            });
        });
    });
    // 게시물 삭제 완료 -> 게시물 리스트, 게시물 페이지
    connect(httpclient, &HttpClient::deletePostResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchAllPosts();
            this->ui->stackedWidget->setCurrentIndex(0);
        });
    });
    // 댓글 업로드 완료 -> 게시물 다시 받아오기
    connect(httpclient, &HttpClient::uploadCommentResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchPostById(this->postWidget->getPostId());
        });
    });
    // 댓글 수정 완료 -> 게시물 다시 받아오기
    connect(httpclient, &HttpClient::editCommentResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchPostById(this->postWidget->getPostId());
        });
    });
    // 댓글 삭제 완료 -> 게시물 다시 받아오기
    connect(httpclient, &HttpClient::deleteCommentResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchPostById(this->postWidget->getPostId());
        });
    });

    // 로그인, 회원가입----------------------------------------------------------
    // 로그인 dialog 연결
    connect(ui->loginButton, &QPushButton::clicked, this, [this]() {
        LoginDialog loginDialog;
        connect(&loginDialog, &LoginDialog::loginSucceed, this, [=](QString username) {
            qDebug() << username << "로그인 성공";
            this->username = username;
            updateMode();
        });
        loginDialog.exec();
    });

    //회원가입 dialog 연결
    connect(ui->registerButton, &QPushButton::clicked, this, [=]() {
        RegisterDialog registerDialog;
        registerDialog.exec();
    });

    // 새 글 작성----------------------------------------------------------
    // 새 글 작성
    connect(ui->newPostButton, &QPushButton::clicked, this, [=]() {
        writePostWidget->clear();
        ui->stackedWidget->setCurrentIndex(2);
    });
    // 새 글 작성 취소
    connect(writePostWidget, &WritePostWidget::back, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    // 새 글 업로드
    connect(writePostWidget, &WritePostWidget::uploadPost, [=](QString title, QString description) {
        httpclient->uploadPost(title, this->username, description);
    });
    // 새 글 업로드 완료
    connect(httpclient, &HttpClient::uploadPostResponse, this, [=](QByteArray data) {
        httpclient->fetchAllPosts();
        ui->stackedWidget->setCurrentIndex(0);
    });

    // 로그아웃
    connect(ui->logoutButton, &QPushButton::clicked, this, [=]() {
        this->username = "";
        updateMode();
        ui->stackedWidget->setCurrentIndex(0);
    });

    // 회원탈퇴
    connect(ui->withdrawButton, &QPushButton::clicked, this, [=]() {
        // 회원탈퇴 확인 메시지 
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "회원탈퇴", "정말로 회원탈퇴 하시겠습니까?", 
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            httpclient->deleteUser(this->username);
        }
    });
    // 회원탈퇴 완료
    connect(httpclient, &HttpClient::deleteUserResponse, this, [=](QByteArray data) {
        QMessageBox::information(this, "회원탈퇴", "회원탈퇴가 완료되었습니다.");
        this->username = "";
        updateMode();
    });
}


void MainWidget::setPages() {
    ui->postListPage->setLayout(new QVBoxLayout());
    ui->postPage->setLayout(new QVBoxLayout());
    ui->writePostPage->setLayout(new QVBoxLayout());
    
    postWidget = new PostWidget();
    ui->postPage->layout()->addWidget(postWidget);

    postListWidget = new PostListWidget();
    ui->postListPage->layout()->addWidget(postListWidget);

    writePostWidget = new WritePostWidget();
    ui->writePostPage->layout()->addWidget(writePostWidget);
}

void MainWidget::updateMode() {
    setUserLabel();
    if (username == "") {
        updateForGuest();
    } else if (username == "admin") {
        updateForAdmin();
    } else {
        updateForMember();
    }
}

void MainWidget::updateForGuest() {
    ui->registerButton->show();
    ui->loginButton->show();
    ui->newPostButton->hide();
    ui->logoutButton->hide();
    ui->withdrawButton->hide();

    postListWidget->disableClickEvent();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::updateForMember() {
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->show();

    postListWidget->enableClickEvent();
}

void MainWidget::updateForAdmin() {
    ui->registerButton->hide();
    ui->loginButton->hide();
    ui->newPostButton->show();
    ui->logoutButton->show();
    ui->withdrawButton->hide();

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
