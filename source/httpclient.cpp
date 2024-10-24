#include "httpclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

HttpClient* HttpClient::instance = nullptr;

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    fetchAllPostsManager = new QNetworkAccessManager(this);
    fetchPostByIdManager = new QNetworkAccessManager(this);
    joinManager = new QNetworkAccessManager(this);
    loginManager = new QNetworkAccessManager(this);
    uploadPostManager = new QNetworkAccessManager(this);
    editPostManager = new QNetworkAccessManager(this);
    uploadCommentManager = new QNetworkAccessManager(this);
    editCommentManager = new QNetworkAccessManager(this);
    deletePostManager = new QNetworkAccessManager(this);
    deleteCommentManager = new QNetworkAccessManager(this);
    deleteUserManager = new QNetworkAccessManager(this);
}

HttpClient* HttpClient::getInstance()
{
    if (instance == nullptr) {
        instance = new HttpClient();
    }
    return instance;
}

QJsonObject HttpClient::byteArrayToJsonObject(const QByteArray &data)
{   
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

void HttpClient::fetchAllPosts()
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/get-all-post")));
    connect(fetchAllPostsManager, &QNetworkAccessManager::finished, this, &HttpClient::onAllPostsFetched);
    fetchAllPostsManager->get(request);
}

void HttpClient::fetchPostById(int postId)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/get-post/%1").arg(postId))));
    connect(fetchPostByIdManager, &QNetworkAccessManager::finished, this, &HttpClient::onPostFetched);
    fetchPostByIdManager->get(request);
}

void HttpClient::join(const QString &loginId, const QString &password, const QString &name)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/join")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;
    json["name"] = name;

    connect(joinManager, &QNetworkAccessManager::finished, this, &HttpClient::onJoinResponse);
    joinManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::login(const QString &loginId, const QString &password)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/login")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;

    connect(loginManager, &QNetworkAccessManager::finished, this, &HttpClient::onLoginResponse);
    loginManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadPost(const QString &subject, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["subject"] = subject;
    json["writer"] = writer;
    json["description"] = description;

    connect(uploadPostManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadPostResponse);
    uploadPostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editPost(int postId, const QString &subject, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-post"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qDebug() << "editPost" << postId << subject << description;
    QJsonObject json;
    json["postNumber"] = postId;
    json["subject"] = subject;
    json["description"] = description;

    connect(editPostManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditPostResponse);
    editPostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadComment(int postId, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;
    json["writer"] = writer;
    json["description"] = description;

    connect(uploadCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadCommentResponse);
    uploadCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editComment(int commentId, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-comment"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject json;
    json["commentNumber"] = commentId;
    json["description"] = description;

    connect(editCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditCommentResponse);
    editCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deletePost(int postId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;

    connect(deletePostManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeletePostResponse);
    deletePostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteComment(int commentId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["commentNumber"] = commentId;

    connect(deleteCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteCommentResponse);
    deleteCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteUser(const QString &user)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-user")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["user"] = user;

    connect(deleteUserManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteUserResponse);
    deleteUserManager->post(request, QJsonDocument(json).toJson());
}

// slots

void HttpClient::onAllPostsFetched(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        QJsonArray posts = response["posts"].toArray();

        QList<Post> postList;

        for (const QJsonValue &post : posts) {
            Post postStruct;
            postStruct.postNumber = post.toObject()["postNumber"].toInt();
            postStruct.subject = post.toObject()["subject"].toString();
            postStruct.writer = post.toObject()["writer"].toString();
            postList.push_back(postStruct);
        }

        emit allPostsFetched(postList);
    }
    reply->deleteLater();
}

void HttpClient::onPostFetched(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        Post post;
        post.postNumber = response["postNumber"].toInt();
        post.subject = response["subject"].toString();
        post.writer = response["writer"].toString();
        post.description = response["description"].toString();
        for (const QJsonValue &comment : response["comments"].toArray()) {
            Comment commentStruct;
            commentStruct.commentNumber = comment.toObject()["commentNumber"].toInt();
            commentStruct.writer = comment.toObject()["writer"].toString();
            commentStruct.description = comment.toObject()["description"].toString();
            post.comments.append(commentStruct);
        }

        emit postFetched(post);
    }
    reply->deleteLater();
}

void HttpClient::onJoinResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        if (response["result"].toString() == "success") {
            emit joinSucceed();
        } else {
            emit joinFailed(response["message"].toString());
        }
    }
    reply->deleteLater();
}

void HttpClient::onLoginResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        if (response["user"].toString() != "") {
            emit loginSucceed(response["user"].toString());
        } else {
            emit loginFailed();
        }
    }
    reply->deleteLater();
}

void HttpClient::onUploadPostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit uploadPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditPostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit editPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onUploadCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit uploadCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit editCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeletePostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deletePostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deleteCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteUserResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deleteUserResponse(reply->readAll());
    }
    reply->deleteLater();
}