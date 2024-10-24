#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "httpclient.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    isValidName = false;
    isValidPwCheck = false;
    // 뒤로가기 버튼
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        this->close();
    });
    // 회원가입 버튼
    connect(ui->registerButton, &QPushButton::clicked, this, [this]() {
        if (this->isValidName && this->isValidPwCheck) {
            httpclient->join(ui->idLineEdit->text(), ui->pwLineEdit->text(), ui->nameLineEdit->text());
        }
    });
    // 회원가입 성공 시
    connect(httpclient, &HttpClient::joinSucceed, this, [this]() {
        this->close();
    });
    // 회원가입 실패 시 - 실패 원인 1가지 -> 아이디 중복
    connect(httpclient, &HttpClient::joinFailed, this, [this](QString message) {
        qDebug() << message;
        QPalette palette = ui->idStatusLabel->palette();
        palette.setColor(QPalette::WindowText, Qt::red);
        ui->idStatusLabel->setPalette(palette);
        ui->idStatusLabel->setText("아이디가 중복되었습니다.");
    });
    // 비밀번호 일치 여부
    connect(ui->pwCheckLineEdit, &QLineEdit::textChanged, this, [this]() {
        if (ui->pwLineEdit->text() != ui->pwCheckLineEdit->text()) {
            QPalette palette = ui->pwCheckStatusLabel->palette();
            palette.setColor(QPalette::WindowText, Qt::red);
            ui->pwCheckStatusLabel->setPalette(palette);
            ui->pwCheckStatusLabel->setText("비밀번호가 일치하지 않습니다.");
            this->isValidPwCheck = false;
        } else {
            QPalette palette = ui->pwCheckStatusLabel->palette();
            palette.setColor(QPalette::WindowText, Qt::green);
            ui->pwCheckStatusLabel->setPalette(palette);
            ui->pwCheckStatusLabel->setText("비밀번호가 일치합니다.");
            this->isValidPwCheck = true;
        }
    });
    // 닉네임 admin 예약어 확인
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, [this]() {
        if (ui->nameLineEdit->text() == "admin") {
            QPalette palette = ui->nameStatusLabel->palette();
            palette.setColor(QPalette::WindowText, Qt::red);
            ui->nameStatusLabel->setPalette(palette);
            ui->nameStatusLabel->setText("사용할 수 없는 닉네임입니다.");
            this->isValidName = false;
        } else {
            QPalette palette = ui->nameStatusLabel->palette();
            palette.setColor(QPalette::WindowText, Qt::green);
            ui->nameStatusLabel->setPalette(palette);
            ui->nameStatusLabel->setText("사용 가능한 닉네임입니다.");
            this->isValidName = true;
        }
    });
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}