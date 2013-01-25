#include <windows.h>
#include <conio.h>
#include <iostream>
#include <fstream>

#define Max_size 10
//	Map grid
#define finish_x (start_x + 232)
#define start_x 212
#define start_y 18
#define step_y 24
//	Colors
#define Red RGB(255,0,0)
#define Green RGB(0,255,0)
//	Symbols for frame drawing 
#define VLine 186
#define HLine 205
#define LTTurn 201
#define LBTurn 200
#define RTTurn 187
#define RBTurn 188

using namespace std;

HWND hConWnd;
HDC DrawHDC;

class node
{
public:
	bool links[Max_size];
	bool status;
	void clear()
	{
		for(int i = 0; i < Max_size; i++)
			links[i] = false;
		status = false;
	}
	node()
	{
		clear();
	}
	node& operator =(const node& source)
	{
		if(this != &source)
		{
			status = source.status;
			for(int i = 0; i < Max_size; i++)
			{
				links[i] = source.links[i];
			}
		}
		return *this;
	}
};

class map
{
public:	
	node transactions[Max_size];
	node resources[Max_size];

	int num_transacrions;
	int num_resources;

	void create(int num_t, int num_r)
	{
		if(num_t <= 0)	num_t = 1;
		if(num_t > Max_size)	num_t = Max_size;
		if(num_r <= 0)	num_r = 1;
		if(num_r > Max_size)	num_r = Max_size;

		num_transacrions = num_t;
		num_resources = num_r;

		for(int i = 0; i < num_t; i++)
		{
			transactions[i].clear();
		}
		for(int i = 0; i < num_r; i++)
		{
			resources[i].clear();
		}
	}
	map(int num_t, int num_r){ create(num_t, num_r); }
	map() { create(1, 1); }
	map& operator =(const map& source)
	{
		if(this != &source)
		{
			num_transacrions = source.num_transacrions;
			num_resources = source.num_resources;
			for(int i = 0; i < Max_size; i++)
			{
				transactions[i] = source.transactions[i];
				resources[i] = source.resources[i];
			}
		}
		return *this;
	}
};



HWND GetConsoleWndHandle();
void drawPoint(int x, int y, COLORREF color); 
void drawLine(int x1, int x2, int y1, int y2, COLORREF color);
void drawBlank(map Map);
void drawMapLines(map Map);
bool findCycleStep(map& Map);
bool isDeadlock(map Map);
map handleInput();
map fileInpute();
map DeadLockExample();
map FreeExample();

int main()
{
	hConWnd = GetConsoleWndHandle();
	DrawHDC = GetDC(hConWnd);	
	map graph;
	
	cout << " 1) DeadLock example" << endl;
	cout << " 2) Free example" << endl;
	cout << " 3) Handle input" << endl;
	cout << " 4) File input" << endl << endl;
	char mode;
	do
	{
		cout << "    Choose mode: ";
		cin >> mode;
	}
	while(!strchr("1234", mode));
	try
	{
		switch(mode)
		{
		case '1':	graph = DeadLockExample();	break;
		case '2':	graph = FreeExample();		break;
		case '3':	graph = handleInput();		break;
		case '4':	graph = fileInpute();
		}
		do
		{
			drawBlank(graph);
			drawMapLines(graph);		
			cout << "\n                             Press SPACE for next step." << endl;		
			while(getch() != ' ') {}
		}
		while( findCycleStep(graph) );
		drawBlank(graph);
		drawMapLines(graph);	

		if(isDeadlock(graph))
			cout << "\n\a                                   DeadLock!";
		else
			cout << "\n\a                               DeadLock is not find!";
	}
	catch(...)
	{
		cout << "File is broken!" << endl;
	}
	getch();
	return 1;
}

