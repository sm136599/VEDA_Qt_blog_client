#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "postlistwidget.h"
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
        ui->stackedWidget->setCurrentIndex(0);
        ui->loginButton->hide();
        ui->registerButton->hide();
        ui->newPostButton->show();
        ui->userButton->show();
        ui->logoutButton->show();
        ui->withdrawButton->show();
        qDebug() << "loginButton clicked";
    });
    ui->allPostPage->setLayout(new QVBoxLayout);
    ui->allPostPage->layout()->addWidget(new PostListWidget(ui->allPostPage));
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->homeButton, &QPushButton::clicked, [this](){
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "homeButton clicked";
    });
}

MainWidget::~MainWidget()
{
    delete ui;
}
