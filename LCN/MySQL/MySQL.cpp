/**

    @file      MySQL.cpp
    @brief     MySQL class template definitions
    @details   ~
    @author    eepsite
    @date      23.09.2022
    @copyright © eepsite, 2022. All right reserved.

**/

#include "MySQL.hpp"
#include <chrono>
#include <sstream>
#include <iostream>

void MySQL::QueryCompleted(const char* query) const
{
	auto tpSystemClock = std::chrono::system_clock::now();
	std::time_t conv_tpSystemClock = std::chrono::system_clock::to_time_t(tpSystemClock);
	std::cout << "[*][QUERY]> " << query << std::endl;
	std::cout << "[*][TIME]> " << std::ctime(&conv_tpSystemClock) << std::endl;
}

void MySQL::QueryFailed(MYSQL* connection) const
{
	std::cout << "[***][MYSQL]" << std::endl;
	std::cout << mysql_error(connection) << std::endl;
	std::cout << "[MYSQL][***]" << std::endl;
}

void MySQL::CreateTable(const std::string& tbName, const std::string& colName)
{
	int queryState;
	MYSQL* connection;
	connection = mysql_init(0);
	connection = mysql_real_connect(connection, m_Hostname, m_Username, m_Password, m_Database, m_Port, NULL, 0);
	if (connection)
	{
		std::stringstream ssQuery;
		ssQuery << "CREATE TABLE " << tbName << "(" << colName << " varchar(255));";
		std::string sQuery = ssQuery.str();
		const char* csQuery = sQuery.c_str();
		queryState = mysql_query(connection, csQuery);
		if (!queryState)
		{
			this->QueryCompleted(csQuery);
		}
		else
		{
			this->QueryFailed(connection);
		}
	}
}

void MySQL::DropTable(const std::string& tbName)
{
	int queryState;
	MYSQL* connection;
	connection = mysql_init(0);
	connection = mysql_real_connect(connection, m_Hostname, m_Username, m_Password, m_Database, m_Port, NULL, 0);
	if (connection)
	{
		std::stringstream ssQuery;
		ssQuery << "DROP TABLE " << tbName << ";";
		std::string sQuery = ssQuery.str();
		const char* csQuery = sQuery.c_str();
		queryState = mysql_query(connection, csQuery);
		if (!queryState)
		{
			this->QueryCompleted(csQuery);
		}
		else
		{
			this->QueryFailed(connection);
		}
	}
}

void MySQL::InsertKey(const std::string& tbName, const std::string& colName, const std::string& idxData)
{
	int queryState;
	MYSQL* connection;
	connection = mysql_init(0);
	connection = mysql_real_connect(connection, m_Hostname, m_Username, m_Password, m_Database, m_Port, NULL, 0);
	if (connection)
	{
		std::stringstream ssQuery;
		ssQuery << "INSERT INTO " << tbName << "(" << colName << ") VALUES" << "('" << idxData << "');";
		std::string sQuery = ssQuery.str();
		const char* csQuery = sQuery.c_str();
		queryState = mysql_query(connection, csQuery);
		if (!queryState)
		{
			this->QueryCompleted(csQuery);
		}
		else
		{
			this->QueryFailed(connection);
		}
	}
}

void MySQL::QueryTable(const std::string& tbName, const int& idxPos)
{
	int queryState;
	MYSQL* connection;
	MYSQL_ROW row;
	MYSQL_RES* result;
	connection = mysql_init(0);
	connection = mysql_real_connect(connection, m_Hostname, m_Username, m_Password, m_Database, m_Port, NULL, 0);
	if (connection)
	{
		std::stringstream ssQuery;
		ssQuery << "SELECT * FROM " << tbName << ";";
		std::string sQuery = ssQuery.str();
		const char* csQuery = sQuery.c_str();
		queryState = mysql_query(connection, csQuery);
		if (!queryState)
		{
			result = mysql_store_result(connection);
			std::cout << "[*][REQUEST]" << std::endl;
			while (row = mysql_fetch_row(result))
			{
				std::cout << row[idxPos] << std::endl;
			}
			std::cout << "[REQUEST][*]" << std::endl;
		}
		else
		{
			this->QueryFailed(connection);
		}
	}
}
