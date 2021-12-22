#include "uTextCorrector.h"
#include <QDebug>

const QString gCirLetters = QString::fromUtf8("абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ");
const QString gLatLetters = QString::fromUtf8("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

const QString gCirLettersForm = QString::fromUtf8("аеогпрсхубАЕОРСХУ");
const QString gLatLettersForm = QString::fromUtf8("aeornpcxy6AEOPCXY");

const QString gMarkerY = QString::fromUtf8("Y");
const QString gMarkerSFX = QString::fromUtf8("SFX");
const QString gMarker0 = QString::fromUtf8("0");
const QString gMarkerQuestion = QString::fromUtf8("?");
const QString gMarkerTrasfer = QString::fromUtf8("­­");
const QString gMarkerBrahma = QString::fromUtf8("Брахма");


uTextCorrector::uTextCorrector(){
	m_maxLenWord = 0;
	m_listReplBeforeChek  << ",\n ,"<<".\n ." << ":\n :" << "-\n - "<<")\n ) "<<"(\n ( "<<"<\n < "<<">\n > "<<"«\n « "<<"»\n » " ;
	m_listReplAfterChek << "  \n "<< " ,\n,"<<" .\n." << " :\n:"<<" < \n<"<<" > \n>"<<"« \n«"<<"» \n»" ;
	m_engPack = new uLangPack(QString::fromUtf8("en"));
	m_ruPack = new uLangPack(QString::fromUtf8("ru"));
	m_currentPack = m_ruPack;
	m_lisrWidget = 0;
	m_statusLabel = 0;


}

bool uTextCorrector::loadDictionary(QString &psFile)
{
	bool vRetVal = false;
	if (dictFile.compare(psFile) == 0) {
		return true;
	}
	if (!QFile::exists(psFile)) {
		return false;
	}
	QString vDir;
	QFileInfo vFileInfo(psFile);
	if (vFileInfo.isFile()) {
		vDir = vFileInfo.absoluteDir().absolutePath();
	} else {
		vDir = psFile;
	}
	dictFile = psFile;

	m_FolderPath = vDir;
	QString vSep = "\\/";
	QChar vr = m_FolderPath.right(1).at(0);
	int vIndex = vSep.indexOf(vr);
	if (vIndex == 0) {
		m_FolderPath.append("\\");
	} else if (vIndex == 1) {
		m_FolderPath.append("/");
	} else if (m_FolderPath.indexOf("/") != -1) {
		m_FolderPath.append("/");
	}
	m_affFilePath = m_FolderPath;
	m_affFilePath.append(QString::fromUtf8("ru_RU.aff"));

	m_userDictPath = m_FolderPath;
	m_userDictPath.append("UserDic.dic");

	QString vStatus = QString::fromUtf8("load start");
	qDebug() << vStatus << QDateTime::currentDateTime().toString();
	m_statusLabel->setText(vStatus);
	qApp->processEvents();
	//en_US.aff
	m_currentPack = m_ruPack;
	vRetVal = vRetVal & loadDictionaryFile(m_affFilePath,1);
	vRetVal = vRetVal & loadDictionaryFile(psFile,0);
	vRetVal = vRetVal & loadDictionaryFile(m_userDictPath,2,true);

	m_currentPack = m_engPack;
	m_affFilePath = m_FolderPath;
	m_affFilePath.append(QString::fromUtf8("en_US.aff"));
	vRetVal = vRetVal & loadDictionaryFile(m_affFilePath,1);
	m_affFilePath = m_FolderPath;
	m_affFilePath.append(QString::fromUtf8("en_US.dic"));
	vRetVal = vRetVal & loadDictionaryFile(m_affFilePath,0, true);

	vStatus = QString::fromUtf8("load end");
	qDebug() << vStatus << QDateTime::currentDateTime().toString();
	m_statusLabel->setText(vStatus);
	qApp->processEvents();
	return vRetVal;
}

bool uTextCorrector::loadDictionaryFile(QString &psFile, int psIsAff, bool psToLC){
	QFile vTxtFile(psFile);
	if (!vTxtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}
	QFileInfo vFileInfo(psFile);
	qint64 vSize = vFileInfo.size(), vSizeAllRedy = 0;

	QString vStatus, vStatus0 = QString::fromUtf8("Load: ");
	vStatus0.append(vFileInfo.baseName());
	vStatus.append(vStatus0);
	m_statusLabel->setText(vStatus);
	qApp->processEvents();
	//qDebug() << "load start" << QDateTime::currentDateTime().toString() << vFileInfo.fileName();
	QTextStream in(&vTxtFile);
	QTextCodec* defaultTextCodec = QTextCodec::codecForName("UTF-8");
	in.setCodec(defaultTextCodec);
	QString line_orig, line;
	QStringList fields;
	int vLines = 0;
	while(!in.atEnd()) {
		line_orig = in.readLine();
		vLines++;
		if (line_orig.isEmpty()) {
			continue;
		}
		if (psToLC) {
			line_orig = line_orig.toLower();
		}
		if (psIsAff == 0) {
			//line = defaultTextCodec->toUnicode(in.readLine());
			if (line_orig.indexOf("/") != -1) {
				fields = line_orig.split("/");
				translateMarkers(fields);
				line_orig = fields.at(0);
				//MapAff.insert(line_orig,fields.at(1));
			}
			m_maxLenWord = qMax(m_maxLenWord, line_orig.length());

			m_currentPack->apendMain(line_orig);
			//			m_setMain.insert(line_orig);
		} else if (psIsAff == 1) {

			line = line_orig;
			while (line.indexOf("  ") != -1) {
				line = line.replace("  "," ");
			}
			fields = line.split(" ");
			if (fields.size() >= 4 ) {
				if (fields.at(0) == gMarkerSFX) {
					if (fields.at(2) == gMarkerY) {
						continue;
					}
					m_currentPack->addMarker(fields.at(1).at(0),fields);
				}
			}
		}else if (psIsAff == 2) {
			// userdict
			if (vLines > 1) {
				m_maxLenWord = qMax(m_maxLenWord, line_orig.length());
				m_currentPack->apendMain(line_orig);
			}
		}

		if (vLines % 1000 == 0) {
			//vSizeAllRedy = in.pos();
			vStatus = vStatus0;
			vStatus.append(":").append(line_orig);
			m_statusLabel->setText(vStatus);
			qApp->processEvents();
		}
	}
//	vSizeAllRedy = in.pos();
//	qDebug() << "vSizeAllRedy  " << vSizeAllRedy  << "vSize " << vSize << psFile;
	vStatus = QString::fromUtf8("load end");
	vStatus.append(vFileInfo.baseName());
	m_statusLabel->setText(vStatus);
	qApp->processEvents();

	vTxtFile.close();
	return true;

}




// trdm 2021-11-13 10:48:09
bool uTextCorrector::spell(QString &psWord){
	if (psWord.isEmpty()) {
		return false;
	}
	return m_currentPack->spell(psWord);
}

void uTextCorrector::status(QString &psStatuStr)
{
	qApp->processEvents();
	m_statusLabel->setText(psStatuStr);
	qApp->processEvents();
}


QString uTextCorrector::correct(QString &psIn)
{
	m_inputStr = psIn, m_outputStr = "";
	QString line, vWord, vWordLW, vWordFR, line_part, line_partLC;
	uStringType vType, vCurrType;
	int vPartCnt, vPartCntW;
	QStack<QString> vWordStack;
	QMap<QString,QString> vWordMapFounds;
	QMap<QString,QString> vWordMapFoundsLC;
	QStringList strings_txt = m_inputStr.split('\n'), line_items;
	kiilTransferLines(strings_txt);
	int vReplaceCuonter = 0, vLineCounter = 0, vLineAll = strings_txt.size();
	m_lisrWidget->clear();
	foreach (line, strings_txt) {		
		while (line.indexOf("  ") != -1) {
			line = line.replace("  "," ");
		}
		vLineCounter++;
		if (vLineCounter % 50 == 0) {
			vWord = QString::fromUtf8("Обработано строк: %1 из %2").arg(vLineCounter).arg(vLineAll);
			status(vWord);
			vWord = "";
		}
		if (vLineCounter == 24) {
			vLineCounter++;
			vLineCounter--;
		}
		line = line.replace("  "," ");
		replaceFragments(line, true);	//	line = line.replace(","," ,");		line = line.replace("."," .");		line = line.replace(":"," :");
		//qDebug() << "line: " << vLineCounter << line;
top_step:
		line_items = line.split(" ");		vPartCnt = line_items.count();
		vReplaceCuonter += replaceLat2Cir(line_items, line);
		vReplaceCuonter += replaceQuestionWord(line_items, line);
		if (line_items.size() > 0) {
			bool vHasRplc = false;
			for (int il4 = 0; il4 < vPartCnt; ++il4) {
				vWord = line_items.at(il4);
				vWordLW = vWord;
				//vWordLW = vWordLW.toLower();
				if (!spell(vWordLW)) {
					/*	Однажды царь полубогов, Индра, и царь демонов, Вирочана, обратилиськБрахме, желая обрестизнание обатме,
					или своем собственном «Я». Решив проверить, насколько они разумны, Брахма сказал, что «я» – это отражение,
						котороемы видимвзеркалеиливсосудесводой. ГлупыйВирочанадовольный вернулся в своецарство и былпровозглашенгуру среди демонов. */
					line_partLC = correctWord(vWordLW);
					if (line_partLC.compare(vWordLW) != 0) {
						vHasRplc = true;
						line = line.replace(vWordLW, line_partLC, Qt::CaseInsensitive);
					}
				}
			}
			if (vHasRplc) {
				line_items = line.split(" ");		vPartCnt = line_items.count();
			}
		}
		int vCnt2, vCnt3;

		for (vCnt2 = 0; vCnt2 < vPartCnt; ++vCnt2) { // строки
			vWord = ""; vPartCntW = 0; vWordFR = "";
			if (vWordStack.size() > 0) {
				//vCnt2 = vCnt3;
				vWord = vWordStack.pop();
				//qDebug() << "Test (vWordStack.pop): " << vWord;
				if (vWordMapFoundsLC.contains(vWord)) {
					vWord = vWordMapFoundsLC.value(vWord);
				}
				if (vWordMapFounds.contains(vWord)) {
					vWordFR = vWordMapFounds.value(vWord);
					line = line.replace(vWordFR, vWord);
					vReplaceCuonter++;
				}
			}
			vWord = ""; vPartCntW = 0; vWordFR = "";
			vCurrType = uUnknown;
			vWordMapFounds.clear();
			vWordMapFoundsLC.clear();
			vWordStack.clear(); // << тут ловить самую большую слову..
			bool vFound = false;
			for (vCnt3 = vCnt2; vCnt3 < vPartCnt; ++vCnt3) { // слова.
				vFound = false;
				line_part = line_items.at(vCnt3);
				vType = typeString(line_part);
				if (!(vType == uCirLetter || vType == uLatLetter)) {
					if (vType == uLatCirLetter) {
						line_part.append("(").append(m_lineCir).append(" | ").append(m_lineLat).append(")");
						m_lisrWidget->addItem(line_part);
					}
					break;
				}
				if (vCurrType == uUnknown) {
					vCurrType = vType;
					setCurrentLangPack(vType);
				} else if (vCurrType != vType) {
					break;
				}
				if (line_part.length() > 5) {
					line_partLC = line_part;
					if (spell(line_part)) {
						continue;
					}
					line_partLC = line_partLC.toLower();
					vWordLW = line_partLC;
				}
				vWord.append(line_part);
				vWordFR.append(line_part).append(" ");
				vPartCntW++;
				if (vWord.length() == 1) {	continue;					}

				vWordLW = vWord;
				vWordLW = vWordLW.toLower();
				if (spell(vWordLW)) {
					vCnt2 = vCnt3;
					vWordStack.push(vWordLW);
					vWordMapFounds.insert(vWord, vWordFR.trimmed());
					vWordMapFoundsLC.insert(vWordLW,vWord);
					//qDebug() << "Found-2: " << vWord << "vCnt2:" << vCnt2;
				}
			}
			if (vWordStack.size() > 0) {
				vWord = vWordStack.pop();
				if (vWordMapFoundsLC.contains(vWord)) {
					vWord = vWordMapFoundsLC.value(vWord);
				}
				if (vWordMapFounds.contains(vWord)) {
					vWordFR = vWordMapFounds.value(vWord);
					line = line.replace(vWordFR, vWord);
					vReplaceCuonter++;
				}
			}

		}
		replaceFragments(line, false);	//			line = line.replace(" ,",",");		line = line.replace(" .",".");		line = line.replace(" :",":");

		m_outputStr.append(line);
		m_outputStr.append('\n');
	}
	QString vStat = QString::fromUtf8("Replace cuonter: %1").arg(vReplaceCuonter);
	m_statusLabel->setText(vStat);
	return m_outputStr;
}
//\todo новый алгоритм, выделить максимальные слова по длине сначала
QString uTextCorrector::correctWord(QString &psWord)
{
	correctWordA2(psWord);
	return correctWordA0(psWord);
}

QString uTextCorrector::correctWordA0(QString &psWord)
{
	QString vWordRes = psWord, vWordTmp = "", vWordTmp2 = "", vWordCheck = "";
	if (psWord.isEmpty()) {
		return psWord;
	}
	QStringList vList;
	QStack<QString> vWordStack;
	QChar vSChar;
	bool vIsRedy = false;
	while (!vIsRedy) {
		vList = vWordRes.split(" ");
		if (vList.isEmpty()) {
			break;
		}
		vWordCheck = vList.back();
		if (vWordCheck.isEmpty()) {
			break;
		}
		vSChar = vWordCheck.at(0).toUpper();
		for (int i = 0; i < vWordCheck.length(); ++i) {
			vWordTmp = vWordCheck.mid(0,i+1);
			if (spell(vWordTmp)) {
				vWordStack.push(vWordTmp);
			} else if (vWordTmp.length() > 1){
				vWordTmp2 = vWordTmp;
				vWordTmp2 = vSChar + vWordTmp2.mid(1,vWordTmp2.length()-1);
				if (spell(vWordTmp2)) {
					vWordStack.push(vWordTmp2);
				}
			}
		}
		if (vWordStack.size()>0) {
			vWordTmp = vWordStack.pop();
			vWordTmp2 = vWordCheck;
			vWordCheck.insert(vWordTmp.length()," ");
			vWordRes = vWordRes.replace(vWordTmp2, vWordCheck);
			if (false) {
				vWordCheck = vWordStack.pop();
				vWordTmp = vWordCheck;
				vWordTmp.append(" ");
				vWordRes = vWordRes.replace(vWordCheck,vWordTmp);
			}
			vWordStack.clear();
			continue;
		}
		break;
	}

	return vWordRes;
}

bool lengthLessThan(const QString &s1, const QString &s2)
{
	return s1.length() > s2.length();
}


QString uTextCorrector::correctWordA2(QString &psWord)
{
	QString vWordRes = psWord, vWordTmp = "", vWordTmp2 = "", vWordCheck = "";
	if (psWord.isEmpty()) {
		return psWord;
	}
	vWordRes = vWordRes.trimmed();
	QStringList vList;
	int vLen = 0, vLenWord = vWordRes.length();
	// m_maxLenWord
	QChar vSChar;
	QStack<QString> vWordStack;
	for (int var = 0; var < vLenWord; ++var) {
		vSChar = vWordRes.at(var).toUpper();
		for (int var2 = 3; var2 < m_maxLenWord; ++var2) {
			if (var+var2 <= vLenWord) {
				vWordTmp = vWordRes.mid(var,var2);
				if (spell(vWordTmp)) {
					vWordStack.push(vWordTmp);
//					if (!vList.contains(vWordTmp)) {
//						vList.append(vWordTmp);
//					}
				} else {
					vWordTmp2 = vWordTmp;
					vWordTmp2 = vSChar + vWordTmp2.mid(1,vWordTmp2.length()-1);
					if (spell(vWordTmp2)) {
						vWordStack.push(vWordTmp);
//						if (!vList.contains(vWordTmp2)) {
//							vList.append(vWordTmp2);
//						}
					}
				}
			}
		}
		if (vWordStack.size() > 0) {
			vWordTmp = vWordStack.top();
			if (!vList.contains(vWordTmp)) {
				vList.append(vWordTmp);
			}
		}
	}
	if (vList.size() > 0) {
		qSort(vList.begin(), vList.end(), lengthLessThan);

	}

	return vWordRes;
}


uStringType uTextCorrector::typeString(QString &str){
	uStringType vType = uOther, vTypeChr, vTypeRet = uOther;
	QChar vChr;
	m_lineCir = "Ru:", m_lineLat = "En:";
	int vCntrCir = 0, vCntrLat = 0;
	for (int var = 0; var < str.length(); ++var) {
		vChr = str.at(var);
		vTypeChr = uOther;
		if (vChr.isDigit()) {
			vTypeChr = uDigit;
		} else if (vChr.isLetter()) {
			vTypeChr = uOther;
			if (gCirLetters.indexOf(vChr) != -1) {
				vTypeChr = uCirLetter;
				m_lineCir.append(vChr);
				m_lineLat.append('_');
				vCntrCir++;
			} else if (gLatLetters.indexOf(vChr) != -1) {
				m_lineCir.append('_');
				m_lineLat.append(vChr);
				vTypeChr = uLatLetter;
				 vCntrLat++;
			}
		}
		if (var == 0) {
			vType = vTypeChr;
			vTypeRet = vType;
		} else {
			if (vType != vTypeChr) {
				if (vCntrLat > 0 &&  vCntrCir > 0) {
					vTypeRet = uLatCirLetter;
				}
				vTypeRet = uOther;
			}
		}
	}
	return vTypeRet;
}


void uTextCorrector::translateMarkers(QStringList &psList)
{
	m_currentPack->translateMarkers(psList);
	return;
}

void uTextCorrector::setCurrentLangPack(uStringType &psType){
	if (psType == uCirLetter) {
		m_currentPack = m_ruPack;
	} else
		m_currentPack = m_engPack;
}

void uTextCorrector::replaceFragments(QString &psStr, bool psBefore){
	QStringList vItems,list = psBefore ? m_listReplBeforeChek : m_listReplAfterChek;
	QString vListItem;
	foreach (vListItem, list) {
		vItems = vListItem.split("\n");
		if (!psBefore) {
			while (psStr.indexOf(vItems.at(0)) != -1) {
				psStr = psStr.replace(vItems.at(0),vItems.at(1));
			}
		} else
			psStr = psStr.replace(vItems.at(0),vItems.at(1));
	}
}

int uTextCorrector::replaceLat2CirString(QString &psStr)
{
	int vRv = 0, vCntrCir = 0, vCntrLat = 0;
	QChar vc, vCc, vCl;
	vCntrCir = 0, vCntrLat = 0;
	for (int ii = 0; ii < psStr.length(); ++ii) {
		vc = psStr.at(ii);
		if (vc.isLetter() || vc.isDigit()) { // vc.isDigit() '6' ~ 'б'
			if (gCirLetters.indexOf(vc) != -1) {
				vCntrCir++;
			} else if (gLatLetters.indexOf(vc) != -1) {
				vCntrLat++;
			}
		}
	}
	if (vCntrCir > 0 && vCntrLat > 0) {
		vRv++;
		for (int i2 = 0; i2 < gLatLettersForm.length(); ++i2) {
			vCl = gLatLettersForm.at(i2);
			vCc = gCirLettersForm.at(i2);
			psStr = psStr.replace(vCl,vCc);
		}
	}
	return vRv;
}

int uTextCorrector::kiilTransferLines(QStringList &psList)
{
	int vRv = 0;
	QString vLine_1, vLine_2, vWordAdd, vStr = "";
	vStr.append(QChar(0x00ad));
	QStringList vWordList;

	for (int ilc = 0; ilc < psList.length()-1; ++ilc) {
		vLine_1 = psList.at(ilc);
		vLine_1 = vLine_1.trimmed();
		vLine_2 = psList.at(ilc+1);
		if (vLine_1.endsWith(vStr) || vLine_1.endsWith("-")) {
			vLine_1 = vLine_1.left(vLine_1.length()-1);
			vWordList = vLine_2.split(" ");
			vLine_1.append(vWordList.at(0)).append(" ");
			vWordList.removeAt(0);
			vLine_2 = vWordList.join(" ");
			psList.replace(ilc,vLine_1);
			psList.replace(ilc+1,vLine_2);
			continue;
		}
	}
	return vRv;
}


int uTextCorrector::replaceLat2Cir(QStringList& psList, QString& psLine)
{
	int vRv = 0, vRvs;
	//void QList::replace ( int i, const T & value )

	// _ _pynnax n__________ _ _.
	// и г______ _редставлен и й
	QString vItem, vItemOrigin;
	for (int var = 0; var < psList.count(); ++var) {
		vItem = psList.at(var);
		if (vItem.length() < 2) {
			continue;
		}
		vItemOrigin = vItem;
		vRvs = replaceLat2CirString(vItem);
		if (vRvs>0) {
			vRv++;
			psList.replace(var,vItem);
			psLine.replace(vItemOrigin,vItem);
		}
	}
	return vRv;
}

int uTextCorrector::replaceQuestionWord(QStringList &psList, QString &psLine)
{
	/*
	  Подкат?ог res/values/ ................................... 37
	  1.2.3. Функцион?ьные библиотеки ...................... 11
	  2.2.2. Инструменты ?я разработки и отладки приложений ................................... 22
	*/
	QString vWord, vWordOfSet,vWordFound, vWord_s,vWord_e;
	int vIndex = 0, vRetval = 0, vOkCounter = 0, vWordLen = 0, vWordNextLen = 0;
	QStringList vListWPart;
	uStringType vStype;
	uLangPack* vLangPack;
	QSet<QString>::const_iterator iter;
	for (int var = 0; var < psList.size(); ++var) {
		vWord = psList.at(var);
		vIndex = vWord.indexOf(gMarkerQuestion);
		if (vIndex == -1 || vWord.length() < 3) {
			continue;
		}
		vWordLen = vWord.length();
		vListWPart = vWord.split(gMarkerQuestion);
		if (vListWPart.size() != 2) {
			continue;
		}
		vWord_s = vWord;
		vWord_s = vWord_s.replace("?","");
		vStype = typeString(vWord_s);
		vLangPack = m_engPack;
		if (vStype == uCirLetter) {
			vLangPack = m_ruPack;
		}

		vWord_s = vListWPart.at(0);
		vWord_e = vListWPart.at(1);
		vWordFound = "";
		iter = vLangPack->m_setMain.constBegin();
		while (iter != vLangPack->m_setMain.constEnd()) {
			vOkCounter = 0;
			vWordOfSet = *iter;
			vWordNextLen = vWordOfSet.length();
			if (vWord_s.isEmpty()) {
				vOkCounter++;
			} else if (vWordOfSet.startsWith(vWord_s)) {
				vOkCounter++;
			}
			if (vWord_e.isEmpty()) {
				vOkCounter++;
			} else if (vWordOfSet.endsWith(vWord_e)) {
				vOkCounter++;
			}
			if (vWordLen+1 == vWordNextLen && vOkCounter == 2) {
				vWordFound = vWordOfSet;
				qDebug() << "Found for: " << vWord << vWordFound;
				break;
			}
			iter++;
		}
		if (vWordFound.isEmpty()) {
			iter = vLangPack->m_setAdd.constBegin();
			while (iter != vLangPack->m_setAdd.constEnd()) {
				vOkCounter = 0;
				vWordOfSet = *iter;
				vWordNextLen = vWordOfSet.length();
				if (vWord_s.isEmpty()) {
					vOkCounter++;
				} else if (vWordOfSet.startsWith(vWord_s)) {
					vOkCounter++;
				}
				if (vWord_e.isEmpty()) {
					vOkCounter++;
				} else if (vWordOfSet.endsWith(vWord_e)) {
					vOkCounter++;
				}
				//if (vWordLen+1 == vWordNextLen && vOkCounter == 2) {
				if (vOkCounter == 2/* &&   */) {
					if (vWordLen <= vWordNextLen && vWordNextLen-vWordLen<3) {
						vWordFound = vWordOfSet;
						qDebug() << "Found for: " << vWord <<" - "<< vWordFound << psLine;
						break;
						vOkCounter++;
						vOkCounter--;
					}
				}
				iter++;
			}
		}
		if (!vWordFound.isEmpty()) {
			psList.replace(var,vWordFound);
			psLine.replace(vWord,vWordFound);
			vRetval++;
		} else {
			qDebug() << "Not found for: " << vWord << psLine;
		}
	}
	return vRetval;
}

// trdm 2021-11-14 10:24:33
bool uLangPack::spell(QString &psWord)
{
	bool vRetVal = false;
	QString vWord = psWord;
	if (m_setMain.contains(vWord)) {
		vRetVal = true;
	} else if (m_setAdd.contains(vWord)) {
		vRetVal = true;
	}
	vWord = vWord.toLower();
	if (m_setMain.contains(vWord)) {
		vRetVal = true;
	} else if (m_setAdd.contains(vWord)) {
		vRetVal = true;
	}
	return vRetVal;

}

// trdm 2021-11-14 10:24:39
void uLangPack::apendMain(QString &psStr){
	m_setMain.insert(psStr);
}

// trdm 2021-11-14 10:24:44
void uLangPack::apendAdd(QString &psStr){
	m_setAdd.insert(psStr);
}

bool uLangPack::addMarker(QChar psMChar, QStringList &psList)
{
	uMarkerList* list = 0;
	if (m_markerMap.contains(psMChar)) {
		list = m_markerMap.find(psMChar).value();
	} else {
		list = new uMarkerList;
		m_markerMap.insert(psMChar,list);
	}
	uGroupMarker* marker = new uGroupMarker(psMChar);
	marker->m_prist_1 = psList.at(2);
	marker->m_prist_2 = psList.at(3);
	if (psList.length() >4) {
		marker->m_part4 = psList.at(4);
	}
	list->append(marker);
	return true;

}

void uLangPack::translateMarkers(QStringList &psList)
{
	//QSet<QString> m_setAdd;
//	прогрессирующий/AZ
//	прогрессировавший/A
//	прогрессировать/LQTU
	if (psList.size() != 2) {
		return;
	}
	QString vWord_orig,vWord = psList.at(0);
	QString vMarks = psList.at(1);
	QString vPr_1, vPr_2;
	QChar vChM;
	uMarkerList* list = 0;
	uGroupMarker* vGm = 0;
	if (vWord.compare(gMarkerBrahma) == 0) {
		int stoper = 2;
	}
	int vLenWord = vWord.length();
	for (int var = 0; var < vMarks.length(); ++var) {
		vChM = vMarks.at(var);
		if (m_markerMap.contains(vChM)) {
			list = m_markerMap.find(vChM).value();
		} else {
			continue;
		}
		for (int ii = 0; ii < list->length(); ++ii) {
			vGm = list->at(ii);
			vPr_1 = vGm->m_prist_1;
			vPr_2 = vGm->m_prist_2;
			if (vPr_2 == gMarker0) {
				vPr_2 = "";
			}
			if (vWord.endsWith(vPr_1)) {
				vWord_orig = vWord.left(vLenWord-vPr_1.length());
				vWord_orig.append(vPr_2);
				m_setAdd.insert(vWord_orig);
			} else if (vPr_1 == gMarker0) {
				vWord_orig = vWord;
				vWord_orig.append(vPr_2);
				m_setAdd.insert(vWord_orig);
			}
		}
	}
}
