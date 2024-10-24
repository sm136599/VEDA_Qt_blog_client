#include "postwidget.h"
#include "ui_postwidget.h"
#include "commentwidget.h"

PostWidget::PostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostWidget)
{
    ui->setupUi(this);
}

PostWidget::PostWidget(Post post, QString user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostWidget)
{
    ui->setupUi(this);
    this->isEditing = false;
    this->post = post;
    ui->titleLineEdit->setText(post.subject);
    ui->textEdit->setText(post.description);
    for (Comment& comment : post.comments) {
        CommentWidget *commentWidget = new CommentWidget(comment, user);
        ui->scrollAreaWidgetContents->layout()->addWidget(commentWidget);
        connect(commentWidget, &CommentWidget::editComment, [this](int commentId, QString description) {
            emit editComment(commentId, description);
        });
        connect(commentWidget, &CommentWidget::deleteComment, [this](int commentId) {
            emit deleteComment(commentId);
        });
    }

    if (user == post.writer) {
        ui->editButton->show();
        ui->deleteButton->show();
    } else if (user == "admin") {
        ui->editButton->hide();
    } else {
        ui->editButton->hide();
        ui->deleteButton->hide();
    }

    connect(ui->backButton, &QPushButton::clicked, [this]() {
        qDebug() << "뒤로가기";
        emit back();
    });
    connect(ui->editButton, &QPushButton::clicked, [this]() {
        if (this->isEditing) {
            ui->editButton->setText("수정");
            ui->titleLineEdit->setReadOnly(true);
            ui->textEdit->setReadOnly(true);
            this->isEditing = false;
            emit editPost(this->post.postNumber, ui->titleLineEdit->text(), ui->textEdit->toPlainText());
        } else {
            ui->editButton->setText("수정 완료");
            ui->titleLineEdit->setReadOnly(false);
            ui->textEdit->setReadOnly(false);
            this->isEditing = true;
            ui->titleLineEdit->setFocus();
        }
    });
    connect(ui->deleteButton, &QPushButton::clicked, [this]() {
        emit deletePost(this->post.postNumber);
    });

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
        emit uploadComment(this->post.postNumber, ui->commentTextEdit->toPlainText());
    });
}

PostWidget::~PostWidget()
{
    delete ui;
}

int PostWidget::getPostId()
{
    return this->post.postNumber;
}