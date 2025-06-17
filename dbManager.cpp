#include "dbManager.hpp"
#include <sqlite3.h>
SQLStatement::SQLStatement(sqlite3* db, std::string cmd)
{
    if(sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_finalize(stmt);
    }
}

void SQLStatement::Launch()
{
    lr_stmt_int.clear();
    lr_stmt_float.clear();
    lr_stmt_string.clear();
    nbreturn = 0;
    //bool found = false;
    int ret_code = 0;
    int id = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        nbreturn++;
        const int cols = sqlite3_column_count(stmt);
        std::map<std::string,int> mcol_int;
        std::map<std::string,float> mcol_float;
        std::map<std::string,std::string> mcol_string;
        for(int i=0;i<cols;i++)
        {
            std::string colname = sqlite3_column_name(stmt, i);
            //sqlite3_value* v = sqlite3_column_value(stmt,i);
            const int itype = sqlite3_column_type(stmt,i);
            //printf("\ntype :%d\n",itype);
            //printf("TEST: name = %s\n", colname.c_str());
            switch(itype)
            {
                case SQLITE_NULL:
                    mcol_int[colname]=0;
                break;
                case SQLITE_INTEGER:
                    mcol_int[colname] = sqlite3_column_int(stmt,i);
                break;
                case SQLITE_FLOAT:
                    mcol_float[colname] = (float)sqlite3_column_double(stmt,i);
                break;
                case SQLITE_TEXT:
                    mcol_string[colname] = (const char*)sqlite3_column_text(stmt,i);
                break;
                default:;
            };
            //mcol[colname]= v;

            //printf("TEST: value = %s\n", sqlite3_column_text(stmt, i));
        }
        lr_stmt_int.push_back(mcol_int);
        lr_stmt_float.push_back(mcol_float);
        lr_stmt_string.push_back(mcol_string);
        id++;
    }
    if(ret_code != SQLITE_DONE) {
        //this error handling could be done better, but it works
        printf("ERROR: while performing sql: ");//%s\n", sqlite3_errmsg(db));
        printf("ret_code = %d\n", ret_code);
    }
   // return ret_code;
}

int SQLStatement::Size()
{
    return nbreturn;
}


int SQLStatement::GetInt(std::string pkey, int id)
{
    int ri=-1;
    if((int)lr_stmt_int.size()>id && id>-1)
    {
        for(const auto& [key,value] : lr_stmt_int[id])
        {
            if(key==pkey)
            {
                ri = value;
                break;
            }
        }
    }
    return ri;
}

std::string SQLStatement::GetString(std::string pkey, int id)
{
    std::string lstr;
    if(lr_stmt_string.size()>0)
    {
        for(unsigned int i=0;i<lr_stmt_string.size();i++)
        {
            if((int)i==id)
            for(const auto& [key,value] : lr_stmt_string[i])
            {
                if(key==pkey && lstr.empty())
                {
                    lstr = value;
                    break;
                }
            }
        }
    }
    return lstr;
}

float SQLStatement::GetFloat(std::string pkey, int id)
{
    float rf=-1;
    if(lr_stmt_float.size()>0)
    {
        for(unsigned int i=0;i<lr_stmt_float.size();i++)
        {
            if((int)i==id)
            for(const auto& [key,value] : lr_stmt_float[i])
            {
                if(key==pkey && rf==-1)
                {
                    rf = value;
                    break;
                }
            }
        }
    }
    return rf;
}

int DbManager::_exec(std::string cmd,char** perr)
{
    int ir = sqlite3_exec(db,cmd.c_str(),0,0,perr);
    return ir;
}

int DbManager::_exec_call(std::string cmd,int (*fcall)(void* ptr,int argc,char** sarg,char** colname),char** perr)
{
    int ir = sqlite3_exec(db,cmd.c_str(),fcall,0,perr);
    return ir;
}

DbManager::DbManager(std::string path)
{
    int rc = sqlite3_open(path.c_str(),&db);
    if(rc)
        // erreur d'ouverture
    {
        puts("DbManager : erreur db");
    }
    else
        // db ouverte
    {
        puts("DbManager : open db");
    }
}

void DbManager::CreateStatement(std::string name, std::string rqt)
{
    SQLStatement* stmt = new SQLStatement(db,rqt);
    lstmt.insert({name, stmt});
}

SQLStatement* DbManager::LoadStatement(std::string name)
{
    if(!lstmt.empty())
    {
        if(lstmt.find(name)!=lstmt.end())
        {
            lstmt[name]->Launch();
        }

    }
    return lstmt[name];
}

void DbManager::ClearStatement()
{
    lstmt.clear();
}

void DbManager::Close()
{
    sqlite3_close(db);
    puts("DbManager : close db");
    ClearStatement();
}
