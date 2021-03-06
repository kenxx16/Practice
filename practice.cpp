// practice.cpp: определяет точку входа для консольного приложения.
//



#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <codecvt>
#include <stdlib.h>
#include <cstdio>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <odbcinst.h>


using namespace std;




bool FileIsExist(std::string filePath)
{
	bool isExist = false;
	std::ifstream fin(filePath.c_str());

	if (fin.is_open())
		isExist = true;

	fin.close();
	return isExist;
}

struct TALM_GLN {
#pragma pack(1)
	__int16 NOM_KA;  /*НОМЕР КА*/
	__int16 CHAN;    /*ЧАСТОТНЫЙ КАНАЛ;*/
	__int16 HEALTH;  /*ПРИЗНАК ЗДОРОВЬЯ;*/
	float E;          /*!ЭКСЦЕНТРИСИТЕТ*/
	__int32 N_DAY;   /*НОМЕР ДНЯ 4-Х ЛЕТНЕГО ПЕРИОДА*/
	float I_CORR;    /*ПОПРАВКА К НАКЛОНЕНИЮ (РАД)*/
	float LAMBDA;    /*ДОЛГОТА 1-ГО В СУТКАХ ВОСХОДЯЩЕГО УЗЛА
					 (РАД)*/
	float T_LAMBDA;  /*ВРЕМЯ (СЕК) ОТ НАЧАЛА СУТОК 1-ГО ВОСХ.
					 УЗЛА (РАД)*/
	float OMEGA;    /*АРГУМЕНТ ПЕРИГЕЯ (РАД) */
	float DT_DRAC;   /*ПОПРАВКА К ПЕРИОДУ 43200 (СЕК)*/
	float AF1;      /*СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК)*/
	float AF0;      /*УХОД ВРЕМЕНИ БОРТА (СЕК)*/
	__int16 CHECK_;  /*КОНТРОЛЬНАЯ СУММА*/
	double MJD;     /* MJD (ТАЙМЕР ПК)*/
};

struct TALM_GPS {
#pragma pack(1)
	__int16 NOM_KA;   //НОМЕР КА
	__int16 HEALTH;   //ПРИЗНАК ЗДОРОВЬЯ
	float E;          //ЭКСЦЕНТРИСИТЕТ
	__int32 TOA;      //ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА (СЕК)
	float I_ANGLE;    //НАКЛОНЕНИЕ (РАД)
	float OMEGADOT;   //СКОРОСТЬ ПРЯМОГО ВОСХОЖДЕНИЯ (РАД/СЕК)
	double ROOTA;     //SQRT(A), КОРЕНЬ КВ.(МЕТР)
	double OMEGA0;    //ДОЛГОТА ВОСХОДЯЩЕГО УЗЛА (РАД)
	double OMEGA;     //АРГУМЕНТ ПЕРИГЕЯ (РАД)
	double M0;        //СРЕДНЯЯ АНОМАЛИЯ НА ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА(РАД)
	float AF0;        //УХОД ВРЕМЕНИ БОРТА (СЕК)
	float AF1;        //СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК)
	__int16 WNA;      //НОМЕР НЕДЕЛИ АЛЬМАНАХА
	__int16 WN;       //НОМЕР НЕДЕЛИ
	__int32 TOW;      //ВРЕМЯ В СЕКУНДАХ ОТ НАЧАЛА GPS-НЕДЕЛИ
	__int16 CHECK_;   //КОНТРОЛЬНАЯ СУММА
	double MJD;       //MJD (ТАЙМЕР ПК)
};






