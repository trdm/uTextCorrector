#include "settingsdlg.h"
#include "ui_settingsdlg.h"

SettingsDlg::SettingsDlg(QWidget *parent) :
QDialog(parent),
ui(new Ui::SettingsDlg)
{
	ui->setupUi(this);
}

SettingsDlg::~SettingsDlg()
{
	delete ui;
}

void SettingsDlg::setCorrector(uTextCorrector *corrector)
{
	m_corrector = corrector;
	ui->unionSentenceChar->setChecked(m_corrector->m_unionSentence);
	ui->correctWordsAndChar->setChecked(m_corrector->m_correctWordsAndChar);
	ui->maxCharInSentense->setValue(m_corrector->m_maxCharInSentense);


}

void SettingsDlg::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void SettingsDlg::on_buttonBox_accepted()
{
//	ui->maxCharInSentense->setValue();
	m_corrector->m_maxCharInSentense = ui->maxCharInSentense->value();
	m_corrector->m_unionSentence = ui->unionSentenceChar->isChecked();
	m_corrector->m_correctWordsAndChar = ui->correctWordsAndChar->isChecked();
	QDialog::accept();
}
