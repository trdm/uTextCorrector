#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include "uTextCorrector.h"


namespace Ui {
	class SettingsDlg;
}

class SettingsDlg : public QDialog
{
		Q_OBJECT

	public:
		explicit SettingsDlg(QWidget *parent = 0);
		~SettingsDlg();
		void setCorrector(uTextCorrector* corrector);

	protected:
		void changeEvent(QEvent *e);

	private slots:
		void on_buttonBox_accepted();

	private:
		Ui::SettingsDlg *ui;
		uTextCorrector* m_corrector;
};

#endif // SETTINGSDLG_H
