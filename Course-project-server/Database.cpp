#pragma once
#include "Database.h"
char* Database::host_name = "localhost";
char* Database::user_name = "root";
char* Database::password = "admin"; 
char* Database::db_name = "it_step";
unsigned int Database::port_num = 3306;
char* Database::socket_name = NULL;
unsigned int Database::flags = 0;
bool Database::connect()
{
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host_name, user_name, password, db_name, port_num, socket_name, flags)))
	{
		return false;
		//fprintf(stderr, "Error : %s [%d]", mysql_error(conn), mysql_errno(conn));
	}
	else {
		return true;
	}
}
