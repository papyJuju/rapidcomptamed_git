#ifndef MESSAGEALERTE_H
#define MESSAGEALERTE_H

#include <QSqlDatabase>
#include <QObject>
#include <QStringList>
#include <QString>

namespace MessageSpace{

class MessageAlerte:public QObject
{
	Q_OBJECT
	public:
		MessageAlerte (QObject * parent = 0);
		~MessageAlerte ();
		bool messageAreAvailableAndNotRead();
		bool messageAreAvailable();

	private:
	       QString messagesAvailables();
	       QString messagesAvailablesAndNotRead();
	       int showMessage();
	       int showMessageNotRead();
	       bool modifyAlertesToRead();
	       QSqlDatabase m_db;
};

};

#endif
