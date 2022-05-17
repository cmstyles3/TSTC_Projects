//Collin Munson
//ITSE 1343-1031
//June 18, 2007

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <mmsystem.h>
#include <stdlib.h>

#include "resources.h"

#define WINDOW_CLASS_NAME "LAB04_CMM"

#define WINDOW_WIDTH  550
#define WINDOW_HEIGHT 300

#define DICE_SIZE 60

#define MAX_DICE 6

enum GAMESTATE{START, PLAY, GAMEOVER};
enum DIESTATE{DEFAULT, ROLLABLE, SELECTED, LOCKED};
enum TURN{PLAYER1 = 0, PLAYER2 = 1};

struct Die
{
	int x, y;
	int value, ornt;
	DIESTATE state;
};


void init_game();

void kill_game();

void draw_game();

void init_dice();

void draw_dice();

int calc_dice(int [], int &);

void calc_quant();

void end_turn(int);

void next_turn();

void reset_dice();

void reset_values();


GAMESTATE game_state = START;
TURN turn = PLAYER1;

Die dice[MAX_DICE];

char buffer[80];

bool first_roll = true;
bool last_turn = false;
bool farkle = false;
bool draw = true;

int lock_dice[6]; //locked quantities
int select_dice[6]; //selected and locked quantities
int all_dice[6]; //all quantities

int total = 0;
int lock = 0;
int all = 0;

int player_score[] = {0,0};
int next_player[] = {PLAYER2,PLAYER1};

int multiplier[] = {1,2,4,8};

int temp_roll_score = 0;
int temp_lock_score = 0;
int temp_select_score = 0;
int temp_total_score = 0;

HDC hdc;

HINSTANCE g_hinstance = NULL;

HWND g_hwnd = NULL;