int main(int argc, char* argv[])		
{	
	setlocale(LC_ALL,"1251");
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	SQLWCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;

	HWND desktopHandle = GetDesktopWindow();   // desktop's window handle  

											   // Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				
				retcode = SQLDriverConnect( // SQL_NULL_HDBC  
					hdbc,
					desktopHandle,
					(SQLWCHAR*)"driver=SQL server",
					_countof("driver=SQL server"),
					OutConnStr,
					255,
					&OutConnStrLen,
					SQL_DRIVER_PROMPT);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { cout << "ok0" << endl; }
				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					HSTMT hStmt = SQL_NULL_HSTMT;
					
					retcode = SQLAllocStmt(hdbc, &hStmt);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { cout << "ok1" << endl; }

					retcode = SQLExecDirect(&hStmt,(SQLWCHAR*)"CREATE TABLE A2 (Q INT Primary key, W INT, E INT)", SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { cout << "ok2" << endl; }
					if (retcode == SQL_ERROR) { cout << "err" << endl; }
					// cout << retcode << endl;
					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					cout << "DataBase connected" << endl;
					cout << "------------------"<< endl;
					}
					
					SQLDisconnect(hdbc);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	
		
	TALM_GPS y;
	TALM_GLN x;
	for (int al = 1; al < (argc); al++) {
		fstream tx;
		FILE *in;
		
		if (FileIsExist(argv[al]))
			cout << "file available" << endl;
		else
		{
			cout << "file not found" << endl;
			cout << "              " << endl;
			continue;
		}
		fopen_s(&in, argv[al], "rb");

		char n[28];
		strcpy_s(n, 28, argv[al]);
		strcat_s(n, 28, ".txt");
		
		if (FileIsExist(n))
			 cout << "txt file is overwritten" << endl;
		else cout << "txt file not found, new created" << endl;
		tx.open(n, ios::out);
		cout << n << endl;

		int ex;
		int i;

		cout << strrchr(n, '.s') << endl;
		cout << "          " << endl;

		string s = strrchr(n, '.s');
		if (s == "sal.alm.txt")
		{									
			ex = 0;
			i = 0;




			while (ex != 1)
			{
				fseek(in, i * 78, SEEK_SET);               //GPS  
				if (!fread(&y, 78, 1, in))
				{
					ex = 1;
				}
				else
				{
					tx << "НОМЕР КА ========================================== " << y.NOM_KA << endl <<
						"ПРИЗНАК ЗДОРОВЬЯ ================================== " << y.HEALTH << endl <<
						"ЭКСЦЕНТРИСИТЕТ ==================================== " << y.E << endl <<
						"ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА (СЕК) ===================== " << y.TOA << endl <<
						"НАКЛОНЕНИЕ (РАД) ================================== " << y.I_ANGLE << endl <<
						"СКОРОСТЬ ПРЯМОГО ВОСХОЖДЕНИЯ (РАД/СЕК) ============ " << y.OMEGADOT << endl <<
						"SQRT(A), КОРЕНЬ КВ.(МЕТР) ========================= " << y.ROOTA << endl <<
						"ДОЛГОТА ВОСХОДЯЩЕГО УЗЛА (РАД) ==================== " << y.OMEGA0 << endl <<
						"АРГУМЕНТ ПЕРИГЕЯ (РАД) ============================ " << y.OMEGA << endl <<
						"СРЕДНЯЯ АНОМАЛИЯ НА ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА(РАД) == " << y.M0 << endl <<
						"УХОД ВРЕМЕНИ БОРТА (СЕК) ========================== " << y.AF0 << endl <<
						"СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК) ============ " << y.AF1 << endl <<
						"НОМЕР НЕДЕЛИ АЛЬМАНАХА ============================ " << y.WNA << endl <<
						"НОМЕР НЕДЕЛИ ====================================== " << y.WN << endl <<
						"ВРЕМЯ В СЕКУНДАХ ОТ НАЧАЛА GPS-НЕДЕЛИ ============= " << y.TOW << endl <<
						"КОНТРОЛЬНАЯ СУММА ================================= " << y.CHECK_ << endl <<
						"MJD (ТАЙМЕР ПК) =================================== " << y.MJD << endl <<
						" " << endl;
					i++;
				}
			}
		}
		else
		{
			i = 0;
			ex = 0;

			while (ex != 1)
			{
				fseek(in, i * 52, SEEK_SET);                  //GLN  
				if (!fread(&x, 52, 1, in))
				{
					ex = 1;
				}
				else
				{
					tx << "НОМЕР КА ========================================== " << x.NOM_KA << endl <<
						"ЧАСТОТНЫЙ КАНАЛ =================================== " << x.CHAN << endl <<
						"ПРИЗНАК ЗДОРОВЬЯ ================================== " << x.HEALTH << endl <<
						"ЭКСЦЕНТРИСИТЕТ ==================================== " << x.E << endl <<
						"НОМЕР ДНЯ 4-Х ЛЕТНЕГО ПЕРИОДА ===================== " << x.N_DAY << endl <<
						"ПОПРАВКА К НАКЛОНЕНИЮ (РАД) ======================= " << x.I_CORR << endl <<
						"ДОЛГОТА 1-ГО В СУТКАХ ВОСХОДЯЩЕГО УЗЛА (РАД) ====== " << x.LAMBDA << endl <<
						"ВРЕМЯ (СЕК) ОТ НАЧАЛА СУТОК 1-ГО ВОСХ. УЗЛА(РАД) == " << x.T_LAMBDA << endl <<
						"АРГУМЕНТ ПЕРИГЕЯ (РАД) ============================ " << x.OMEGA << endl <<
						"ПОПРАВКА К ПЕРИОДУ 43200 (СЕК) ==================== " << x.DT_DRAC << endl <<
						"СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК) ============ " << x.AF1 << endl <<
						"УХОД ВРЕМЕНИ БОРТА ================================ " << x.AF0 << endl <<
						"КОНТРОЛЬНАЯ СУММА ================================= " << x.CHECK_ << endl <<
						"MJD (ТАЙМЕР ПК) =================================== " << x.MJD << endl <<
						" " << endl;
					i++;
				}
			}
		}

		fclose(in);
		tx.close();

	}

	system("pause");

	return 0;
}
