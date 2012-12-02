
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>



MYSQL* turn_database_connect(
                      char* server,
                      char* username,
                      char* password,
                      char* database
                      )
{
  MYSQL* conn;
  conn = mysql_init(NULL);
  if (!mysql_real_connect(
                          conn,
                          server,
                          username,
                          password,
                          database,
                          0, NULL, 0)
  )
  {
    syslog(LOG_WARNING, mysql_error(conn));
  }
  
  return conn;
}


void turn_database_disconnect(MYSQL* conn)
{
  /* close connection */
  mysql_free_result(mysql_use_result(conn));
  mysql_close(conn);
  return;
}


MYSQL_RES* turn_database_query(MYSQL* conn, char* query, int (*connect_to_database)(void))
//~ MYSQL_RES* turn_database_query(MYSQL* conn, char* query)
{
  if (mysql_query(conn, query))
  {
    //try reconnecting if fail
    syslog(LOG_WARNING, "Disconnected from database. Reconnecting...");
    (*connect_to_database)();
    if (mysql_query(conn, query))
    {
      syslog(LOG_WARNING, mysql_error(conn));
      return NULL;
    }
  }
  return mysql_use_result(conn);
}

int turn_connect_user_database()
{
  syslog(LOG_NOTICE, "Connecting to user database.");
  turn_user_database = turn_database_connect(
    __TURN_USER_DATABASE_SERVER,
    __TURN_USER_DATABASE_USERNAME,
    __TURN_USER_DATABASE_PASSWORD,
    __TURN_USER_DATABASE_NAME
  );
  syslog(LOG_NOTICE, "Connected to user database.");
  if(turn_user_database != NULL)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void turn_disconnect_user_database()
{
  turn_database_disconnect(turn_user_database);
  return;
}

char* turn_get_password(const char* userid)
{
  char* query = "";
  char* result = "";
  const char* query_begin = "select password from turn_users where userid='";
  const char* query_end = "';";
  MYSQL_RES* res;
  MYSQL_ROW row;
  int num_fields;
  
  // define query
  query = malloc(strlen(query_begin) + strlen(userid) + strlen(query_end));
  strcpy(query, query_begin);
  strcat(query, userid);
  strcat(query, query_end);
  
  //get query result
  res = turn_database_query(turn_user_database, query, &turn_connect_user_database);
  //~ res = turn_database_query(turn_user_database, query);
  if(res == NULL){
    syslog(LOG_WARNING, "Not found in database.");
    return NULL;
  }
  
  //check number of results
  num_fields = mysql_num_fields(res);
  if(num_fields != 1)
  {
    syslog(LOG_WARNING, "Number of fields expected 1, got %d. Query error.", num_fields);
    return NULL;
  }
  
  //extract password
  int num_rows=0;
  while((row = mysql_fetch_row(res)))
  {
    result = malloc(strlen(row[0]));
    strcpy(result, row[0]);
    if(++num_rows > 1)
    {
      syslog(LOG_WARNING, "Number of results bigger than 1. Database inconsistent.");
      return NULL;
    }
  }
  
  free(query);
  free(row);
  free(res);
  
  return (strlen(result)>0)?result:NULL;
}


