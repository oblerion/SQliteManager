#pragma once
#include "sqlite3.h"
#include <map>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

class SQLiteStatement
{
    sqlite3_stmt* stmt;
    int nbreturn;
    std::vector<std::map<std::string,int>> lr_stmt_int;
    std::vector<std::map<std::string,float>> lr_stmt_float;
    std::vector<std::map<std::string,std::string>> lr_stmt_string;

public:
    SQLiteStatement(sqlite3* db,std::string cmd);
    void Launch();
    int Size();
    int GetInt(std::string pkey,int id);
    std::string GetString(std::string pkey,int id);
    float GetFloat(std::string pkey,int id);
};

class SQLiteManager
{
    sqlite3* db=NULL;
    std::map<std::string, SQLiteStatement*> lstmt;

public:
    int _exec_call(std::string cmd,int (*fcall)(void* ptr,int argc,char** sarg,char** colname),char** perr);
    int _exec(std::string cmd,char** perr);
    void CreateStatement(std::string name,std::string rqt);
    SQLiteStatement* LoadStatement(std::string name);
    void ClearStatement();
    SQLiteManager(std::string path);

    void Close();
};
