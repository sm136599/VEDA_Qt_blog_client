#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "postlistwidget.h"
#include "postwidget.h"
#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    qDebug() << "MainWidget constructor";
    ui->newPostButton->hide();
    ui->userButton->hide();
    ui->logoutButton->hide();
    ui->withdrawButton->hide();

    connect(ui->loginButton, &QPushButton::clicked, [this](){
        ui->stackedWidget->setCurrentIndex(1);
        ui->loginButton->hide();
        ui->registerButton->hide();
        ui->newPostButton->show();
        ui->userButton->show();
        ui->logoutButton->show();
        ui->withdrawButton->show();
        qDebug() << "loginButton clicked";
    });
    ui->postListPage->setLayout(new QVBoxLayout);
    ui->postListPage->layout()->addWidget(new PostListWidget(ui->postListPage));
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->homeButton, &QPushButton::clicked, [this](){
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "homeButton clicked";
    });

    ui->postPage->setLayout(new QVBoxLayout);
    ui->postPage->layout()->addWidget(new PostWidget(ui->postPage));
    ui->stackedWidget->setCurrentIndex(1);
}

MainWidget::~MainWidget()
{
    delete ui;
}
