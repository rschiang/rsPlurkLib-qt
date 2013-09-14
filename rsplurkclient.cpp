#include "rsplurkclient.h"

const QString REQUEST_TOKEN_URL = QString("https://www.plurk.com/OAuth/request_token");
const QString AUTH_URL_BASE = QString("https://www.plurk.com/OAuth/authorize?oauth_token=%1&deviceid=%2");
const QString AUTH_URL_MOBILE_BASE = QString("https://www.plurk.com/m/authorize?oauth_token=%1&deviceid=%2");
const QString EXCHANGE_TOKEN_URL = QString("https://www.plurk.com/OAuth/access_token");
const QString API_URL_BASE = QString("https://www.plurk.com/APP/%1");

class RSPlurkClientPrivate {
public:
	RSPlurkClientPrivate() {
		//
	}

	~RSPlurkClientPrivate() {
		//
	}

	const QString appKey;
	const QString appSecret;
	QString tokenId;
	QString tokenSecret;
};

explicit RSPlurkClient::RSPlurkClient(QObject *parent = 0)
	: QObject(parent), d_ptr(new RSPlurkClientPrivate()) {
	//
}
RSPlurkClient::~RSPlurkClient() {
	delete d_ptr;
	d_ptr = 0;
}

void RSPlurkClient::setAppIdentity(const QString appKey, const QString appSecret) {
	d_ptr->appKey = appKey;
	d_ptr->appSecret = appSecret;
}
void RSPlurkClient::setToken(QString tokenId, QString tokenSecret) {
	d_ptr->tokenId = tokenId;
	d_ptr->tokenSecret = tokenSecret;
}

void RSPlurkClient::getRequestToken() {
	//
}
void RSPlurkClient::getAuthorizationUrl() {
	//
}
const QStringPair RSPlurkClient::getAccessToken(QString verifier) {
	//
}

QNetworkReply* RSPlurkClient::sendRequest(const QString endpointUrl, const QStringMap args) {
	//
}