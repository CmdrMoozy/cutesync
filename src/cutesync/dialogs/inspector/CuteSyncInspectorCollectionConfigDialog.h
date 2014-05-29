#ifndef INCLUDE_CUTE_SYNC_INSPECTOR_COLLECTION_CONFIG_DIALOG_H
#define INCLUDE_CUTE_SYNC_INSPECTOR_COLLECTION_CONFIG_DIALOG_H

#include <QDialog>

class QGridLayout;
class QPushButton;

class CuteSyncAbstractCollectionConfigWidget;

/*!
 * \brief This dialog provides collection configuration support.
 *
 * We utilize a collection's built-in configuration widget in order to allow the user to change its
 * options.
 */
class CuteSyncInspectorCollectionConfigDialog : public QDialog
{
	Q_OBJECT
	
	public:
		CuteSyncInspectorCollectionConfigDialog(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncInspectorCollectionConfigDialog();
	
		void setWidget(CuteSyncAbstractCollectionConfigWidget *w);
	
	protected:
		virtual void showEvent(QShowEvent *e);
	
	private:
		QGridLayout *layout;
		
		CuteSyncAbstractCollectionConfigWidget *widget;
		
		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *okButton;
		QPushButton *applyButton;
		QPushButton *cancelButton;
	
	private slots:
		void doOk();
		void doApply();
};

#endif
