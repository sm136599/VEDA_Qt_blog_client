#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpclient.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 로그인 메시지 빨강으로 설정
    QPalette palette = ui->loginStatusLabel->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->loginStatusLabel->setPalette(palette);


    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        this->close();
    });

    connect(ui->loginButton, &QPushButton::clicked, this, [this]() {
        httpclient->login(ui->idLineEdit->text(), ui->pwLineEdit->text());
    });

    connect(httpclient, &HttpClient::loginSucceed, this, [this](QString username) {
        emit loginSucceed(username);
        this->close();
    });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}