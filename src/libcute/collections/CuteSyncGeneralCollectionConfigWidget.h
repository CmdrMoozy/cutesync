#ifndef INCLUDE_CUTE_SYNC_GENERAL_COLLECTION_CONFIG_WIDGET_H
#define INCLUDE_CUTE_SYNC_GENERAL_COLLECTION_CONFIG_WIDGET_H

#include "CuteSyncAbstractCollectionConfigWidget.h"

class QGridLayout;
class QGroupBox;
class QCheckBox;
class QThread;

class CuteSyncGeneralCollectionConfigWidget : public CuteSyncAbstractCollectionConfigWidget
{
	Q_OBJECT
	
	public:
		CuteSyncGeneralCollectionConfigWidget(QThread *t);
		virtual ~CuteSyncGeneralCollectionConfigWidget();
		
		virtual void apply();
		virtual void reset();
		
		bool getSaveState() const;
		void setSaveState(bool s);
		
	private:
		QGridLayout *layout;
		QGroupBox *groupBox;
		QGridLayout *optionsLayout;
		QCheckBox *saveCheckBox;
	
	signals:
		void applyRequest();
		void resetRequest();
};

#endif
