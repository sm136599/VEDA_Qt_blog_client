#include "postwidget.h"
#include "ui_postwidget.h"
#include "commentwidget.h"

PostWidget::PostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostWidget)
{
    ui->setupUi(this);
    clearPost();

    connect(ui->backButton, &QPushButton::clicked, [this]() {
        qDebug() << "뒤로가기";
        emit back();
    });
    connect(ui->editButton, &QPushButton::clicked, [this]() {
        if (this->isEditing) {
            // 수정 완료
            qDebug() << "수정 완료";
            ui->editButton->setText("수정");
            ui->titleLineEdit->setReadOnly(true);
            ui->textEdit->setReadOnly(true);
            this->isEditing = false;
            emit editPost(this->post.postNumber, ui->titleLineEdit->text(), ui->textEdit->toPlainText());
        } else {
            // 수정
            qDebug() << "수정";
            ui->editButton->setText("수정 완료");
            ui->titleLineEdit->setReadOnly(false);
            ui->textEdit->setReadOnly(false);
            this->isEditing = true;
            ui->titleLineEdit->setFocus();
        }
    });
    connect(ui->deleteButton, &QPushButton::clicked, [this]() {
        qDebug() << "삭제";
        emit deletePost(this->post.postNumber);
    });

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
        qDebug() << "댓글 업로드";
        if (ui->commentTextEdit->toPlainText().isEmpty()) {
            return;
        }
        emit uploadComment(this->post.postNumber, ui->commentTextEdit->toPlainText());
    });
}

PostWidget::~PostWidget()
{
    delete ui;
    for (CommentWidget* commentWidget : commentWidgetList) {
        commentWidget->deleteLater();
    }
}

int PostWidget::getPostId()
{
    return this->post.postNumber;
}

void PostWidget::clearPost()
{
    this->isEditing = false;
    ui->editButton->setText("수정");
    ui->titleLineEdit->setReadOnly(true);
    ui->textEdit->setReadOnly(true);
    ui->titleLineEdit->clear();
    ui->textEdit->clear();
    ui->writerLabel->clear();
    ui->commentTextEdit->clear();

    for (CommentWidget* commentWidget : commentWidgetList) {
        commentWidget->hide();
        commentWidget->setParent(nullptr);
        commentWidget->disconnect();
        delete commentWidget;
    }
    commentWidgetList.clear();
}

void PostWidget::setPost(const Post &post, QString user)
{
    this->post = post;
    ui->titleLineEdit->setText(post.subject);
    ui->textEdit->setText(post.description);
    ui->writerLabel->setText(post.writer);
    for (const Comment& comment : post.comments) {
        addComment(comment, user);
    }

    if (user == post.writer) {
        ui->editButton->show();
        ui->deleteButton->show();
    } else if (user == "admin") {
        ui->editButton->hide();
        ui->deleteButton->show();
    } else {
        ui->editButton->hide();
        ui->deleteButton->hide();
    }
}

void PostWidget::addComment(const Comment &comment, QString user)
{
    CommentWidget *commentWidget = new CommentWidget(comment, user);
    commentWidgetList.append(commentWidget);
    ui->scrollAreaWidgetContents->layout()->addWidget(commentWidget);
    connect(commentWidget, &CommentWidget::editComment, [this](int commentId, QString description) {
        emit editComment(commentId, description);
    });
    connect(commentWidget, &CommentWidget::deleteComment, [this](int commentId) {
        emit deleteComment(commentId);
    });
}