map DeadLockExample()
{
	map Map(2,2);
	Map.transactions[0].status = true;
	Map.transactions[0].links[0] = true;
	Map.transactions[1].status = true;
	Map.transactions[1].links[1] = true;
	Map.resources[0].status = true;
	Map.resources[0].links[1] = true;
	Map.resources[1].status = true;
	Map.resources[1].links[0] = true;
	return Map;
}
map FreeExample()
{
	map Map(4,3);
	Map.transactions[0].status = true;
	Map.transactions[0].links[0] = true;
	Map.transactions[2].status = true;
	Map.transactions[2].links[2] = true;
	Map.resources[0].status = true;
	Map.resources[0].links[2] = true;
	Map.resources[0].links[3] = true;
	Map.resources[1].status = true;
	Map.resources[1].links[1] = true;
	Map.resources[2].status = true;
	Map.resources[2].links[3] = true;	
	return Map;
}
map handleInput()
{	
	system("cls");
	map Map;
	int num_t, num_r;
	do
	{
		cout << "Enter number of transaction (1-" << Max_size << "): ";
		cin >> num_t;
	}
	while(num_t < 1 || num_t > Max_size);
	do
	{
		cout << "Enter number of resources (1-" << Max_size << "): ";
		cin >> num_r;
	}
	while(num_r < 1 || num_r > Max_size);
	Map.create(num_t, num_r);

	bool free_resource[Max_size];
	for(int i = 0; i < Max_size; i++)
		free_resource[i] = true;
	for(int i = 0; i < num_t; i++)
	{
		cout << "\n    Transaction " << char('A'+i) << " use recoures?" << endl;
		char res = '0';
		do
		{
			cout << "Enter name of resoures (a-" << char('a' + num_r - 1) << ") or 'N' for inputing next transaction: ";
			cin >> res;
			if(res >= 'a' && res <= 'a' + num_r - 1)
			{
				if(free_resource[res-'a'])
				{
					free_resource[res-'a'] = false;
					Map.transactions[i].links[res-'a'] = true;
					Map.transactions[i].status = true;
					cout << "> OK!" << endl;
				}
				else
				{
					cout << "> Resource " <<  char(res) << " is use already." << endl;
				}
			}
			else
			{
				if(res != 'N' && res != 'n')
					cout << "> Unknow resource!" << endl;
			}
		}
		while(res != 'N' && res != 'n');
	}
	for(int i = 0; i < num_r; i++)
	{
		cout << "\n    Which transactions are waiting for resource " <<  char('a'+i) << "?" << endl;	
		char tran = '0';
		do
		{
			cout << "Enter name of transaction (A-" <<  char('A' + num_t - 1) << ") or 'N' for inputing next resource: ";
			cin >> tran;
			if(tran >= 'A' && tran <= 'A' + num_t - 1)
			{
				Map.resources[i].links[tran-'A'] = true;
				Map.resources[i].status = true;
				cout << "> OK!" << endl;
			}
			else
			{
				if(tran != 'N' && tran != 'n')
					cout << "> Unknow transaction!" << endl;
			}
		}
		while(tran != 'N' && tran != 'n');
	}
	return Map;
}
map fileInpute()
{
	system("cls");
	char filename[256];
	ifstream inFile;
	do
	{
		cout << "Enter filename: ";
		cin >> filename;
		inFile.open(filename, ios_base::in);
	}
	while(!inFile);

	map Map;
	int max_t(0);
	int max_r(0);
	bool free_resource[Max_size];
	for(int i = 0; i < Max_size; i++)
		free_resource[i] = true;

	char temp;
	inFile >> temp;
	while(!inFile.eof())
	{		
		if(temp >= 'A' && temp < 'A' + Max_size)
		{
			int tran = int(temp - 'A');
			if (max_t < tran)
				max_t = tran;
			inFile >> temp;
			if(temp >= 'a' && temp < 'a' + Max_size)
			{
				int res = int(temp - 'a');
				if (max_r < res)
					max_r = res;
				if(free_resource[res])
				{
					free_resource[res] = false;
					Map.transactions[tran].links[res] = true;
					Map.transactions[tran].status = true;
				}
				else
					throw 1;
			}
			else
				throw 1;
		}
		else
		{
			if(temp >= 'a' && temp < 'a' + Max_size)
			{
				int res = int(temp - 'a');
				if (max_r < res)
					max_r = res;
				inFile >> temp;
				if(temp >= 'A' && temp < 'A' + Max_size)
				{
					int tran = int(temp - 'A');
					if (max_t < tran)
						max_t = tran;
					Map.resources[res].links[tran] = true;
					Map.resources[res].status = true;
				}
				else
					throw 1;
			}
			else
			{
				if(temp != '\n' && temp != ' ' && temp != '\r' && temp != '\t')
				{				
					throw 1;
				}
			}
		}
		inFile >> temp;
	}
	Map.num_resources = max_r + 1;
	Map.num_transacrions = max_t + 1;
	return Map;
}
bool findCycleStep(map& Map)
{
	bool change = false;
	/*
	**	Step 1.
	**	Delete T->x arc where T have not input arc 
	*/
	for(int i = 0; i < Map.num_transacrions; i++)
	{
		if(Map.transactions[i].status)
		{
			bool free_link = true;
			for(int j = 0; j < Map.num_resources; j++)
			{
				if(Map.resources[j].links[i])
				{
					free_link = false;
				}
			}
			if(free_link)
			{
				for(int j = 0; j < Map.num_resources; j++)
				{
					Map.transactions[i].links[j] = false;
				}	
				Map.transactions[i].status = false;
				change = true;
			}
		}
	}
	/*
	**	Step 2.
	**	If resource have not input arc - rotate one arc.
	*/
	for(int i = 0; i < Map.num_resources; i++)
	{
		if(Map.resources[i].status)
		{
			bool input_link = false;
			for(int j = 0; j < Map.num_transacrions; j++)
			{
				if(Map.transactions[j].links[i])
				{
					input_link = true;
				}
			}
			if(!input_link)
			{
				int transaction = -1;
				for(int j = 0;  j <  Map.num_transacrions && transaction < 0; j++)
				{
					if(Map.resources[i].links[j])
					{
						transaction = j;
					}
				}
				Map.transactions[transaction].links[i] = true;
				Map.transactions[transaction].status = true;
				Map.resources[i].links[transaction] = false;
				Map.resources[i].status = false;
				for(int j = transaction + 1; j <  Map.num_transacrions; j++)
				{
					if(Map.resources[i].links[j])
						Map.resources[i].status = true;
				}
				change = true;
			}
		}
	}
	return change;
}
bool isDeadlock(map Map)
{
	for(int i = 0; i < Map.num_transacrions; i++)
	{
		if(Map.transactions[i].status)
			return true;
	}
	return false;
}
void drawMapLines(map Map)
{
	for(int i = 0; i < Map.num_transacrions; i++)
	{
		if(Map.transactions[i].status)
		{
			for(int j = 0; j < Map.num_resources; j++)
			{
				if(Map.transactions[i].links[j])
				{
					drawLine(start_x, start_y + i*step_y, finish_x, + start_y + j*step_y, Green);
				}
			}
		}
	}
	for(int i = 0; i < Map.num_resources; i++)
	{
		if(Map.resources[i].status)
		{
			for(int j = 0; j < Map.num_transacrions; j++)
			{
				if(Map.resources[i].links[j])
				{
					drawLine(finish_x, start_y + i*step_y, start_x, + start_y + j*step_y, Red);
				}
			}
		}
	}
}
void drawBlank(map Map)
{
	system("cls");
	char format[] = "\n                        %c%c                              %c%c";
	printf("			%c", LTTurn);
	for(int i = 0; i < 32; i++)
	{
		printf("%c", HLine);
	}
	printf("%c", RTTurn);
	printf(format, VLine,'A', 'a', VLine);
	for(char i = 1; i < max(Map.num_resources, Map.num_transacrions); i++)
	{	
		printf(format, VLine,' ', ' ', VLine);
		char transaction = (Map.num_transacrions > i)?(i+'A'):(' ');
		char resource = (Map.num_resources > i)?(i+'a'):(' ');
		printf(format, VLine, transaction, resource, VLine);
	}	
	printf("\n			%c", LBTurn);
	for(int i = 0; i < 32; i++)
	{
		printf("%c", HLine);
	}
	printf("%c", RBTurn);
	printf("\n                         Transactions           Resources\n");
}
void drawLine(int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hNPen = CreatePen(PS_SOLID, 2, color);
	HPEN hOPen = (HPEN)SelectObject(DrawHDC, hNPen);
	MoveToEx(DrawHDC, x1, y1, NULL);	
	LineTo(DrawHDC, x2, y2);
}
void drawPoint(int x, int y, COLORREF color)
{
	for(int i = -1; i < 2; i++)
	{
		SetPixel(DrawHDC, x+i, y-2, color);		
		SetPixel(DrawHDC, x+i, y+2, color);
	}
	for(int i = -2; i < 3; i++)
	{
		SetPixel(DrawHDC, x+i, y-1, color);		
		SetPixel(DrawHDC, x+i, y, color);	
		SetPixel(DrawHDC, x+i, y+1, color);
	}
}