HBRUSH white_brush = NULL;
HBRUSH black_brush = NULL;
HBRUSH lightgrey_brush = NULL;
HBRUSH grey_brush = NULL;
HPEN black_pen = NULL;
HPEN red_pen = NULL;
HPEN green_pen = NULL;
HPEN blue_pen = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//HDC hdc;
	PAINTSTRUCT ps;
	
	switch(msg)
	{
		case WM_CREATE:
		{
			
			return(0);
		}break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);

			draw = true;
			return(0);
		}break;

		case WM_COMMAND:
		{
			switch(LOWORD(wparam))
			{
				case MENU_FILE_ID_NEW:
				{
					game_state = PLAY;	
					draw = true;
					first_roll = true;
					last_turn = false;
					player_score[PLAYER1] = player_score[PLAYER2] = 0;
					reset_values();
					init_dice();
					//SendMessage(hwnd,WM_PAINT,wparam,lparam);
				}break;

				case MENU_FILE_ID_EXIT:
				{
					SendMessage(hwnd,WM_DESTROY,wparam,lparam);
					PlaySound(MAKEINTRESOURCE(SOUND_ID_EXIT), g_hinstance, SND_SYNC| SND_RESOURCE);
				}break;

				case MENU_HELP_ID_ABOUT:
				{
					MessageBox(hwnd,"Programmed by Collin Munson \nDate Completed June 18, 2007\nCopyright 2007", "ABOUT", MB_OK );//| MB_ICONEXCLAMATION);
				}break;

				case BUTTON_ROLL_DICE:
				{
					switch(game_state)
					{
						case PLAY:
						{
							if(total == 6)
							{
								first_roll = true;
								temp_select_score = 0;
								temp_total_score += temp_roll_score;
								
							}
						
							if(first_roll || total > lock)
							{
								if(first_roll)
								{
									first_roll = false;
									total = 0;
									reset_dice();
								}
								int count = 0;
								for(int i = 0; i < MAX_DICE; i++)
								{
									int value;
									if(dice[i].state == ROLLABLE || dice[i].state == DEFAULT)
									{
										value = dice[i].value = rand()%6+1;
										dice[i].ornt = rand()%4;
										dice[i].state = ROLLABLE;
										
									}
									else
									{
										value = dice[i].value;
										dice[i].state = LOCKED;
										count++;
									}
									
								}
								
								calc_quant();
								temp_roll_score = 0;
								all = calc_dice(all_dice, temp_roll_score);
								temp_lock_score = 0;
								lock = calc_dice(lock_dice, temp_lock_score);
								
								if(count == 6 || all == lock && lock != 6)
								{
									draw_dice();
									farkle = true;
									first_roll = true;
									end_turn(0);
									reset_values();
									reset_dice();
									if(last_turn) game_state = GAMEOVER;
								}
							}
						}break;
						default: 
							MessageBox(hwnd,"Can't roll dice until you've started new game","Invalid button press",MB_OK); 
							break;
					}
					draw = true;
				}break;
				case BUTTON_END_TURN:
				{
					switch(game_state)
					{
						case PLAY:
						{
							int score = temp_select_score + temp_total_score;

							if(total != 6 && total > lock)
							{
								if(last_turn)
								{
									game_state = GAMEOVER;
								}

								if(score >= 500 && player_score[turn] == 0 || score >= 300 && player_score[turn] > 0)
								{
									
									end_turn(score);
									draw = true;
									reset_dice();
									reset_values();
									
								}

							}
						}break;
						default: 
							MessageBox(hwnd,"Can't end turn until you've started new game","Invalid button press",MB_OK); 
							break;
					}
				
				}break;
				
			}
		}break;

		case WM_LBUTTONDOWN:
		{
			if(game_state == PLAY)
			{
				int x = LOWORD(lparam);
				int y = HIWORD(lparam);
				for(int i = 0; i < MAX_DICE; i++)
				{
					if((x > dice[i].x && x < dice[i].x + DICE_SIZE && y > dice[i].y && y < dice[i].y + DICE_SIZE) && dice[i].value)
					{
						
						if(dice[i].state == ROLLABLE) dice[i].state = SELECTED;
						else if(dice[i].state == SELECTED) dice [i].state = ROLLABLE;
						calc_quant();
						temp_select_score = 0;
						total = calc_dice(select_dice, temp_select_score);
						draw = true;
					}
				}
			}
			
		}break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return(0);
		}break;
	}
	return(DefWindowProc(hwnd,msg,wparam,lparam));
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	WNDCLASSEX wc;
	MSG msg;
	HWND hwnd;
	//HDC hdc;
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0,0,230));
	wc.hCursor = LoadCursor(hinstance, IDC_ARROW);
	wc.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(hinstance, IDI_APPLICATION);
	wc.hInstance = hinstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.lpszMenuName = MAKEINTRESOURCE(MainMenu);
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
	
	if(!RegisterClassEx(&wc))
		return(0);

	if(!(hwnd = CreateWindowEx(NULL, 
		                       WINDOW_CLASS_NAME, 
		                       "FARKEL", 
		                       WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 
		                       (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH)/2, 
							   (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT)/2, 
		                       WINDOW_WIDTH, WINDOW_HEIGHT, 
		                       NULL, 
		                       NULL, 
		                       hinstance, 
		                       NULL)))
		return(0);
	
	CreateWindowEx(NULL,
		           "button", 
		           "Roll Dice", 
		           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		           150 , 210, 
		           80, 20, 
		           hwnd, 
		           (HMENU)BUTTON_ROLL_DICE,
		           hinstance,
		           NULL);

	CreateWindowEx(NULL,
		           "button", 
		           "End Turn", 
		           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		           270 , 210, 
		           80, 20, 
		           hwnd, 
		           (HMENU)BUTTON_END_TURN, 
		           hinstance, 
		           NULL);
	
	g_hinstance = hinstance;
	g_hwnd = hwnd;

	//seed random number generator
	srand(GetTickCount());

	init_game();
	
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		switch(game_state)
		{
		case START:
			if(draw)
			{
				hdc = GetDC(hwnd);
				TextOut(hdc,150,150,"FARKLE - Select New Game from File to play",(int)strlen("FARKLE - Select New Game from File to play"));
				ReleaseDC(hwnd,hdc);
				draw = false;
			}
			break;
		case PLAY:
			if(draw)
			{
				draw_game();
				draw = false;
			}
			break;
		case GAMEOVER:
			if(draw)
			{
				hdc = GetDC(hwnd);
				RECT rect = {15,60,530,190};
				FillRect(hdc,&rect,CreateSolidBrush(RGB(0,0,170)));
				rect.top = 30;
				rect.bottom = 46;
				FillRect(hdc,&rect,CreateSolidBrush(RGB(0,0,170)));

				COLORREF p1_color = RGB(255,255,255), p2_color = RGB(255,255,255);
				if(turn == PLAYER1)
					p1_color = RGB(255,255,0);
				else if(turn == PLAYER2)
					p2_color = RGB(255,255,0);

				sprintf_s(buffer,"Player 1 Score %d ", player_score[PLAYER1]);
				SetTextColor(hdc,p1_color);
				TextOut(hdc,15,30,buffer,(int)strlen(buffer));
				sprintf_s(buffer,"Player 2 Score %d ", player_score[PLAYER2]);
				SetTextColor(hdc,p2_color);
				TextOut(hdc,275,30,buffer,(int)strlen(buffer));
				
				SetTextColor(hdc,RGB(0,255,0));
				if(player_score[PLAYER1] > player_score[PLAYER2])
					sprintf_s(buffer,"PLAYER 1 IS THE WINNER!",player_score[PLAYER1],player_score[PLAYER2]);
				else if(player_score[PLAYER2] > player_score[PLAYER1])
					sprintf_s(buffer,"PLAYER 2 IS THE WINNER!",player_score[PLAYER2],player_score[PLAYER1]);
				else sprintf_s(buffer,"PLAYER 1 AND 2 TIE at %d",player_score[PLAYER1]);

				TextOut(hdc,140,130,buffer,(int)strlen(buffer));
				ReleaseDC(hwnd,hdc);
				SetTextColor(hdc,RGB(255,0,0));
				if(farkle)
				{
					TextOut(hdc,400,160,"FARKEL",(int)strlen("FARKEL"));
					farkle = false;
				}
				draw = false;
			}
			break;
		}
	
	}
	kill_game();
	return((int)msg.wParam);
}

