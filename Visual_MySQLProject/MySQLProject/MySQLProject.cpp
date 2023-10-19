#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

//for demonstration only. never save your password in the code!
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "ehggur0347!";

int main()
{
	//---------- DB 서버 연결 ----------
	sql::Driver* MySQLDriver;
	sql::Connection* Connection;
	sql::Statement* State;
	sql::PreparedStatement* PreState;
	sql::ResultSet* Result;

	try
	{
		MySQLDriver = get_driver_instance();
		Connection = MySQLDriver->connect(server, username, password); // 서버 연결
		cout << "DB Server Connect Success\n";
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
	// --------------------


	// 미리 생성해 둔 스키마 연결
	Connection->setSchema("dbserver_tcp");


	//---------- 테이블 생성 ----------
	// 해당 작업 상태 선택
	State = Connection->createStatement();

	// 해당 테이블이 존재한다면 삭제해라
	State->execute("DROP TABLE IF EXISTS UserInfo");
	cout << "Finished dropping table (if existed)" << endl;

	// 테이블 및 컬럼 생성
	State->execute("CREATE TABLE UserInfo (SerialNum serial PRIMARY KEY, UserName VARCHAR(1024) not null, UserPassword VARCHAR(1024) not null)");
	cout << "Finished creating table" << endl;
	delete State;
	// --------------------


	//---------- 테이블에 정보 넣기 ----------
	PreState = Connection->prepareStatement("INSERT INTO UserInfo(UserName, UserPassword) VALUES(?,?)");

	//string testtext;
	//char testtext[1024];
	//cin >> testtext;

	PreState->setString(1, "동혁");
	PreState->setString(2, "abcd123!");
	PreState->execute();
	cout << "One row inserted." << endl;

	PreState->setString(1, "윤제");
	PreState->setString(2, "efgh123!");
	PreState->execute();
	cout << "One row inserted." << endl;

	PreState->setString(1, "찬영");
	PreState->setString(2, "abcd456!");
	PreState->execute();
	cout << "One row inserted." << endl;
	// --------------------
	

	//---------- 테이블 정보 읽어 오기 ----------
	PreState = Connection->prepareStatement("SELECT * FROM UserInfo");
	Result = PreState->executeQuery();

	while (Result->next())
	{
		printf("SerialNum : %d, UserName : %s, UserPassword : %s)\n", Result->getInt(1), Result->getString(2).c_str(), Result->getString(3).c_str());
	}
	cout << "----------\n";
	// --------------------


	//---------- 테이블 업데이트 ----------
	PreState = Connection->prepareStatement("UPDATE UserInfo SET UserPassword = ? WHERE UserName = ?");
	PreState->setString(1, "123abcd!");
	PreState->setString(2, "ChanYoung");
	PreState->executeQuery();
	printf("Row updated\n");
	cout << "----------\n";

	// 갱신한 정보 다시 가져오기
	PreState = Connection->prepareStatement("SELECT * FROM UserInfo");
	Result = PreState->executeQuery();

	while (Result->next())
	{
		printf("SerialNum : %d, UserName : %s, UserPassword : %s)\n", Result->getInt(1), Result->getString(2).c_str(), Result->getString(3).c_str());
	}
	cout << "----------\n";
	// --------------------


	//---------- 테이블 삭제 ----------
	//PreState = Connection->prepareStatement("DELETE FROM UserInfo WHERE UserName = ?");
	//PreState->setString(1, "ChanYoung");
	//Result = PreState->executeQuery();
	//printf("Row deleted\n");
	//cout << "----------\n";

	//// 수정한 정보 다시 가져오기
	//PreState = Connection->prepareStatement("SELECT * FROM UserInfo");
	//Result = PreState->executeQuery();

	//while (Result->next())
	//{
	//	printf("SerialNum : %d, UserName : %s, UserPassword : %s)\n", Result->getInt(1), Result->getString(2).c_str(), Result->getString(3).c_str());
	//}
	//cout << "----------\n";
	// --------------------


	//---------- 두번째 테이블 만들기 ----------
	State = Connection->createStatement();

	State->execute("DROP TABLE IF EXISTS ChatLog");
	cout << "Finished dropping table (if existed)" << endl;

	// 테이블 및 컬럼 생성
	State->execute("CREATE TABLE ChatLog (MessageNum serial PRIMARY KEY, SenderID INT not null, SenderName VARCHAR(1024) not null, Text VARCHAR(50), Time DATETIME)");
	cout << "Finished creating table" << endl;
	delete State;
	// --------------------
	
	delete Result;
	delete PreState;
	delete Connection;
	system("pause");
	return 0;
}