#pragma once
#include<windows.h>
#include "../mysql/include/mysql.h"
#pragma comment( lib, "mysql\\lib//libmysql.lib" )
//Singleton
class Database {
public:
	static Database& Instance() 
	{
		static Database db;
		return db;
	}
	
	MYSQL*getConnection() { return conn; };
	bool connect();
private:
	Database() { };
	~Database() { };
	Database(Database const&) = delete;
	Database& operator= (Database const&) = delete;

	static char *host_name;
	static char *user_name;
	static char *password;
	static char *db_name;
	static unsigned int port_num;
	static char *socket_name;
	static unsigned int flags;
	MYSQL *conn;
};