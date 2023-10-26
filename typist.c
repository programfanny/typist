//typist
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ID_TIMER 		1
#define ID_MINUTE       2
#define bdwidth 		60
#define bdheight 		60

typedef struct bdkey{
	int left, top, xpos, ypos, row, col, finger;
	char name[10];
}BDKEY;

MCI_OPEN_PARMS m_mciOpen;  //打开参数  
MCI_PLAY_PARMS m_mciPlay;  //播放参数  

BOOL PlayMP3(char * szFileName)   
{   
    m_mciOpen.lpstrDeviceType = "mpegvideo"; //要操作的文件类型  
    m_mciOpen.lpstrElementName = szFileName; //要操作的文件路径  
    MCIERROR mcierror = mciSendCommand( 0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT ,(DWORD)&m_mciOpen); //打开文件命令  
    if(mcierror)
    {  
        return FALSE;  
    }
    else
    {  
        m_mciPlay.dwCallback = (DWORD)NULL;  
        m_mciPlay.dwFrom = (DWORD)0; //播放起始位置ms为单位  
        mciSendCommand(m_mciOpen.wDeviceID, MCI_PLAY, MCI_NOTIFY|MCI_FROM, (DWORD)(LPVOID)&m_mciPlay);
    }  
    return TRUE;  
}  
void CloseMP3()
{
    mciSendCommand(m_mciOpen.wDeviceID, MCI_CLOSE, 0, 0);
}
VOID InitKeyBoard(BDKEY *bdkeys,int width, int height){
	char *stroke="QWERTYUIOPASDFGHJKLZXCVBNM";
	int keynumber[]={10,9,7};
	for(int i=0;i<26;i++){
		bdkeys[i].name[0] = stroke[i];
		bdkeys[i].name[1] = 0;
	}	
	int i=0;
	for(int j=0;j<3;j++){
		for(int k=0;k<keynumber[j];k++){
			bdkeys[i].xpos = (k+j*0.5)*width;
			bdkeys[i].ypos = j*height;
			bdkeys[i].row=j;
			bdkeys[i].col=k;
			bdkeys[i].finger=k;
			if(k==4)bdkeys[i].finger=3;
			else if(k==5)bdkeys[i].finger=4;
			else if(k>5)bdkeys[i].finger=k-2;
			i++;
		}
	}
}
VOID InitHand(POINT ***hand,int startx,int starty){
	int rows=10,cols=4;
	*hand=(POINT**)malloc(rows*sizeof(POINT*));
	for(int i=0;i<rows;i++){
		(*hand)[i]=(POINT*)malloc(cols*sizeof(POINT));
		(*hand)[i][0].x=   (i+i/4*2)*bdwidth+startx+30;(*hand)[i][0].y=starty+350;
		(*hand)[i][3].x=45+(i+i/4*2)*bdwidth+startx+30;(*hand)[i][3].y=starty+350;
		(*hand)[i][1].x=   (i+i/4*2)*bdwidth+startx+30;(*hand)[i][1].y=starty+150;
		(*hand)[i][2].x=40+(i+i/4*2)*bdwidth+startx+30;(*hand)[i][2].y=starty+150;
	}
	(*hand)[8][0].x= (*hand)[3][3].x;      (*hand)[8][0].y=(*hand)[3][3].y+10;
	(*hand)[8][3].x= (*hand)[3][3].x+30;   (*hand)[8][3].y=(*hand)[3][3].y+30;
	(*hand)[8][1].x= (*hand)[3][3].x+40;   (*hand)[8][1].y=(*hand)[3][3].y-120;
	(*hand)[8][2].x= (*hand)[3][3].x+90;   (*hand)[8][2].y=(*hand)[3][3].y-90;
	(*hand)[9][0].x= (*hand)[4][0].x;      (*hand)[9][0].y=(*hand)[4][0].y+10;
	(*hand)[9][3].x= (*hand)[4][0].x-30;   (*hand)[9][3].y=(*hand)[4][0].y+30;
	(*hand)[9][1].x= (*hand)[4][0].x-40;   (*hand)[9][1].y=(*hand)[4][0].y-120;
	(*hand)[9][2].x= (*hand)[4][0].x-90;   (*hand)[9][2].y=(*hand)[4][0].y-90;
}
VOID ShowHand(HDC hdc,BDKEY bdkey,POINT **hand){
	char buf[300];
	int rows=10,cols=4;
	for(int i=0;i<rows;i++){
		PolyBezier(hdc,hand[i],4);
	}
	int x=(hand[bdkey.finger][0].x+hand[bdkey.finger][1].x+hand[bdkey.finger][2].x+hand[bdkey.finger][3].x)/4;
	int y=(hand[bdkey.finger][0].y+hand[bdkey.finger][1].y+hand[bdkey.finger][2].y+hand[bdkey.finger][3].y)/4;
	HBRUSH purplebrush = CreateSolidBrush(RGB(0,192,192));
	SelectObject(hdc, purplebrush);
	Ellipse(hdc,x-20,y-20,x+20,y+20);
}
VOID ShowKeyBoard(HWND hwnd,BDKEY *bdkeys, int startx,int starty,int width,int height){
	HDC hdc=GetDC(hwnd);
	HPEN redpen,oldpen;
	redpen=CreatePen(PS_SOLID,5,RGB(255,0,0));
	char buf[100];
	SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));
	for(int i=0;i<26;i++){
		Rectangle(hdc,startx+bdkeys[i].xpos,starty+bdkeys[i].ypos,startx+bdkeys[i].xpos+width-4,starty+bdkeys[i].ypos+height-4);
		TextOut(hdc,startx+bdkeys[i].xpos+10,starty+bdkeys[i].ypos+10,bdkeys[i].name,1);		
		if(bdkeys[i].col==4 ){
			oldpen = SelectObject(hdc,redpen);
			MoveToEx(hdc,startx+bdkeys[i].xpos+width-2,starty+bdkeys[i].ypos-2,NULL);
			LineTo(hdc,startx+bdkeys[i].xpos+width-2,starty+bdkeys[i].ypos+height-2);
			if(bdkeys[i].row<2)
			LineTo(hdc,startx+bdkeys[i].xpos+width*3/2-2,starty+bdkeys[i].ypos+height-2);
			SelectObject(hdc, oldpen);
		}
	}
	DeleteObject(redpen);
	ReleaseDC(hwnd,hdc);	
}
VOID ReadKey(BDKEY bdkey){
	char *path=malloc(MAX_PATH);
	GetCurrentDirectory(256,path);
	char filename[MAX_PATH];	
	strcpy(filename,path);
	strcat(filename,"\\letterSound\\");
	strcat(filename,bdkey.name);
	strcat(filename,".mp3");
	PlayMP3(filename);		
}
VOID ShowColorKey(HWND hwnd, BDKEY bdkey, int startx,int starty,int width,int height){
	HDC hdc=GetDC(hwnd);
	HPEN redpen=CreatePen(PS_SOLID,5,RGB(255,0,0));
	SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));
	SelectObject(hdc,redpen);
	Rectangle(hdc,startx+bdkey.xpos,starty+bdkey.ypos,startx+bdkey.xpos+width-4,starty+bdkey.ypos+height-4);
	ReleaseDC(hwnd,hdc);
	DeleteObject(redpen);
	ReadKey(bdkey);
	SetTimer(hwnd,ID_TIMER,750,NULL);
}
VOID ShowMark(HWND hwnd, int right, int wrong){
	char buf[100];
	RECT rect;
	HDC hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	HFONT hFont=CreateFont(-50,-20,0,0,0, 0,0,0,0,0, 0,0,0,0);
	SelectObject(hdc, hFont);
	SetTextColor(hdc,RGB(255,0,255));
	Rectangle(hdc, rect.left,rect.top,rect.right, rect.bottom);
	sprintf(buf,"You pressed %d keys in two minutes.",right+wrong);
	TextOut(hdc,50,70,buf,strlen(buf));
	sprintf(buf,"Right Key: %d  Wrong Key: %d",right,wrong);
	TextOut(hdc,50,130,buf,strlen(buf));
	sprintf(buf," Mark = %5.2f",right*100.0/(right+wrong));
	TextOut(hdc,90,190,buf,strlen(buf));
	ReleaseDC(hwnd,hdc);
	DeleteObject(hFont);
}
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("HelloWin") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR) ;
		return 0 ;
	}
	hwnd = CreateWindow (szAppName,                 	// window class name
						TEXT ("The Hello Program"), 	// window caption
						WS_OVERLAPPEDWINDOW,        	// window style
						CW_USEDEFAULT,              	// initial x position
						CW_USEDEFAULT,              	// initial y position
						CW_USEDEFAULT,              	// initial x size
						CW_USEDEFAULT,              	// initial y size
						NULL,                       	// parent window handle
						NULL,                       	// window menu handle
						hInstance,                  	// program instance handle
						NULL) ;                     	// creation parameters
	 
	 ShowWindow (hwnd, iCmdShow) ;
	 UpdateWindow (hwnd) ;
	 
	 while (GetMessage (&msg, NULL, 0, 0))
	 {
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	 }
	 return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	char buf[100];
	static RECT rect;
	static BDKEY *bdkeys, bdkey;
	static int number;
	static POINT **hand;
	static int startx, starty;
	static int right, wrong,tick;
	static HBRUSH hOldBrush,redbrush,bluebrush;

	switch (message)
	{
	case WM_CREATE:
		//PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC) ;
		redbrush = CreateSolidBrush(RGB(192,0,0));
		bluebrush = CreateSolidBrush(RGB(0,192,192));
		srand((unsigned)time(NULL));
		bdkeys = (BDKEY*)malloc(26*sizeof(BDKEY));
		InitKeyBoard(bdkeys,bdwidth,bdheight);
		startx=200; starty=130;
		InitHand(&hand,startx,starty);
		right=0;wrong=0;tick=0;
		number=rand()%26;
		SetTimer(hwnd,ID_MINUTE,100,NULL);
		return 0 ;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		if(right+wrong>0){
			hOldBrush = SelectObject(hdc,redbrush);
			Rectangle(hdc,startx,starty-150,startx+600.0*right/(right+wrong),starty-80);
			SelectObject(hdc,bluebrush);
			Rectangle(hdc,startx+600.0*right/(right+wrong),starty-150,startx+600,starty-80);
			SelectObject(hdc, hOldBrush);
		}
		ShowKeyBoard(hwnd, bdkeys, startx,starty,bdwidth,bdheight);
		ShowColorKey(hwnd, bdkeys[number], startx,starty,bdwidth,bdheight);
		ShowHand(hdc,bdkeys[number],hand);
		
		EndPaint (hwnd, &ps) ;
		return 0 ;
	case WM_TIMER:
		switch(wParam){
			case ID_TIMER:
				CloseMP3();
				KillTimer(hwnd,ID_TIMER);
				break;
			case ID_MINUTE:
				tick++;
				if(tick==1200){
					KillTimer(hwnd, ID_MINUTE);					
					ShowMark(hwnd,right,wrong);
					Beep(600,1000);
				}
		}
		return 0;	
	case WM_KEYDOWN:
		if(64<wParam && wParam<91){
			if(bdkeys[number].name[0]!=wParam){
				Beep(500,150);
				wrong++;
			}else{
				right++;
			}
			if(wrong+right==100){
				ShowMark(hwnd,right,wrong);
			}
			number=rand()%26;
		}else{
			switch(wParam){
				case VK_ESCAPE:
					PostMessage( hwnd,WM_DESTROY,0,0 );
					break;
				case VK_F2:

					break;
				case VK_F3:

					break;	
				case VK_RETURN:
					right=0;wrong=0;tick=0;
					number = rand()%26;
					SetTimer(hwnd, ID_MINUTE,100,NULL);
					break;
			}
		}
		InvalidateRect(hwnd,&rect,TRUE);
		return 0;	
		  
	case WM_DESTROY:
		DeleteObject(redbrush);
		DeleteObject(bluebrush);
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


/*

L:\\disk02\\03_Study\\LetterSound\\
BOOL InvalidateRect(
  [in] HWND       hWnd,
  [in] const RECT *lpRect,
  [in] BOOL       bErase
);

*/

