#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QSettings>

const QString gMyOrganization = "trdmval";
const QString gMyAppName = "QTextCorrector";

Widget::Widget(QWidget *parent)
: QWidget(parent)
, ui(new Ui::Widget)
{
	ui->setupUi(this);
	m_corrector.m_statusLabel = ui->label_status;
	m_corrector.m_lisrWidget = ui->listWidget;
	ui->label_status->setText("");
	settings_Load();
}

Widget::~Widget(){
	settings_Save();
	delete ui;
}




void Widget::on_toolButton_clicked()
{
	QFileDialog::Options options;
	QString selectedFilter;
	QString vDir = ui->folder_dict->text();
	QString fileName = QFileDialog::getOpenFileName(this,	tr("OpenFileName"),	vDir,tr("All Files (*);;Text Files (*.txt)"),
	&selectedFilter,	options);
	if (!fileName.isEmpty())
		ui->folder_dict->setText(fileName);

}

void Widget::settings_Load(){
	QSettings vSett(gMyOrganization,gMyAppName);
	ui->folder_dict->setText(vSett.value("folder_dict").toString());
	ui->input_plainTE->setPlainText(vSett.value("inputStr").toString());


}

void Widget::settings_Save(){
	QSettings vSett(gMyOrganization,gMyAppName);
	vSett.setValue("folder_dict",ui->folder_dict->text());
	vSett.setValue("inputStr",ui->input_plainTE->toPlainText());
}

void Widget::on_pushButton_clicked()
{
	m_corrector.loadDictionary(ui->folder_dict->text());
	QString inputStr = ui->input_plainTE->toPlainText();
	QString outputStr = m_corrector.correct(inputStr);
	ui->output_plainTE->setPlainText(outputStr);
}

void Widget::on_pb_settings_clicked(){

}
