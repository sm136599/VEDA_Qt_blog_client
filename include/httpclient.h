#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define SERVER_URL "http://localhost:8000"

class HttpClient : public QObject
{
    Q_OBJECT

public:
    explicit HttpClient(QObject *parent = nullptr);
    void fetchAllPosts();
    void fetchPostById(int postId);
    void join(const QString &loginId, const QString &password, const QString &passwordCheck, const QString &name);
    void login(const QString &loginId, const QString &password);
    void uploadPost(const QString &subject, const QString &writer, const QString &description);
    void editPost(int postId, const QString &subject, const QString &description);
    void uploadComment(int postId, const QString &writer, const QString &description);
    void editComment(int commentId, const QString &description);
    void deletePost(int postId);
    void deleteComment(int commentId);
    void deleteUser(const QString &user);

signals:
    void allPostsFetched(QByteArray data);
    void postFetched(QByteArray data);
    void joinResponse(QByteArray data);
    void loginResponse(QByteArray data);
    void uploadPostResponse(QByteArray data);
    void editPostResponse(QByteArray data);
    void uploadCommentResponse(QByteArray data);
    void editCommentResponse(QByteArray data);
    void deletePostResponse(QByteArray data);
    void deleteCommentResponse(QByteArray data);
    void deleteUserResponse(QByteArray data);

private slots:
    void onAllPostsFetched(QNetworkReply *reply);
    void onPostFetched(QNetworkReply *reply);
    void onJoinResponse(QNetworkReply *reply);
    void onLoginResponse(QNetworkReply *reply);
    void onUploadPostResponse(QNetworkReply *reply);
    void onEditPostResponse(QNetworkReply *reply);
    void onUploadCommentResponse(QNetworkReply *reply);
    void onEditCommentResponse(QNetworkReply *reply);
    void onDeletePostResponse(QNetworkReply *reply);
    void onDeleteCommentResponse(QNetworkReply *reply);
    void onDeleteUserResponse(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QJsonObject byteArrayToJsonObject(const QByteArray& data);
};

#endif // HTTPCLIENT_H