// CppReadFile.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <vector>
#include <algorithm>
#using <mscorlib.dll>
#using <System.Data.dll>
#using <System.dll>
//步骤1：添加对ADO的支持 
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
using namespace System;
using namespace System::Data::OleDb;
using namespace System::Runtime::InteropServices;


using namespace std;

//去除空格
string& ClearAllSpace(string &str)
{
	int index = 0;
	if (!str.empty())
	{
		while ((index = str.find(' ', index)) != string::npos)
		{
			str.erase(index, 1);
		}
	}
	return str;
}

//去除:colon
string& ClearAllcolon(string &str)
{
	int index = 0;
	if (!str.empty())
	{
		while ((index = str.find(':', index)) != string::npos)
		{
			str.erase(index, 1);
		}
	}
	return str;
}

//用sstream流转换数据类型
void str2double(double &double_temp, const string &string_temp)
{
	stringstream stream(string_temp);
	stream >> double_temp;
}

//System::String 转std::string
string Str2str(System::String^ Str) {
	string str = "";
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(Str)).ToPointer();
	str = chars;
	return str;
}


//用来确认是否是同一人的结构
struct SamePerson
{
	string name;
	string sex;
	string birthday;
	bool operator==(const SamePerson& another) // 操作运算符重载，判断两个结构内容相等
	{
		return(name == another.name) && (sex == another.sex) && (birthday == another.birthday);
	}
};


//用来储存用户资料的结构
struct Persondata
{
	char name[10];
	char sex[6];
	int phonenumber;
	char birthday[12];
	float height;
	float weight;
	string date;
	string time;
	double stepping_1;
	double stepping_2;
	double stepping_3;
	double stepping_4;
	double stepping_5;
	double stepping_6;
	double stepping_7;
	double stepping_8;
};
int main(int argc, _TCHAR* argv[])
{
	//获取系统时间
#pragma region 获取系统时间
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
		ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
		ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
	string currentTime
		= to_string(ptminfo->tm_year + 1900) + "-"
		+ to_string(ptminfo->tm_mon + 1) + "-"
		+ to_string(ptminfo->tm_mday);
	cout << currentTime << endl;
#pragma endregion

	currentTime = "2018/7/25";//test!
	vector<double> v_time(0);//存放是今天的资料的时间
	SamePerson person;//存放要读取的人的资料
	//建立数据库连接
	OleDbConnection^ conn = nullptr;
	OleDbCommand^ cmd = nullptr;
	//选择数据库中的表格
	String^ sqlstr = "SELECT * FROM [biaoge]";
	try
	{
#pragma region reader
		//**此处修改文件路径Data Source
		conn = gcnew OleDbConnection("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\Users\\yj\\Desktop\\CppReadfile\\ku.mdb");
		//**
		conn->Open();
		cmd = gcnew OleDbCommand(sqlstr, conn);

		OleDbDataReader^ reader = cmd->ExecuteReader(System::Data::CommandBehavior::CloseConnection);

		String^ Sep = gcnew String('*', 60);

		//找出今日的资料并按时间先后排序
		int row = 0;
		while (reader->Read())
		{
			//获取表格中的日期
			string date = "";
			String^ Str_date = reader->GetValue(6)->ToString();
			date = Str2str(Str_date);

			if (ClearAllSpace(date) == currentTime)
			{
				cout << date << "," << currentTime << endl;
				cout << "today" << "row number is:" << row << endl;
				//读取时间	
				string time = "";
				String^ Str_time = reader->GetValue(7)->ToString();
				time = Str2str(Str_time);
				time = ClearAllcolon(time);
				double d_time;
				str2double(d_time, time);
				v_time.push_back(d_time);
			}
			else {
				cout << date << "," << currentTime << endl;
				cout << "not today!" << endl;

			}
			Console::WriteLine(Sep);
			++row;


		}
		//排序找出最新的时间
		sort(v_time.begin(), v_time.end());
#pragma endregion

		reader->Close();
		conn->Close();
#pragma region reader_2
		//第二次，读取最新一笔资料，reader_2,读取只能顺序读取，，，
		

		conn->Open();
		OleDbDataReader^ reader_2 = cmd->ExecuteReader(System::Data::CommandBehavior::CloseConnection);

		int newst_row = 0;
		while (reader_2->Read())
		{
			string time = "";
			String^ Str_time = reader_2->GetValue(7)->ToString();

			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(Str_time)).ToPointer();
			time = chars;
			time = ClearAllcolon(time);
			double d_time;
			str2double(d_time, time);
			if (d_time == v_time.back())
			{
				cout << "最新的数据在第几row:" << newst_row << endl;
				String^ Str_name  =  reader_2->GetValue(0)->ToString();
				String^ Str_sex   =  reader_2->GetValue(1)->ToString();
				String^ Str_birth =  reader_2->GetValue(3)->ToString();
				person = { Str2str(Str_name),Str2str(Str_sex),Str2str(Str_birth) };
				cout << "name: " << person.name << "sex: " << person.sex << "  "<< "birth: " << person.birthday << endl;

			}
			else
			{
				cout << d_time << "," << v_time.back() << endl;
			}
			newst_row++;
		}
		
#pragma endregion
		reader_2->Close();
		conn->Close();
#pragma region reader_3
		//第三次，读取同名资料，reader_3
		conn->Open();
		OleDbDataReader^ reader_3 = cmd->ExecuteReader(System::Data::CommandBehavior::CloseConnection);
		int n = 0;//记录有同一人有几个记录
		while (reader_3->Read()) {
			String^ Str_name = reader_3->GetValue(0)->ToString();
			String^ Str_sex = reader_3->GetValue(1)->ToString();
			String^ Str_birth = reader_3->GetValue(3)->ToString();
			SamePerson p = { Str2str(Str_name),Str2str(Str_sex),Str2str(Str_birth) };
			if (p == person)
			{
				n++;
				cout << p.name << endl;

			}
			
		}
		cout << "record :" << n << endl;
		if (n == 1)
		{
			cout << "new" << endl;
			//在此处输出数据
		}
		else
		{
			cout << "old" << endl;
			//在此处输出数据
		}
#pragma endregion

		
	}
	catch (Exception^ ex)
	{
		Console::WriteLine(ex->ToString());
	}
	Console::ReadLine();

	
	conn->Close();

	//update
	//Data::DataSet^ dataset = gcnew Data::DataSet();
	//conn = gcnew OleDbConnection("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\Users\\yj\\Desktop\\CppReadfile\\ku.mdb");
	//conn->Open();
	//cmd = gcnew OleDbCommand(sqlstr, conn);

	//OleDbDataAdapter^ adapter(conn);
    return 0;
}

