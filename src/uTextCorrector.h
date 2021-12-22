#ifndef UTEXTCORRECTOR_H
#define UTEXTCORRECTOR_H
#include <QtCore>
#include <QtGui>

//#include "spellchecker.h"
enum uStringType {
	uUnknown = 0,
	uCirLetter,
	uLatLetter,
	uLatCirLetter,
	uDigit,
	uOther
};


class uGroupMarker{
	public:
		uGroupMarker() {}
		uGroupMarker(QChar psMark){m_marker = psMark;}
		~uGroupMarker();
	public:
	//private:
		QChar m_marker;
		QString m_prist_1;
		QString m_prist_2;
		QString m_part4;
};

typedef QList<uGroupMarker*> uMarkerList;
typedef QMap<QChar, uMarkerList*> uMarkerMap;

class uLangPack
{
	public:
		uLangPack() {}
		uLangPack(QString& psLangName):m_langName(psLangName) {}
		bool spell(QString& psWord);
		void apendMain(QString& psStr);
		void apendAdd(QString& psStr);

		bool addMarker(QChar psMChar, QStringList &psList);
		void translateMarkers(QStringList &psList);

		uMarkerMap m_markerMap;
	//private:
		QString m_langName;
		QSet<QString> m_setMain;
		QSet<QString> m_setAdd;
};

class uTextCorrector
{
	public:
		uTextCorrector();
		bool loadDictionary(QString& psFile);
		bool loadDictionaryFile(QString& psFile, int psIsAff = 0, bool psToLC = false);
		QString correct(QString& psIn);
		QString correctWord(QString& psWord);
		QString correctWordA0(QString& psWord);
		QString correctWordA2(QString& psWord);
		bool spell(QString& psWord);
	public:
		QLabel* m_statusLabel; //, ui->label_status
		QListWidget* m_lisrWidget;
		void status(QString& psStatuStr);
	private:
		QString dictFile;
		uStringType typeString(QString& str);
		QString m_inputStr, m_outputStr;
		QSet<QString> m_setMain;
		QSet<QString> m_setAdd;
		QString m_affFilePath;
		QString m_userDictPath;
		QString m_FolderPath;

		QString m_lineCir, m_lineLat;
		int m_maxLenWord;

		uMarkerMap m_markerMap;
		//bool addMarker(QChar psMChar, QStringList& psList);
		void translateMarkers(QStringList& psList);
		void setCurrentLangPack(uStringType& psType);
		void replaceFragments(QString& psStr, bool psBefore = true);
		int replaceLat2Cir(QStringList& psList, QString &psLine);
		int replaceQuestionWord(QStringList& psList, QString &psLine);
		int replaceLat2CirString(QString &psStr);
		int kiilTransferLines(QStringList& psList);
		uLangPack* m_engPack;
		uLangPack* m_ruPack;
		uLangPack* m_currentPack;


		QStringList m_listReplBeforeChek;
		QStringList m_listReplAfterChek;

};

#endif // UTEXTCORRECTOR_H
