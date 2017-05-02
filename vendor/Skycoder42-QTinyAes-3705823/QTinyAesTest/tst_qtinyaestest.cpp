#include <QString>
#include <QtTest>
#include <qtinyaes.h>

#define TEST_ROUNDS 256

class QTinyAesTest : public QObject
{
	Q_OBJECT

public:
	QTinyAesTest();

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testCBC_data();
	void testCBC();
	void testECB_data();
	void testECB();

	void tinyAEStest();

private:
	QByteArray generateData(int size);

	QTinyAes aes;
};

QTinyAesTest::QTinyAesTest(){}

void QTinyAesTest::initTestCase()
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
}

void QTinyAesTest::cleanupTestCase()
{
}

void QTinyAesTest::testCBC_data()
{
	QTest::addColumn<QByteArray>("plain");
	QTest::addColumn<QByteArray>("key");
	QTest::addColumn<QByteArray>("iv");

	for(int i = 0; i < TEST_ROUNDS; i++) {
		QTest::newRow(qPrintable(QStringLiteral("CBC_%1").arg(i)))
				<< generateData(i * i)
				<< generateData(16)
				<< generateData(16);
	}
}

void QTinyAesTest::testCBC()
{
	QFETCH(QByteArray, plain);
	QFETCH(QByteArray, key);
	QFETCH(QByteArray, iv);

	QByteArray cipher;
	QByteArray result;

	this->aes.setMode(QTinyAes::CBC);
	this->aes.setKey(key);
	this->aes.setIv(iv);

	cipher = aes.encrypt(plain);
	result = aes.decrypt(cipher);

	QCOMPARE(plain, result);
}

void QTinyAesTest::testECB_data()
{
	QTest::addColumn<QByteArray>("plain");
	QTest::addColumn<QByteArray>("key");
	QTest::addColumn<QByteArray>("iv");

	for(int i = 0; i < TEST_ROUNDS; i++) {
		QTest::newRow(qPrintable(QStringLiteral("ECB_%1").arg(i)))
				<< generateData(i * i)
				<< generateData(16)
				<< generateData(16);
	}
}

void QTinyAesTest::testECB()
{
	QFETCH(QByteArray, plain);
	QFETCH(QByteArray, key);
	QFETCH(QByteArray, iv);

	QByteArray cipher;
	QByteArray result;

	this->aes.setMode(QTinyAes::ECB);
	this->aes.setKey(key);
	this->aes.setIv(iv);

	cipher = aes.encrypt(plain);
	result = aes.decrypt(cipher);

	QCOMPARE(plain, result);
}

QByteArray QTinyAesTest::generateData(int size)
{
	QByteArray data(size, Qt::Uninitialized);
	for(int i = 0; i < size; i++)
		data[i] = (char)qrand();
	return data;
}

QTEST_APPLESS_MAIN(QTinyAesTest)

#include "tst_qtinyaestest.moc"


extern "C" {
#define main mainDelegate
#include "../QTinyAes/tiny-AES128-C/test.c"
}
void QTinyAesTest::tinyAEStest()
{
	test_encrypt_cbc();
	test_decrypt_cbc();
	test_decrypt_ecb();
	test_encrypt_ecb();
}
