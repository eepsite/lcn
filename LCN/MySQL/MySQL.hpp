/**

    @file      MySQL.h
    @brief     MySQL class template declarations
    @details   ~
    @author    eepsite
    @date      23.09.2022
    @copyright © eepsite, 2022. All right reserved.

**/

#pragma once

#include <mysql.h>
#include <string>

class MySQL
{
protected:
	const char* m_Hostname;
	const char* m_Username;
	const char* m_Password;
	const char* m_Database;
	int m_Port;
public:
	MySQL(const char* hostname, const char* username, const char* password, const char* database, int port)
		: m_Hostname(hostname), m_Username(username), m_Password(password), m_Database(database), m_Port(port) {}
	~MySQL() {}
public:
	void QueryCompleted(const char* query) const;
	void QueryFailed(MYSQL* connection) const;
	void CreateTable(const std::string& tbName, const std::string& colName);
	void DropTable(const std::string& tbName);
	void InsertKey(const std::string& tbName, const std::string& colName, const std::string& idxData);
	void QueryTable(const std::string& tbName, const int& idxPos);
};

