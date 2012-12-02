
//need: libmysqlclient16, libmysqlclient-dev
#include <mysql.h>

#define __TURN_USER_DATABASE_SERVER "localhost"
#define __TURN_USER_DATABASE_USERNAME "root"
#define __TURN_USER_DATABASE_PASSWORD "root"
#define __TURN_USER_DATABASE_NAME "turn"

MYSQL *turn_user_database;

MYSQL* turn_database_connect(
                      char* server,
                      char* username,
                      char* password,
                      char* database
                      );

void turn_database_disconnect(MYSQL* conn);

MYSQL_RES* turn_database_query(MYSQL* conn, char* query, int (*connect_to_database)(void));
//~ MYSQL_RES* turn_database_query(MYSQL* conn, char* query);

int turn_connect_user_database(void);

void turn_disconnect_user_database(void);

char* turn_get_password(const char* userid);


