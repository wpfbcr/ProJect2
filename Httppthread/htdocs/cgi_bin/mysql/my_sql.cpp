#include"my_sql.h"
using namespace std;


	HttpSql::HttpSql()
	{
		mysql=mysql_init(NULL);
		if(!mysql_real_connect(mysql,"localhost","wpfbcr","123","httpsql",0,NULL,0))
		{
			cout<<"mysql_real_connect deafult"<<endl;
			exit(1);
		}
	}
	bool HttpSql::mysql_start()
	{
		if(mysql_query(mysql,"SET autocommit=0")==0)
		{
			cout<<"start success"<<endl;
			return true;
		}
		else {
			return false;
		}

	}
	bool HttpSql::mysql_begin()
	{
		if(mysql_query(mysql,"BEGIN")==0)
		{
			cout<<"begin success"<<endl;
			return true;
		}
		else{
			return false;
		}
	}
	bool HttpSql::mysql_commit()
	{
		if(mysql_query(mysql,"COMMIT")==0)
		{
			cout<<"commit success"<<endl;
			return true;
		}
		else{
			return false;
		}
	}
	bool HttpSql::mysql_rollback()
	{
		if(mysql_query(mysql,"ROLLBACK")==0)
		{
			cout<<"rollback success"<<endl;
			return true;
		}
		else
		{
			return false;
		}
	}
	bool HttpSql::mysql_op(string str)
	{
		//acout<<str<<endl;
		int ret=mysql_query(mysql,str.c_str());
		if(ret==0)
		{
			return true;
		}
		else{
			return false;
		}
	}
	bool HttpSql:: mysql_insert(string _str)
	{
		//cout<<_str<<endl;
		return mysql_op(_str);
	}
	bool HttpSql::mysql_select(string str)
	{
		bool ret=false;
		char nullbuf[1024];
		memset(nullbuf,'\0',sizeof(nullbuf));
		strcpy(nullbuf,str.c_str());
		if(nullbuf[0]=='\0')
		{
			string _str="select * from test_info";
			ret=mysql_op(_str);
		}else
		{
			string _str="select * from test_info where ";
			cout<<str<<endl;
			_str+=str.c_str();
			cout<<_str<<endl;
			ret=mysql_op(_str);
		}
		if(ret)
		{
			res=mysql_store_result(mysql);

			int rows=mysql_num_rows(res);
			int fields=mysql_num_fields(res);

			MYSQL_FIELD *fd=NULL;
			for(;fd=mysql_fetch_field(res);)
			{
				cout<<fd->name<<'\t'<<'\t';
			}
			cout<<endl;
			while(rows)
			{
			MYSQL_ROW row=mysql_fetch_row(res);
			rows--;
			int i=0;
			for(;i<fields;i++)
			{
				cout<<row[i]<<'\t'<<"      ";
			}
			cout<<endl;
			}
		}
	}
	bool HttpSql::mysql_modify(string str)
	{
		bool ret=false;
		string _str="update test_info set ";
	//	cout<<str<<endl;
		_str+=str.c_str();
	//	cout<<_str<<endl;
		ret=mysql_op(_str);
		if(ret)
		{
			return 1;
		}else{
			return -1;
		}
//		if(ret)
//		{
//			res=mysql_store_result(mysql);
//
//
//			int rows=mysql_num_rows(res);
//			int fields=mysql_num_fields(res);
//
//			MYSQL_FIELD *fd=NULL;
//			for(;fd=mysql_fetch_field(res);)
//			{
//				cout<<fd->name<<'\t'<<'\t';
//			}
//			cout<<endl;
//			while(rows)
//			{
//			MYSQL_ROW row=mysql_fetch_row(res);
//			rows--;
//			int i=0;
//			for(;i<fields;i++)
//			{
//				cout<<row[i]<<'\t'<<"      ";
//			}
//			cout<<endl;
		//	}
	//	}
	}

	bool HttpSql::mysql_delete(string _str)
	{
		return mysql_op(_str);
	}