void init_game()
{
	white_brush = CreateSolidBrush(RGB(255,255,255));
	black_brush = CreateSolidBrush(RGB(0,0,0));
	lightgrey_brush = CreateSolidBrush(RGB(230,230,230));
	grey_brush = CreateSolidBrush(RGB(170,170,170));
	black_pen = CreatePen(PS_SOLID,2,RGB(0,0,0));
	blue_pen = CreatePen(PS_SOLID,2,RGB(0,0,255));
	green_pen = CreatePen(PS_SOLID,2,RGB(0,255,0));
	red_pen = CreatePen(PS_SOLID,2,RGB(255,0,0));

	init_dice();
}

void kill_game()
{
	if(white_brush) DeleteObject(white_brush);
	if(black_brush) DeleteObject(black_brush);
	if(lightgrey_brush) DeleteObject(lightgrey_brush);
	if(grey_brush) DeleteObject(grey_brush);
	if(black_pen) DeleteObject(black_pen);
	if(blue_pen) DeleteObject(blue_pen);
	if(green_pen) DeleteObject(green_pen);
	if(red_pen) DeleteObject(red_pen);
}

void draw_game()
{
	hdc = GetDC(g_hwnd);
	RECT rect = {15,60,530,190};
	FillRect(hdc,&rect,CreateSolidBrush(RGB(0,0,170)));
	rect.top = 30;
	rect.bottom = 46;
	FillRect(hdc,&rect,CreateSolidBrush(RGB(0,0,170)));
	SetBkMode(hdc,TRANSPARENT);

	COLORREF p1_color = RGB(255,255,255), p2_color = RGB(255,255,255);
	if(turn == PLAYER1)
		p1_color = RGB(255,255,0);
	else if(turn == PLAYER2)
		p2_color = RGB(255,255,0);
		
	sprintf_s(buffer,"Player 1 Score %d ", player_score[PLAYER1]);
	SetTextColor(hdc,p1_color);
	TextOut(hdc,15,30,buffer,(int)strlen(buffer));
	sprintf_s(buffer,"Player 2 Score %d ", player_score[PLAYER2]);
	SetTextColor(hdc,p2_color);
	TextOut(hdc,275,30,buffer,(int)strlen(buffer));
	sprintf_s(buffer,"Player %d turn, current roll worth %d, selected worth %d ", turn + 1, temp_roll_score, temp_select_score);
	TextOut(hdc,15,155,buffer,(int)strlen(buffer));
	sprintf_s(buffer,"current total of %d for round ", temp_total_score + temp_select_score);
	TextOut(hdc,15,171,buffer,(int)strlen(buffer));
	draw_dice();
	SetTextColor(hdc,RGB(255,0,0));
	if(farkle)
	{
		TextOut(hdc,400,160,"FARKEL",(int)strlen("FARKEL"));
		farkle = false;
	}
	
	ReleaseDC(g_hwnd, hdc);
}

void init_dice()
{
	for(int i = 0; i < MAX_DICE; i++)
	{
		dice[i].x = int(20 + i * DICE_SIZE * 1.40);
		dice[i].y = 90;
		dice[i].ornt = 0;
		dice[i].state = DEFAULT;
		dice[i].value = 0;
	}
}

void reset_dice()
{
	for(int i = 0; i < MAX_DICE; i++)
		dice[i].state = DEFAULT;
}

