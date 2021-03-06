#ifndef RSPLURKCLIENT_H
#define RSPLURKCLIENT_H

#include <QtCore>

#include <QPair>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>

typedef QPair<QString,QString> QStringPair;
typedef QMap<QString,QString> QStringMap;

class RSPlurkClientPrivate;
class RSPlurkClient : QObject {
	Q_OBJECT
public:
    explicit RSPlurkClient(QObject *parent = 0);
    ~RSPlurkClient();

    // Application setup
    void setAppIdentity(const QString appKey, const QString appSecret);
    void setToken(QString tokenId, QString tokenSecret);
    void setNetworkAccessManager(QNetworkAccessManager* manager);

    // 3-step OAuth authentication
    void getRequestToken();
    const QString getAuthorizationUrl(QString deviceName);
    void getAccessToken(QString verifier);

    // OAuth method
    QNetworkReply* sendRequest(const QString endpointUri, QStringMap args);

signals:
    void tokenReceived(const QStringPair token);
    void error();

protected:
	QNetworkAccessManager* getNetworkAccessManager();

	QString computeSignature(const QString uri, QStringMap args);
	QString urlEncode(QString str);
	QString urlDecode(QString str);
	QStringMap parseQueryString(QString queryString);
    QNetworkRequest createRequest(const QString uri, QStringMap args);

private slots:
    void tokenCallback();
    void networkError(QNetworkReply::NetworkError error);

private:
	RSPlurkClientPrivate* d_ptr;
	Q_DECLARE_PRIVATE(RSPlurkClient)
};

#endif // RSPLURKCLIENT_H
