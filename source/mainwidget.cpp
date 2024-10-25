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
#include <QMessageBox>
#include <QTimer>


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    postWidget = nullptr;
    postListWidget = nullptr;
    writePostWidget = nullptr;
    // 사용자 초기화
    username = "";
    // 레이아웃 초기화
    setLayoutForStackedWidget();
    // 게시물 리스트 위젯 생성
    postListWidget = new PostListWidget(ui->postListPage);
    // 게시물 페이지 연결
    ui->postListPage->layout()->addWidget(postListWidget);
    
    updateForGuest();

    // 게시물 리스트 업데이트
    updatePostList();

    // 연결 초기화
    setConnects();
}

MainWidget::~MainWidget()
{
    delete ui;
    postListWidget->deleteLater();
    postWidget->deleteLater();
}

// public

// private
void MainWidget::setConnects() {
    // 모든 게시물 받아오기
    connect(httpclient, &HttpClient::allPostsFetched, this, [=](QList<Post> postList) {
        if (postListWidget != nullptr) {
            httpclient->fetchAllPostsManager->disconnect();
            postListWidget->hide();
            postListWidget->deleteLater();
        }
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
    connect(httpclient, &HttpClient::postFetched, this, [=](Post post) {
        if (postWidget != nullptr) {
            httpclient->fetchPostByIdManager->disconnect();
            httpclient->uploadCommentManager->disconnect();
            httpclient->editCommentManager->disconnect();
            httpclient->deleteCommentManager->disconnect();
            httpclient->editPostManager->disconnect();
            httpclient->deletePostManager->disconnect();
            postWidget->disconnect();
            postWidget->hide();
            postWidget->deleteLater();
        }
        qDebug() << post.postNumber << "번 게시물 받아오기";
        postWidget = new PostWidget(post, username, this->ui->postPage);
        // 게시물 뒤로가기
        connect(postWidget, &PostWidget::back, this, [=]() {
            this->ui->stackedWidget->setCurrentIndex(0);
        });
        // 게시물 수정
        connect(postWidget, &PostWidget::editPost, [=](int postId, QString subject, QString description) {
            qDebug() << postId << "번 게시물 수정";
            httpclient->editPost(postId, subject, description);
        });
        // 게시물 삭제
        connect(postWidget, &PostWidget::deletePost, [=](int postId) {
            httpclient->deletePost(postId);
        });
        // 댓글 업로드
        connect(postWidget, &PostWidget::uploadComment, [=](int postId, QString description) {
            httpclient->uploadComment(postId, this->username, description);
        });
        // 댓글 수정
        connect(postWidget, &PostWidget::editComment, [=](int commentId, QString description) {
            qDebug() << commentId << "번 댓글 수정";
            httpclient->editComment(commentId, description);
        });
        // 댓글 삭제
        connect(postWidget, &PostWidget::deleteComment, [=](int commentId) {
            httpclient->deleteComment(commentId);
        });
        ui->postPage->layout()->addWidget(postWidget);
        ui->stackedWidget->setCurrentIndex(1);
    });
    // 게시물 수정 완료 -> 게시물 리스트, 게시물 업데이트
    connect(httpclient, &HttpClient::editPostResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            httpclient->fetchPostById(this->postWidget->getPostId());
            QTimer::singleShot(200, this, [this]() {
                updatePostList();
            });
        });
    });
    // 게시물 삭제 완료 -> 게시물 리스트, 게시물 페이지
    connect(httpclient, &HttpClient::deletePostResponse, this, [=](QByteArray data) {
        QTimer::singleShot(200, this, [this]() {
            updatePostList();
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
        qDebug() << "댓글 수정";
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
    // 로그인 dialog 연결
    connect(ui->loginButton, &QPushButton::clicked, this, [this]() {
        LoginDialog loginDialog;
        connect(&loginDialog, &LoginDialog::loginSucceed, this, [=](QString username) {
            qDebug() << username << "로그인 성공";
            this->username = username;
            if (username == "admin") {
                updateForAdmin();
            } else {
                updateForMember();
            }
        });
        loginDialog.exec();
    });

    //회원가입 dialog 연결
    connect(ui->registerButton, &QPushButton::clicked, this, [=]() {
        RegisterDialog registerDialog;
        registerDialog.exec();
    });

    // 새 글 작성
    connect(ui->newPostButton, &QPushButton::clicked, this, [=]() {
        if (writePostWidget != nullptr) {
            httpclient->uploadPostManager->disconnect();
            writePostWidget->disconnect();
            writePostWidget->hide();
            writePostWidget->deleteLater();
        }
        writePostWidget = new WritePostWidget();

        connect(this->writePostWidget, &WritePostWidget::back, this, [=]() {
            ui->stackedWidget->setCurrentIndex(0);
        });

        connect(this->writePostWidget, &WritePostWidget::uploadPost, [=](QString title, QString description) {
            httpclient->uploadPost(title, this->username, description);
        });
        
        ui->writePostPage->layout()->addWidget(this->writePostWidget);
        ui->stackedWidget->setCurrentIndex(2);
    });

    // 새 글 업로드 완료
    connect(httpclient, &HttpClient::uploadPostResponse, this, [=](QByteArray data) {
        updatePostList();
        this->ui->stackedWidget->setCurrentIndex(0);
    });

    // 로그아웃
    connect(ui->logoutButton, &QPushButton::clicked, this, [=]() {
        this->username = "";
        updateForGuest();
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
            this->username = "";
            updateForGuest();
            ui->stackedWidget->setCurrentIndex(0);
        }
    });
    
}


void MainWidget::setLayoutForStackedWidget() {
    if (!ui->postListPage->layout())
        ui->postListPage->setLayout(new QVBoxLayout());
    if (!ui->postPage->layout())
        ui->postPage->setLayout(new QVBoxLayout());
    if (!ui->writePostPage->layout())
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

void MainWidget::updatePostList() {
    httpclient->fetchAllPosts();
}