void draw_dice()
{
	//one = two = three = four = five = six = 0;
	for(int i = 0; i < MAX_DICE; i++)
	{
		if(dice[i].value)
		{
			int x = dice[i].x;
			int y = dice[i].y;
			
			POINT top[4] = {{x, y} , {int(x + DICE_SIZE * 0.4), int(y - DICE_SIZE * 0.4)} , {int(x + DICE_SIZE * 1.4), int(y - DICE_SIZE * 0.4)}, {x + DICE_SIZE,y}};
			POINT side[4] = {{x + DICE_SIZE, y}, {int(x + DICE_SIZE * 1.4), int(y - DICE_SIZE * 0.4)}, {int(x + DICE_SIZE * 1.4), int(y - DICE_SIZE * -0.6)}, {x + DICE_SIZE, y + DICE_SIZE}};

			switch(dice[i].state)
			{
				case DEFAULT:
				{
					SelectObject(hdc, green_pen);
					SelectObject(hdc, black_brush);
					Rectangle(hdc, x, y, x + DICE_SIZE, y + DICE_SIZE); // FRONT
			
					
					Polygon(hdc, top, 4);		//TOP

					
					Polygon(hdc, side, 4); //SIDE

					SelectObject(hdc, white_brush);
				}break;

				case SELECTED:
				{
					SelectObject(hdc, green_pen);
					SelectObject(hdc, white_brush);
					Rectangle(hdc, x, y, x + DICE_SIZE, y + DICE_SIZE); // FRONT
			
					SelectObject(hdc, lightgrey_brush);
					Polygon(hdc, top, 4);		//TOP
					
					SelectObject(hdc, grey_brush);
					Polygon(hdc, side, 4); //SIDE

					SelectObject(hdc, black_brush);
				}break;
				case LOCKED:
				{
					SelectObject(hdc, red_pen);
					SelectObject(hdc, black_brush);
					Rectangle(hdc, x, y, x + DICE_SIZE, y + DICE_SIZE); // FRONT
			
					//SelectObject(hdc, lightgrey_brush);
					Polygon(hdc, top, 4);		//TOP

					//SelectObject(hdc, grey_brush);
					Polygon(hdc, side, 4); //SIDE

					SelectObject(hdc, black_brush);
				}break;
				default:
				{
					SelectObject(hdc, black_pen);
					SelectObject(hdc, white_brush);
					Rectangle(hdc, x, y, x + DICE_SIZE, y + DICE_SIZE); // FRONT
			
					SelectObject(hdc, lightgrey_brush);
					Polygon(hdc, top, 4);		//TOP

					SelectObject(hdc, grey_brush);
					Polygon(hdc, side, 4); //SIDE

					SelectObject(hdc, black_brush);
				}break;
				
			}
		
			switch(dice[i].value)
			{
				case 1:
					switch(dice[i].ornt)
					{
						case 0:
						{
							//one
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6)); //front middle dot

							//orientation 1
							//Ellipse(hdc, x + DICE_SIZE * 0.6, y + DICE_SIZE * -0.25, x + DICE_SIZE * 0.8, y + DICE_SIZE * -0.15); //top middle dot
							//four on top
							Ellipse(hdc, int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.05)); //top lower-left dot
							Ellipse(hdc, int(x + DICE_SIZE * 0.2), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.05)); //top lower-right dot
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25)); //top upper-left dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 1.2), int(y + DICE_SIZE * -0.25)); //top upper-right dot

							//five on side
							Ellipse(hdc, int(x + DICE_SIZE * 1.15), int(y + DICE_SIZE * 0.2), int(x + DICE_SIZE * 1.25), int(y + DICE_SIZE * 0.4)); //side middle dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.02), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.22)); //side left-upper corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * 0.38), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.58)); //side lower-right corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
							//end o1
						}break;
						case 1:
						{
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6)); //front middle dot

							Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15));
							Ellipse(hdc, int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.05)); //top lower-left dot
							Ellipse(hdc, int(x + DICE_SIZE * 0.2), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.05)); //top lower-right dot
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25)); //top upper-left dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 1.2), int(y + DICE_SIZE * -0.25)); //top upper-right dot

							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.15), int(y + DICE_SIZE * 0.2), int(x + DICE_SIZE * 1.25), int(y + DICE_SIZE * 0.4)); //side middle dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
						}break;
						case 2:
						{
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6)); //front middle dot

							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25)); //top upper-left dot
							Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15));
							Ellipse(hdc, int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.05)); //top lower-left dot

							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.02), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.22)); //side left-upper corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * 0.38), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.58)); //side lower-right corner dot
						}break;
						case 3:
						{
							Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6)); //front middle dot

							Ellipse(hdc, int(x + DICE_SIZE * 0.2), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.05)); //top lower-right dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 1.2), int(y + DICE_SIZE * -0.25)); //top upper-right dot

							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.02), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.22)); //side left-upper corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * 0.38), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.58)); //side lower-right corner dot
							Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
						}break;
					}
					
					break;
				case 2:
								
					//two on front
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.9));
					
					//face one dot
					Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15));

					//side 
					
					//side face 3 dots
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8));
					Ellipse(hdc, int(x + DICE_SIZE * 1.15), int(y + DICE_SIZE * 0.2), int(x + DICE_SIZE * 1.25), int(y + DICE_SIZE * 0.4));
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0));
					
					break;
				case 3:
					//three on front
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.9));
					
					//one on top
					Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15)); //top middle dot

					//five on side
					Ellipse(hdc, int(x + DICE_SIZE * 1.15), int(y + DICE_SIZE * 0.2), int(x + DICE_SIZE * 1.25), int(y + DICE_SIZE * 0.4)); //side middle dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.02), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.22)); //side left-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * 0.38), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.58)); //side lower-right corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
					
					break;
				case 4:
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.3));

					//one on top
					Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15)); //top middle dot

					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
					
					break;
				case 5:
					//five on front
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * 0.6));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.3));

					//one on top
					Ellipse(hdc, int(x + DICE_SIZE * 0.6), int(y + DICE_SIZE * -0.25), int(x + DICE_SIZE * 0.8), int(y + DICE_SIZE * -0.15)); //top middle dot

					//four on side
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.02), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.22)); //side left-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * 0.38), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.58)); //side lower-right corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
					
					break;
				case 6:
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.7), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.9));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.1), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.3));
					Ellipse(hdc, int(x + DICE_SIZE * 0.1), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.3), int(y + DICE_SIZE * 0.6));
					Ellipse(hdc, int(x + DICE_SIZE * 0.7), int(y + DICE_SIZE * 0.4), int(x + DICE_SIZE * 0.9), int(y + DICE_SIZE * 0.6));

					Ellipse(hdc, int(x + DICE_SIZE * 0.2), int(y + DICE_SIZE * -0.15), int(x + DICE_SIZE * 0.4), int(y + DICE_SIZE * -0.05)); //top lower-right dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.0), int(y + DICE_SIZE * -0.35), int(x + DICE_SIZE * 1.2), int(y + DICE_SIZE * -0.25)); //top upper-right dot

					Ellipse(hdc, int(x + DICE_SIZE * 1.15), int(y + DICE_SIZE * 0.2), int(x + DICE_SIZE * 1.25), int(y + DICE_SIZE * 0.4)); //side middle dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.26), int(y + DICE_SIZE * -0.2), int(x + DICE_SIZE * 1.36), int(y + DICE_SIZE * 0.0)); //side right-upper corner dot
					Ellipse(hdc, int(x + DICE_SIZE * 1.04), int(y + DICE_SIZE * 0.6), int(x + DICE_SIZE * 1.14), int(y + DICE_SIZE * 0.8)); //side lower-left corner dot
					
					break;
			}
		}
	}
}