HWND GetConsoleWndHandle()
{
	HWND hConWnd;
	OSVERSIONINFO os;
	char szTempTitle[64], szClassName[128], szOriginalTitle[1024];
	 
	os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &os );
	// may not work on WIN9x
	if ( os.dwPlatformId == VER_PLATFORM_WIN32s ) return 0;
	 
	GetConsoleTitle( szOriginalTitle, sizeof( szOriginalTitle ) );
	sprintf( szTempTitle,"%u - %u", GetTickCount(), GetCurrentProcessId() );
	SetConsoleTitle( szTempTitle );
	Sleep( 40 );
	// handle for NT
	hConWnd = FindWindow( NULL, szTempTitle );
	SetConsoleTitle( szOriginalTitle );
	 
	// may not work on WIN9x
	if ( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		hConWnd = GetWindow( hConWnd, GW_CHILD );
		if ( hConWnd == NULL ) return 0;
		GetClassName( hConWnd, szClassName, sizeof ( szClassName ) );
		while ( strcmp( szClassName, "ttyGrab" ) != 0 )
		{
			hConWnd = GetNextWindow( hConWnd, GW_HWNDNEXT );
			if ( hConWnd == NULL ) return 0;
			GetClassName( hConWnd, szClassName, sizeof( szClassName ) );
		}
	}
	return hConWnd;
}