int calc_dice(int q[], int &score)
{
	int count = 0;
	if(q[0] && q[1] && q[2] && q[3] && q[4] && q[5])
	{
		score += 1200 ;
		return 6;
	}
	for(int i = 0; i < 6; i++) 
	{
		int value = i + 1;
		int quant = q[i];

		switch(quant) 
		{
			case 0:
				break;
			case 1:
			case 2:
				switch(value) 
				{
					case 1:
						score += 100 * quant;
						count += quant;
						break;
					case 5:
						score += 50 * quant;
						count += quant;
						break;
				}
				break;
			default:
				switch(value) 
				{
					case 1:
						score += multiplier[quant - 3] * 1000;
						count += quant;
						break;
					default:
						score += multiplier[quant - 3] * 100 * value;
						count += quant;
						break;
				}
				break;
		}
	}
	return (count);

}

void calc_quant()
{
	for(int i = 0; i < MAX_DICE; i++)
	{
		lock_dice[i] = 0;
		select_dice[i] = 0;
		all_dice[i] = 0;
	}

	for(int i = 0; i < MAX_DICE; i++)
	{
		int n = dice[i].value - 1;
		all_dice[n]++;
		switch(dice[i].state)
		{
			case LOCKED:
				lock_dice[n]++;
			case SELECTED:
				select_dice[n]++;
				break;
		}
		
	}
}

void end_turn(int score)
{
	player_score[turn] += score;
	if(player_score[turn] >= 10000) last_turn = true;
	turn = (TURN)next_player[turn];
}

void reset_values()
{
	first_roll = true;
	temp_total_score = 0;
	temp_roll_score = 0;
	temp_select_score = 0;
	lock = 0;
	all = 0;
	total = 0;
}
