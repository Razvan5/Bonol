#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <fstream>
#include <unistd.h>

#define CENTERX 200
#define CENTERY 200

#define MyBLUE COLOR(40, 215, 255)
#define MyORANGE COLOR(240,162,2)

using namespace std;

//reparat continue

unsigned int maxsize=4,mouse_clicks=0,width=400,height=400,coord_x=width/4,coord_y=height/4,coord_north=coord_y,coord_west=coord_x,squareNumber=4,squareLenght=100,player_greyturn=2;
bool done=false,verifnonegatives=false,firstmovedone=false,player1_time_lost=false,player2_timer_lost=false;
int bonolTable[9][9],bonolTableGrid[9][9],rotation[9];
int player_turn=1,rotation_number=0,player_number,index_rotation,number_of_turns=0,menu_number=1,player_color1=LIGHTRED,player_color2=LIGHTCYAN,player_color3=YELLOW;//menu_number=1,2,3,4,5...
bool pressed=false,backToMenu=false,continue_pressed=false;
int column_undo,row_undo,rotation_undo;
int sec=0,mins=0,mocanu_sec=0,maxMins=4,maxSec=20;
int sizeUndo,bonolTableUndo[5][5][200];
int language=0,music_track=0,won1=0,won2=0;//0=eng 1=rom 2=fra
char music[30];

ofstream continue_file("continue.in");

int continueGameMatrix[100][5][5];
int undoStep=1;

void initUndoTable()
{int i,j;

    for(i=1;i<=4;i++)
        for(j=1;j<=4;j++)
            bonolTableUndo[i][j][0]=bonolTable[i][j];
}

struct value
{
    int row,column,rot;
};

struct undo
{
    int sizeOfUndo;
    value undoArray[101];
};

undo deck;

void initUndo()
{
    deck.sizeOfUndo=1;
    deck.undoArray[0].row=4;
    deck.undoArray[0].column=2;
    deck.undoArray[0].rot=5;

    deck.undoArray[5].row=1;
    deck.undoArray[5].column=3;
    deck.undoArray[5].rot=7;

}
void pushUndo(int coordX,int coordY,int Rotation)
{
    deck.sizeOfUndo++;
    deck.undoArray[deck.sizeOfUndo].row=coordX;
    deck.undoArray[deck.sizeOfUndo].column=coordY;
    deck.undoArray[deck.sizeOfUndo].rot=Rotation;
}
void popUndo()
{
    deck.sizeOfUndo--;
}
bool isUndoEmpty()
{
    return deck.sizeOfUndo==0;
}

void turnSound()
{
    if(pressed==true)
        {PlaySound(NULL,0,0);pressed=false;}
    else {PlaySound(music,NULL,SND_ASYNC);pressed=true;}
}
int getRow(int y)
{
    return (y-coord_north)/squareLenght+1;
}
int getColumn(int x)
{
    return (x-coord_west)/squareLenght+1;
}
int getCoordX(int column)
{
    return coord_west+squareLenght*(column-1)+squareLenght/2;
}
int getCoordY(int row)
{
    return coord_north+squareLenght*(row-1)+squareLenght/2;
}
void buildXMark(int x,int y,short int tileColor)
{
     setcolor(tileColor);
     line(x-squareLenght/4,y+squareLenght/4,x+squareLenght/4,y-squareLenght/4);
     line(x+squareLenght/4,y+squareLenght/4,x-squareLenght/4,y-squareLenght/4);

}
void drawAllXMark(int COLOR1)
{unsigned int i,j;

     for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]==7)
                buildXMark(getCoordX(j)+600,getCoordY(i),player_color3);

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]==WHITE || bonolTable[i][j]==-1)
                {buildXMark(getCoordX(j)+600,getCoordY(i),COLOR1);}

}
void buildSquare(short int tileColor)
{
    setcolor(tileColor);
    setfillstyle(SOLID_FILL,tileColor);
    line(coord_x-squareLenght/2,coord_y-squareLenght/2,coord_x-squareLenght/2,coord_y+squareLenght/2);
    line(coord_x-squareLenght/2,coord_y+squareLenght/2,coord_x+squareLenght/2,coord_y+squareLenght/2);
    line(coord_x+squareLenght/2,coord_y+squareLenght/2,coord_x+squareLenght/2,coord_y-squareLenght/2);
    line(coord_x+squareLenght/2,coord_y-squareLenght/2,coord_x-squareLenght/2,coord_y-squareLenght/2);
    //line(coord_x,coord_y,coord_x+squareLenght/2,coord_y);
    floodfill(coord_x,coord_y,tileColor);
}
// construieste doar liniile patratului
void buildSquareGrid()
{
    setcolor(DARKGRAY);
    line(coord_x-squareLenght/2,coord_y-squareLenght/2,coord_x-squareLenght/2,coord_y+squareLenght/2);
    line(coord_x-squareLenght/2,coord_y+squareLenght/2,coord_x+squareLenght/2,coord_y+squareLenght/2);
    line(coord_x+squareLenght/2,coord_y+squareLenght/2,coord_x+squareLenght/2,coord_y-squareLenght/2);
    line(coord_x+squareLenght/2,coord_y-squareLenght/2,coord_x-squareLenght/2,coord_y-squareLenght/2);

}
void makeScreenColor()
{
    int x,y;

    x=getmaxx();
    y=getmaxy();

    setcolor(MyBLUE);
    setfillstyle(SOLID_FILL,MyBLUE);
    rectangle(0,0,x,y);
    floodfill(2,2,MyBLUE);
}
// initTable si initEmptyTable initializeza tabla de joc cu modele stocate in fisiere
void initTable()
{
    ifstream file("startlevel.in");
    for(unsigned int i=1;i<=squareNumber;i++)
        for(unsigned int j=1;j<=squareNumber;j++)
                {file>>bonolTable[i][j];}

    file.close();
}
void initEmptyTable()
{
    ifstream file("empty.in");
    for(unsigned int i=1;i<=squareNumber;i++)
        for(unsigned int j=1;j<=squareNumber;j++)
                {file>>bonolTableGrid[i][j];}

    file.close();
}

void drawTable(unsigned int north,unsigned int west,int table[9][9])
{
    for(unsigned int i=1;i<=squareNumber;i++)
        for(unsigned int j=1;j<=squareNumber;j++)
            {
                coord_y=west+(i)*squareLenght-squareLenght/2;
                coord_x=north+(j)*squareLenght-squareLenght/2;

                //buildSquareGrid();

                if(table[i][j]==1)
                    buildSquare(player_color1);
                    else if(table[i][j]==2)
                            buildSquare(player_color2);
                        else buildSquare(WHITE);
                          if(table[i][j]==7)
                                    {setcolor(player_color3);setfillstyle(SOLID_FILL,player_color3);circle(getCoordX(j),getCoordY(i),2*squareLenght/5);floodfill(getCoordX(j),getCoordY(i),player_color3);setcolor(player_color3);setfillstyle(HATCH_FILL,player_color3);circle(getCoordX(j)+600,getCoordY(i),2*squareLenght/5);floodfill(getCoordX(j)+600,getCoordY(i),player_color3);}


                //buildSquare(bonolTable[i][j]);
                buildSquareGrid();
            }
}
/*
void continueGame()
{
    int i,j;
  for(i=1;i<=4;i++)
       for(j=1;j<=4;j++)
        continueGameMatrix[undoStep][i][j]=bonolTable[i][j];
          undoStep++;

}

void drawGameHistory()
{int i,j;

    setbkcolor(BLACK);
    cleardevice();

    for(i=1;i<=4;i++)
        for(j=1;j<=4;j++)
        bonolTable[i][j]=continueGameMatrix[undoStep][i][j];

        //drawTable(coord_north,coord_west,continueGameMatrix[undoStep]);

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            int x=mousex();
            if(x<800)
                {undoStep--;drawGameHistory();
                }
            else if(x>800)
                 {undoStep++;drawGameHistory();
                }
        }

}
*/
void drawMainMenu()//menu=1
{char buton[100];
   // makeScreenColor();
    //settextstyle(SIMPLEX_FONT,0,3);

    coord_y=75;
    coord_x=700;
    buildSquare(WHITE);
    setcolor(LIGHTBLUE);
    setbkcolor(WHITE);
    strcpy(buton,"title.bmp");
    readimagefile(buton,500,0,900,350);



    coord_y=300;
    coord_x=700;
    buildSquare(LIGHTGRAY);
    buildSquareGrid();
    setcolor(LIGHTGREEN);
    setbkcolor(LIGHTGRAY);
    strcpy(buton,"play.bmp");

    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }

    cout<<buton;
    outtextxy(660,290,"Play a game");//->playmenu menu=2;survival if menu=2 and play a game if manu=1
    readimagefile(buton,650,250,750,350);

    coord_y=500;
    coord_x=700;
    buildSquare(GREEN);
    buildSquareGrid();
    setcolor(LIGHTGRAY);
    setbkcolor(GREEN);
    strcpy(buton,"set.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    outtextxy(675,490,"Settings");//->settingsmenu menu=3
    readimagefile(buton,650,450,750,550);


    strcpy(buton,"size.bmp");
    readimagefile(buton,350,450,450,550);
    //readimagefile(buton,3.5*squareLenght,4.5*squareLenght,4.5*squareLenght,5.5*squareLenght);

    itoa(squareLenght,buton,10);
     if(squareLenght<100)
    strcat(buton,"  ");
    setbkcolor(BLACK);
    setcolor(LIGHTCYAN);
    outtextxy(390,550,buton);

    coord_y=700;
    coord_x=700;
    buildSquare(LIGHTBLUE);
    setcolor(BLUE);
    setbkcolor(LIGHTBLUE);
    outtextxy(660,690,"How to Play");//->how to window menu=4
    strcpy(buton,"howto.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,650,650,750,750);

    coord_y=700;
    coord_x=400;
    buildSquare(LIGHTRED);
    setcolor(WHITE);
    setbkcolor(LIGHTRED);
    outtextxy(380,690,"Exit");//->esc if menu=1 and back if menu>1
    strcpy(buton,"exit.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);

    strcpy(buton,"highscore.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,950,450,1050,550);

    coord_y=700;
    coord_x=1000;
    buildSquare(YELLOW);
    setcolor(LIGHTRED);
    setbkcolor(YELLOW);
    outtextxy(955,690,"Music ON/OFF");//works all the time
    strcpy(buton,"music.bmp");
    readimagefile("music.bmp",950,650,1050,750);

    setbkcolor(MyBLUE);


}
void drawPlayMenu()//menu=2
{char buton[100];

    coord_y=75;
    coord_x=700;
    buildSquare(WHITE);
    setcolor(LIGHTBLUE);
    setbkcolor(WHITE);
    strcpy(buton,"title.bmp");
    readimagefile(buton,500,0,900,350);

    coord_y=300;
    coord_x=400;
    buildSquare(LIGHTGRAY);
    buildSquareGrid();
    setcolor(LIGHTGREEN);
    setbkcolor(LIGHTGRAY);
    //outtextxy(330,290,"Player vs Player");//works only for menu=2
     strcpy(buton,"pvp.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,250,450,350);

    coord_y=300;
    coord_x=700;
    buildSquare(LIGHTGRAY);
    buildSquareGrid();
    setcolor(LIGHTGREEN);
    setbkcolor(LIGHTGRAY);
    outtextxy(660,290,"Survival");
     strcpy(buton,"continue.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,650,250,750,350);

    coord_y=300;
    coord_x=1000;
    buildSquare(LIGHTGRAY);
    buildSquareGrid();
    setcolor(LIGHTGREEN);
    setbkcolor(LIGHTGRAY);
    //outtextxy(950,290,"Player vs Computer");//works only for menu=2
     strcpy(buton,"pvc.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,950,250,1050,350);

    coord_y=500;
    coord_x=700;
    buildSquare(GREEN);
    buildSquareGrid();
    setcolor(LIGHTGRAY);
    setbkcolor(GREEN);
    outtextxy(675,490,"Settings");//works for all menus!=3
     strcpy(buton,"set.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,650,450,750,550);

    coord_y=700;
    coord_x=700;
    buildSquare(LIGHTBLUE);
    setcolor(BLUE);
    setbkcolor(LIGHTBLUE);
    outtextxy(660,690,"How to Play");
     strcpy(buton,"howto.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,650,650,750,750);

    coord_y=700;
    coord_x=400;
    buildSquare(LIGHTRED);
    setcolor(WHITE);
    setbkcolor(LIGHTRED);
    outtextxy(380,690,"<Back");
    strcpy(buton,"back.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);

    coord_y=700;
    coord_x=1000;
    buildSquare(YELLOW);
    setcolor(LIGHTRED);
    setbkcolor(YELLOW);
    outtextxy(955,690,"Music ON/OFF");
    strcpy(buton,"music.bmp");
    readimagefile(buton,950,650,1050,750);

    setbkcolor(MyBLUE);
}
void drawSettingsMenu()//menu=3
{char buton[100];

    coord_y=75;
    coord_x=700;
    buildSquare(WHITE);
    setcolor(LIGHTBLUE);
    setbkcolor(WHITE);
    strcpy(buton,"title.bmp");
    readimagefile(buton,500,0,900,350);

    coord_y=300;
    coord_x=700;
    buildSquare(LIGHTGRAY);
    buildSquareGrid();
    setcolor(LIGHTGREEN);
    setbkcolor(LIGHTGRAY);
    outtextxy(660,290,"Change Music");
       strcpy(buton,"audio.bmp");
       if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,625,225,775,375);

    coord_y=500;
    coord_x=700;
    buildSquare(GREEN);
    buildSquareGrid();
    setcolor(LIGHTGRAY);
    setbkcolor(GREEN);
    outtextxy(660,490,"Change Color");
       strcpy(buton,"colors.bmp");
       if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,625,425,775,575);

    coord_y=700;
    coord_x=700;
    buildSquare(LIGHTBLUE);
    setcolor(BLUE);
    setbkcolor(LIGHTBLUE);
    outtextxy(660,690,"Difficulty     ");
    strcpy(buton,"dif.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,625,625,775,775);

    coord_y=700;
    coord_x=400;
    buildSquare(LIGHTRED);
    setcolor(WHITE);
    setbkcolor(LIGHTRED);
    outtextxy(380,690,"<Back");
    strcpy(buton,"back.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);

    strcpy(buton,"lang_eng.bmp");
    if(language==1)
    {
        buton[strlen(buton)-8]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-8]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,425,225,575,375);
    strcpy(buton,"timer.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,825,225,975,375);

    setbkcolor(MyBLUE);



}
void drawColorMenu()//menu=4
{char buton[100];
    strcpy(buton,"title.bmp");
    readimagefile(buton,500,0,900,335);

    coord_y=300;
    coord_x=600;
    buildSquare(player_color1);
    coord_y=300;
    coord_x=700;
    buildSquare(player_color2);
    coord_y=300;
    coord_x=800;
    buildSquare(player_color3);
    initTable();
    //drawTable(coord_north+400,coord_west+260,bonolTable);

    for(unsigned int i=1;i<=squareNumber;i++)
        for(unsigned int j=1;j<=squareNumber;j++)
            {
                coord_y=coord_west+260+(i)*squareLenght-squareLenght/2;
                coord_x=coord_north+400+(j)*squareLenght-squareLenght/2;

                //buildSquareGrid();

                if(bonolTable[i][j]==1)
                    buildSquare(player_color1);
                    else if(bonolTable[i][j]==2)
                            buildSquare(player_color2);
                        else buildSquare(WHITE);
                          if(bonolTable[i][j]==7)
                            {buildSquare(player_color3);}


                //buildSquare(bonolTable[i][j]);
                buildSquareGrid();
            }

   // readimagefile("colors.bmp",625,425,775,575);
     strcpy(buton,"back.bmp");
     if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
     readimagefile(buton,350,650,450,750);
}
unsigned int next_image=1;
void drawHowtoMenu()//menu=5//remember to add a try the controls!
{char buton[100];
    switch(next_image)
    {
    case 1:readimagefile("howto1.bmp",150,50,1300,600);
    break;
    case 2:readimagefile("howto2.bmp",150,50,1300,600);
    break;
    case 3:{readimagefile("howto3.bmp",150,50,1300,600);}
    break;
    case 4:{readimagefile("won1.bmp",150,50,1300,600); next_image=1;}
    break;
    }

    strcpy(buton,"back.bmp");
    if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);

    readimagefile("front.bmp",950,650,1050,750);

}
void drawTimerMenu()//menu==6
{char sir1[5],sir2[5],buton[100];

    itoa(maxMins,sir1,10);
    itoa(maxSec,sir2,10);

    outtextxy(650,350,sir1);
    outtextxy(700,350,":");
    outtextxy(750,350,sir2);


    readimagefile("up.bmp",625,125,775,275);
    readimagefile("down.bmp",625,425,775,575);

       strcpy(buton,"back.bmp");
       if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);
}
ofstream highscore("highscore.in");
void inithighscore()
{
    ifstream highscore("highscore.in");

    highscore>>won1>>won2;

    cout<<won1<<' '<<won2;

}
void drawclas()
{
    //inithighscore();

    char a[10],b[10],buton[30];
    itoa(won1,a,10);
    itoa(won2,b,10);
    setcolor (LIGHTBLUE);
    rectangle(500,100,900,200);

   // floodfill (140,140,LIGHTBLUE);
//bar(110,110,490,190);

   setcolor (YELLOW);
    rectangle(520,250,880,350);

   //floodfill (140,140,LIGHTBLUE);
//bar(110,260,490,340);

   setcolor (RED);
    rectangle(520,400,880,500);

   // floodfill (140,140,LIGHTBLUE);
//bar(110,410,490,490);
setcolor(WHITE);
outtextxy(650,150,"HIGHSCORE");
outtextxy (800,300,"TIMES!");
outtextxy (800,450,"TIMES!");
if(won1>won2)
{
    outtextxy(540,300,"PLAYER 1 HAS WON ");
    outtextxy(700,300,a);
    outtextxy(540,450,"PLAYER 2 HAS WON");
    outtextxy(700,450,b);
}else {
outtextxy(540,300,"PLAYER 2 HAS WON ");outtextxy(700,300,b);
    outtextxy(540,450,"PLAYER 1 HAS WON"); outtextxy(700,450,a);

}
for(int i=0;i<1000;i=i+10)
{setfillstyle(SOLID_FILL,LIGHTBLUE);
    bar(450,i,460,i+10); bar(917,i,927,i+10);
     setfillstyle(SOLID_FILL,YELLOW);
    bar(461,i,471,i+10); bar(928,i,939,i+10);
     setfillstyle(SOLID_FILL,LIGHTRED);
    bar(472,i,483,i+10);bar(940,i,950,i+10);
    delay(4);
}

 strcpy(buton,"back.bmp");
       if(language==1)
    {
        buton[strlen(buton)-4]=0;
        strcat(buton,"_rom.bmp");
    }
    else if(language==2)
            {
                buton[strlen(buton)-4]=0;
                strcat(buton,"_fre.bmp");
            }
    readimagefile(buton,350,650,450,750);


    highscore<<won1<<' '<<won2;

}
void detectButtonLocation(int &tasta)
{int x,y;

    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

        if(x>=950 && y>=450 && x<=1050 && y<=550 && menu_number==1)
                    {setcolor(BLACK);cleardevice();drawclas();menu_number=2;}
        if(x>=825 && x<=975 && y>=225 && y<=375 && menu_number==3)
                    {setbkcolor(BLACK);cleardevice();drawTimerMenu();menu_number=6;}
        if(x>=350 && y>=450 && x<=450 && y<=550 && menu_number==1)
                    {squareLenght-=10; if(squareLenght<=10)squareLenght=100;drawMainMenu();}

        if(x>=650 && x<=750 && y>=250 && y<=350 && menu_number==3)
                    {
                        music_track++;
                        music_track=music_track%5;

                        if(music_track==1)
                        {
                            strcpy(music,"notimpos.wav");
                            PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                        }
                        else if(music_track==2)
                        {
                            strcpy(music,"melody.wav");
                            PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                        }
                        else if(music_track==3)
                                {
                                    strcpy(music,"stars.wav");
                                    PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                                }
                             else if(music_track==4)
                                    {
                                        strcpy(music,"flag.wav");
                                        PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                                    }
                                    else if(music_track==0 && maxMins==4 && maxSec==20)
                                            {
                                                strcpy(music,"lasagna.wav");
                                                PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                                            }
                                            else if(music_track==0)
                                                {
                                                    strcpy(music,"darkness.wav");
                                                    PlaySound(music,NULL,SND_ASYNC || SND_LOOP);
                                                }


                    }


        if(x>=650 && x<=750 && y>=250 && y<=350 && menu_number==1)
                    {setbkcolor(BLACK);cleardevice();drawPlayMenu();menu_number=2;}
        else
        if(x>=650 && x<=750 && y>=250 && y<=350 && menu_number==2 && continue_pressed==false)
                    {setbkcolor(MyBLUE);cleardevice();tasta=13;continue_pressed=true;menu_number=1;}

         if(x>=350 && x<=450 && y>=250 && y<=350 && menu_number==2)
                             {tasta=13;menu_number=1;}





        if(x>=950 && x<=1050 && y>=250 && y<=350 && menu_number==2)
                    {tasta=98;menu_number=1;}

        if(x>=650 && x<=750 && y>=450 && y<=550 && menu_number!=3 && menu_number!=6)
                    {setbkcolor(BLACK);cleardevice();drawSettingsMenu();menu_number=3;}

        else  if(x>=650 && x<=750 && y>=450 && y<=550 && menu_number==3)
                    {setbkcolor(BLACK);cleardevice();drawColorMenu();menu_number=4;}

        if(x>=550 && x<=650 && y>=250 && y<=350 && menu_number==4)
                    {
                        player_color1++;
                        if(player_color1==player_color2 || player_color1==player_color3)
                                        player_color1++;
                         if(player_color1>13)
                                player_color1=1;
                        else if(player_color1==player_color2)
                                player_color1++;
                         drawColorMenu();
                    }

        if(x>=650 && x<=750 && y>=250 && y<=350 && menu_number==4)
                     {
                         player_color2++;

                         if(player_color2==player_color1 || player_color2==player_color3)
                                                  player_color2++;
                             if(player_color2>13 && player_color2!=player_color1)
                                        player_color2=1;
                                else if(player_color2==player_color1)
                                        player_color2++;

                                drawColorMenu();
                     }

        if(x>=750 && x<=850 && y>=250 && y<=350 && menu_number==4)
                    {
                        player_color3++;
                        if(player_color3==player_color2 || player_color3==player_color1)
                                        player_color3++;
                          if(player_color3>13 && player_color3!=player_color1)
                            player_color3=1;
                          else if(player_color3==player_color1)
                                        player_color3++;

                            drawColorMenu();

                    }

        if(x>=350 && x<=450 && y>=650 && y<=750 && menu_number==1)
                    {tasta=27;}

        if(x>=350 && x<=450 && y>=650 && y<=750 && menu_number!=1)
                    {setbkcolor(BLACK);cleardevice();drawMainMenu();menu_number=1;}

        if(x>=950 && x<=1050 && y>=650 && y<=750 && menu_number!=4 && menu_number!=5)
                    turnSound();

        if(x>=650 && x<=750 && y>=650 && y<=750 && menu_number!=5 && menu_number!=3 && menu_number!=6)
                    {setbkcolor(BLACK);cleardevice();drawHowtoMenu();menu_number=5;}

        if(x>=950 && x<=1050 && y>=650 && y<=750 && menu_number==5)
                    {next_image++;drawHowtoMenu();menu_number=5;cout<<next_image<<' ';}

        //625,425,775,575
        if(x>=625 && x<=775 && y>=425 && y<=575 && menu_number==6)
                    {maxSec--;if(maxSec<0){maxSec=59;--maxMins;}menu_number=6;cleardevice();drawTimerMenu();}
        //625,125,775,275
        if(x>=625 && x<=775 && y>=125 && y<=225 && menu_number==6)
                    {maxSec++;if(maxSec==60){maxSec=0;++maxMins;}menu_number=6;cleardevice();drawTimerMenu();}
        if(x>=425 && y>=225 && x<=575 && y<=375 && menu_number==3)
                    {language++;if(language>2)language=0;drawSettingsMenu();}



    }

}
void clearAllNegatives()
{unsigned int i,j;

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]<0)
               {
                   bonolTable[i][j]=WHITE;
                   verifnonegatives=true;
               }

}

bool detectMissingPlayer()
{unsigned int i,j,player;

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==3)
            player=2;
         else player=7;

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]==player)
                    return false;
    return true;
}

int returnGreyNumber()
{unsigned int i,j,sum=0;

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]==LIGHTGRAY)
                        sum++;
    return sum;
}

void UndoProbableMove(int row,int column,int rotationFixed)
{int i,j,player;

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==3)
            player=2;
         else player=7;

    j=column;
    i=row;

    cout<<"UNDOMOVE"<<i<< ' '<<j<<"rot"<<deck.undoArray[deck.sizeOfUndo].rot<<' '<<rotationFixed<<endl;

    switch (rotationFixed-1)
    {
    case 1:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))

    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i][j-2]=player;
        bonolTable[i-1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 2:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i][j+2]=player;
        bonolTable[i-1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 3:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i-1][j]=player;
        bonolTable[i-2][j]=player;
        bonolTable[i][j-1]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
   }
    break;
    case 4:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i-1][j]=player;
        bonolTable[i-2][j]=player;
        bonolTable[i][j+1]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);

    }
    break;
    case 5:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i][j-2]=player;
        bonolTable[i+1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);

    }
    break;
    case 6:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i][j+2]=player;
        bonolTable[i+1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 7:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i+1][j]=player;
        bonolTable[i+2][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 8:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i+1][j]=player;
        bonolTable[i+2][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    }


}
void showCurrentPlayer()
{
 int player,aux;
 char nume[]="Current    Player: ";

  if(player_turn%4==1)
        player=1;
    else if(player_turn%4==2)
            player=7;
         else if(player_turn%4==3)
                    player=2;
              else player=7;

    if(player==1)
    {
        setcolor(player_color1);
        char cif[2];
        itoa(player,cif,10);
        strcat(nume,cif);
        //settextstyle(COMPLEX_FONT,0,1);
        outtextxy(coord_north+448,coord_west-20,nume);

    }
    else if(player==2)
            {
                setcolor(player_color2);
                char cif[2];
                itoa(player,cif,10);
                strcat(nume,cif);
                //settextstyle(COMPLEX_FONT,0,1);
                outtextxy(coord_north+448,coord_west-20,nume);

            }

    else {
            //settextstyle(COMPLEX_FONT,0,1);
            setcolor(player_color3);
            outtextxy(coord_north+448,coord_west-20,"Move Round Piece");
         }

}
void detectGreySquare()
{
 int row,column,x,y,i,j,player;

    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

        //initUndo();

            if (!(x>=coord_west && x<=coord_west+width && y>=coord_north&&y<=coord_north+height))
                           if(x>=200 && y>=600 && x<=400 && y<=800 && sizeUndo>0)
                                {
                                    player_turn--;
                                    showCurrentPlayer();
                                    mins=0;sec=0;
                                    sizeUndo--;
                                        cout<<"greyminus "<<sizeUndo<<endl;
                                    for(i=1;i<=4;i++)
                                        for(j=1;j<=4;j++)
                                            bonolTable[i][j]=bonolTableUndo[i][j][sizeUndo];

                                    drawTable(coord_north,coord_west,bonolTable);
                                    drawTable(coord_north+600,coord_west,bonolTable);
                                    /*
                                    deck.sizeOfUndo--;
                                    cout<<" greyundo "<<deck.sizeOfUndo<<endl;
                                    player_turn--;

                                    mins=0;sec=0;

                                    //cout<<"grey";
                                    //cout<<player_turn<<endl;

                                        if(player_turn%4==1)
                                            player=1;
                                        else if(player_turn%4==2)
                                            player=7;
                                        else if(player_turn%4==3)
                                                player=2;
                                            else player=7;

                                 if(player==1 || player==2)
                                 {

                                     for(i=1;i<=4;i++)
                                        for(j=1;j<=4;j++)
                                            if(bonolTable[i][j]==player)
                                               {
                                                  // cout<<i<<' '<<j<<endl;
                                                   bonolTable[i][j]=WHITE;
                                                   //cout<<"alb"<<endl;
                                               }

                                            UndoProbableMove(deck.undoArray[deck.sizeOfUndo].row,deck.undoArray[deck.sizeOfUndo].column,deck.undoArray[deck.sizeOfUndo].rot);

                                            drawTable(coord_north,coord_west,bonolTable);
                                            drawTable(coord_north+600,coord_west,bonolTable);
                                 }

                                 showCurrentPlayer();

                                    */
                                }

                            else if(x>=500 && y>=600 && x<=700 && y<=800)
                                {
                                cout<<"back";
                                backToMenu=true;
                                player_turn=1;
                                 sec=0;
                                 mins=0;
                                 continue_pressed=false;
                                }
                                else if(x>=800 && y>=600 && x<=1000 && y<=800)
                                {
                                    player_turn=1;
                                    initTable();
                                    sec=0;
                                    mins=0;
                                    drawTable(coord_north,coord_west,bonolTable);
                                    drawTable(coord_north+600,coord_west,bonolTable);
                                }
                                else {PlaySound("wrong.wav",NULL,SND_ASYNC);delay(1000);PlaySound(music,NULL,SND_ASYNC);}

            else
            {
                row=getRow(y);
                column=getColumn(x);

                //deck.sizeOfUndo++;
                //cout<<"greyadd "<<deck.sizeOfUndo<<endl;
                //deck.undoArray[deck.sizeOfUndo].row=row;
                //deck.undoArray[deck.sizeOfUndo].column=column;

                //cout<<deck.undoArray[deck.sizeOfUndo].row<<' '<<deck.undoArray[deck.sizeOfUndo].column<<endl;


                {

                  if(bonolTable[row][column]==LIGHTGRAY && returnGreyNumber()==2)
                            {
                                 sizeUndo++;
                                for(i=1;i<=4;i++)
                                for(j=1;j<=4;j++)
                                    bonolTableUndo[i][j][sizeUndo]=bonolTable[i][j];


                                Beep(7000,100);
                                bonolTable[row][column]=-1;
                                drawAllXMark(GREEN);
                                clearAllNegatives();

                            }

                  else if((bonolTable[row][column]==WHITE ) && verifnonegatives==true)
                        {




                            Beep(700,100);
                            bonolTable[row][column]=LIGHTGRAY;
                            verifnonegatives=false;
                            drawTable(coord_north+600,coord_west,bonolTable);
                            drawAllXMark(WHITE);
                            drawTable(coord_north+600,coord_west,bonolTable);
                            player_turn++;
                            drawTable(coord_north,coord_west,bonolTable);

                        }
                }
            }

    }
}

int isAnyMovePossible(int i,int j,int playercolor)
{
    int sumOfRotations=0;

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j-1]==playercolor||bonolTable[i][j-1]==15||bonolTable[i][j-1]==-1)&&(bonolTable[i][j-2]==playercolor||bonolTable[i][j-2]==15||bonolTable[i][j-2]==-1)&&(bonolTable[i-1][j]==playercolor||bonolTable[i-1][j]==15||bonolTable[i-1][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j+1]==playercolor||bonolTable[i][j+1]==15||bonolTable[i][j+1]==-1)&&(bonolTable[i][j+2]==playercolor||bonolTable[i][j+2]==15||bonolTable[i][j+2]==-1)&&(bonolTable[i-1][j]==playercolor||bonolTable[i-1][j]==15||bonolTable[i-1][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j-1]==playercolor||bonolTable[i][j-1]==15||bonolTable[i][j-1]==-1)&&(bonolTable[i-1][j]==playercolor||bonolTable[i-1][j]==15||bonolTable[i-1][j]==-1)&&(bonolTable[i-2][j]==playercolor||bonolTable[i-2][j]==15||bonolTable[i-2][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i-1][j]==playercolor||bonolTable[i-1][j]==15||bonolTable[i-1][j]==-1)&&(bonolTable[i-2][j]==playercolor||bonolTable[i-2][j]==15||bonolTable[i-2][j]==-1)&&(bonolTable[i][j+1]==playercolor||bonolTable[i][j+1]==15||bonolTable[i][j+1]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j-1]==playercolor||bonolTable[i][j-1]==15||bonolTable[i][j-1]==-1)&&(bonolTable[i][j-2]==playercolor||bonolTable[i][j-2]==15||bonolTable[i][j-2]==-1)&&(bonolTable[i+1][j]==playercolor||bonolTable[i+1][j]==15||bonolTable[i+1][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j+1]==playercolor||bonolTable[i][j+1]==15||bonolTable[i][j+1]==-1)&&(bonolTable[i][j+2]==playercolor||bonolTable[i][j+2]==15||bonolTable[i][j+2]==-1)&&(bonolTable[i+1][j]==playercolor||bonolTable[i+1][j]==15||bonolTable[i+1][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j-1]==playercolor||bonolTable[i][j-1]==15||bonolTable[i][j-1]==-1)&&(bonolTable[i+1][j]==playercolor||bonolTable[i+1][j]==15||bonolTable[i+1][j]==-1)&&(bonolTable[i+2][j]==playercolor||bonolTable[i+2][j]==15||bonolTable[i+2][j]==-1))
                {sumOfRotations++;}

        if((bonolTable[i][j]==playercolor||bonolTable[i][j]==15||bonolTable[i][j]==-1)&&(bonolTable[i][j+1]==playercolor||bonolTable[i][j+1]==15||bonolTable[i][j+1]==-1)&&(bonolTable[i+1][j]==playercolor||bonolTable[i+1][j]==15||bonolTable[i+1][j]==-1)&&(bonolTable[i+2][j]==playercolor||bonolTable[i+2][j]==15||bonolTable[i+2][j]==-1))
                {sumOfRotations++;}

        return sumOfRotations;
}

void drawAllPossiblePlayerPlaces(int COLOR2)
{unsigned int i,j,player;

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==2)
            player=7;
         else if(player_turn%4==3)
                    player=2;
              else player=7;

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            if(isAnyMovePossible(i,j,player)!=0)
                {buildXMark(getCoordX(j)+600,getCoordY(i),COLOR2);}

}
bool ifPlayerLost()
{int sumOfAllMoves=0,player;

    if(player1_time_lost==true || player2_timer_lost==true)
                        return true;

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==2)
            player=7;
         else if(player_turn%4==3)
                    player=2;
              else player=7;

  if(player==1 || player==2)
    for(int i=1;i<=squareNumber;i++)
        {for(int j=1;j<=squareNumber;j++)
            {
               // if(player==1)
               //     cout<<" BLUE ";
               // else cout<<" GREEN ";

                sumOfAllMoves+=isAnyMovePossible(i,j,player);
              // cout<<isAnyMovePossible(i,j,player);
            }
            //cout<<endl;
            }

            //cout<<sumOfAllMoves<<endl;

    if(sumOfAllMoves==1)
      {
          player_turn++;
          return true;
      }
    else return false;
}
void emptyRotationVector()
{
    for(int i=1;i<=8;i++)
        rotation[i]=0;

}
void emptyBonolTableGrid()
{
    for(int i=1;i<=4;i++)
        for(int j=1;j<=4;j++)
            bonolTableGrid[i][j]=0;
}
//E. conditie pt piese gri sa nu faca asta
void copyPlayerPieceToGrid(int player)
{
    number_of_turns++;

     if(number_of_turns<=1)
        {
            for(int i=1;i<=squareNumber;i++)
                for(int j=1;j<=squareNumber;j++)
                    if(bonolTable[i][j]==player)
                        bonolTableGrid[i][j]=player;
        }

}

bool verifPlayerMoved()
{int sameposition=0;

    for(int i=1;i<=squareNumber;i++)
        for(int j=1;j<=squareNumber;j++)
            if(bonolTable[i][j]==bonolTableGrid[i][j])
                sameposition++;

                if(sameposition==4)
                    return false;
                else return true;
}

void createRotationVector(int row,int column)
{
    int i=row,j=column;

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))
                {rotation[1]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))
                {rotation[2]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15))
                {rotation[3]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15))
                {rotation[4]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
                {rotation[5]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
                {rotation[6]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
                {rotation[7]=1;}

        if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
                {rotation[8]=1;}

          //  for(i=1;i<=8;i++)
          //    cout<<rotation[i]<<' ';
          //  cout<<endl;

}
void ProbableMove(int x,int y)
{int q1,q2,player=1,verif_negative=0,i,j,nr=0;
 char cuvant_cifra[]="NR ROTATIE= ",sir1[10];

  j=getColumn(x);
  i=getRow(y);

  row_undo=i;
  column_undo=j;


  cout<<"coord"<<row_undo<<' '<<column_undo<<endl;


  //if(firstmovedone)
  {
    drawTable(coord_north+600,coord_west,bonolTable);
   // drawAllPossiblePlayerPlaces(MAGENTA);

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==2)
            player=7;
         else if(player_turn%4==3)
                    player=2;
              else player=7;

    for(q1=1;q1<=4;q1++)
        {for(q2=1;q2<=4;q2++)
        {
            nr+=isAnyMovePossible(q1,q2,player);
            cout<<isAnyMovePossible(i,j,player)<<' ';
        }
            cout<<endl;
        }

        itoa(nr,sir1,10);
                            if(nr>=10)
                                {readimagefile("counter_high.bmp",1200,300,1400,500);outtextxy(1300,500,sir1);nr=0;}
                                else if(nr<=10 &&nr>5)
                                        {readimagefile("counter_medium.bmp",1200,300,1400,500);outtextxy(1300,500,sir1);nr=0;}
                                        else if(nr<=5)
                                                {readimagefile("counter_low.bmp",1200,300,1400,500);outtextxy(1300,500,sir1);nr=0;}

    if(player==7)
        detectGreySquare();
    else
    {
      copyPlayerPieceToGrid(player);

        for(q1=1;q1<=4;q1++)
            for(q2=1;q2<=4;q2++)
        {
            if(bonolTable[q1][q2]==player)
                bonolTable[q1][q2]=-1;
        }

        if(isAnyMovePossible(i,j,player)!=0)
        {
            emptyRotationVector();
            createRotationVector(i,j);
        }

   do
    {

        if(rotation[index_rotation]==1)
            rotation_number=index_rotation;

        if(index_rotation>=8)
            index_rotation=0;

            index_rotation++;

    }
    while(index_rotation<=8 && rotation[index_rotation]==0);

    switch (rotation_number)
    {
    case 1:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))

    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i][j-2]=player;
        bonolTable[i-1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 2:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i][j+2]=player;
        bonolTable[i-1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 3:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i-1][j]=player;
        bonolTable[i-2][j]=player;
        bonolTable[i][j-1]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
   }
    break;
    case 4:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i-1][j]==-1||bonolTable[i-1][j]==15)&&(bonolTable[i-2][j]==-1||bonolTable[i-2][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i-1][j]=player;
        bonolTable[i-2][j]=player;
        bonolTable[i][j+1]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);

    }
    break;
    case 5:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i][j-2]==-1||bonolTable[i][j-2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i][j-2]=player;
        bonolTable[i+1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);

    }
    break;
    case 6:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i][j+2]==-1||bonolTable[i][j+2]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i][j+2]=player;
        bonolTable[i+1][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 7:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j-1]==-1||bonolTable[i][j-1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j-1]=player;
        bonolTable[i+1][j]=player;
        bonolTable[i+2][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    case 8:if((bonolTable[i][j]==-1||bonolTable[i][j]==15)&&(bonolTable[i][j+1]==-1||bonolTable[i][j+1]==15)&&(bonolTable[i+1][j]==-1||bonolTable[i+1][j]==15)&&(bonolTable[i+2][j]==-1||bonolTable[i+2][j]==15))
    {
        bonolTable[i][j]=player;
        bonolTable[i][j+1]=player;
        bonolTable[i+1][j]=player;
        bonolTable[i+2][j]=player;
        Beep(5000,100);
        drawTable(coord_north+600,coord_west,bonolTable);
    }
    break;
    }

        char cif[2];
        itoa(rotation_number%8+1,cif,10);
        strcat(cuvant_cifra,cif);

        if(player==1)
            {setcolor(player_color1);outtextxy(coord_north+450,coord_west,cuvant_cifra);}
            else if(player==2)
                    {setcolor(player_color2);outtextxy(coord_north+450,coord_west,cuvant_cifra);}
                else {setcolor(player_color3);outtextxy(coord_north+450,coord_west,"altceva");}

/*
        for(q1=1;q1<=4;q1++)
            for(q2=1;q2<=4;q2++)
                if(bonolTable[q1][q2]==-1)
                    {bonolTable[q1][q2]=WHITE;}
*/
            //drawAllPossiblePlayerPlaces(MAGENTA);

            drawTable(coord_north+600,coord_west,bonolTable);


    }
  }
  //else if(isAnyMovePossible(i,j,1))
           // firstmovedone=true;
}
void detectPlayerPiece()
{
 int row,column,x,y,player,i,j;
 char sir[10],sir2[10];


                if(player_turn%4==1)
                        player=1;
                    else if(player_turn%4==2)
                                player=7;
                            else if(player_turn%4==3)
                                    player=2;
                                else player=7;


    if(ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        x=mousex();
        y=mousey();

            if (!(x>=coord_west && x<=coord_west+width && y>=coord_north&&y<=coord_north+height))
                    if(x>=200 && y>=600 && x<=400 && y<=800 && sizeUndo>1 && player_turn-1>0)
                            {
                               //readimagefile("up.bmp",200,600,400,800);
                               //cout<<"undo";
                               //DE TESTA DUPA REFACERE UNDO

                                    player_turn--;

                                    showCurrentPlayer();

                                    sizeUndo--;

                                    for(i=1;i<=4;i++)
                                        for(j=1;j<=4;j++)
                                          bonolTable[i][j]=bonolTableUndo[i][j][sizeUndo];

                                        drawTable(coord_north,coord_west,bonolTable);
                                        drawTable(coord_north+600,coord_west,bonolTable);

                                    /*

                                    bonolTable[deck.undoArray[deck.sizeOfUndo].row][deck.undoArray[deck.sizeOfUndo].column]=WHITE;

                                    deck.sizeOfUndo--;

                                    bonolTable[deck.undoArray[deck.sizeOfUndo].row][deck.undoArray[deck.sizeOfUndo].column]=LIGHTGRAY;

                                    drawTable(coord_north,coord_west,bonolTable);
                                    drawTable(coord_north+600,coord_west,bonolTable);
                                    */


                            }
                    else if(x>=500 && y>=600 && x<=700 && y<=800)
                            {
                                cout<<"back";
                                backToMenu=true;
                                player_turn=1;
                                mins=0;
                                sec=0;
                                  continue_pressed=false;
                            }
                         else if(x>=800 && y>=600 && x<=1000 && y<=800)
                                {
                                    player_turn=1;
                                    initTable();
                                    drawTable(coord_north,coord_west,bonolTable);
                                    drawTable(coord_north+600,coord_west,bonolTable);
                                    mins=0;
                                    sec=0;
                                   // emptyBonolTableGrid();
                                }
                                else
                                    {PlaySound("wrong.wav",NULL,SND_ASYNC);delay(1000);PlaySound(music,NULL,SND_ASYNC);}

            else
            {


                row=getRow(y);
                column=getColumn(x);

                if(player_turn%4==1)
                        player=1;
                    else if(player_turn%4==2)
                                player=7;
                            else if(player_turn%4==3)
                                    player=2;
                                else player=7;

                if(isAnyMovePossible(row,column,player)>=1)
                  {
                            ProbableMove(x,y);
                        if(detectMissingPlayer()==true)
                            ProbableMove(x,y);
                  }
                else if(isAnyMovePossible(row,column,player)==0)
                        {PlaySound("wrong.wav",NULL,SND_ASYNC);delay(1000);PlaySound(music,NULL,SND_ASYNC);}

            }

    }



        if(mocanu_sec == 250)
        {
                ++sec;
                mocanu_sec = 0;
        }
        if(sec==60)
        {
            ++mins;
            sec=0;
        }

        itoa(mins,sir,10);
        itoa(sec,sir2,10);
        if(sec<10)
            strcat(sir," :0");
        else strcat(sir," :");
        strcat(sir,sir2);

        setbkcolor(BLACK);
         if(mins==maxMins && sec>=maxSec-15)
                setcolor(LIGHTRED);
        else setcolor(LIGHTCYAN);
        outtextxy(1280,200,sir);
        //cout<<sir<<endl;
        mocanu_sec++;
        setbkcolor(MyBLUE);

        if(sec>maxSec && mins>=maxMins && (player%4==1 || player%4==3) )
           {
               player1_time_lost=true;
           }
        else if(sec>maxSec && mins>=maxMins && (player%4==2 || player%4==0) )
                {
                    player2_timer_lost=true;
                }


}

void nextTurn()
{int player,i,j;

    if(player_turn%4==1)
        player=1;
    else if(player_turn%4==2)
            player=7;
         else if(player_turn%4==3)
                    player=2;
              else player=7;

    if(ismouseclick(WM_RBUTTONDOWN))
    {
        clearmouseclick(WM_RBUTTONDOWN);

        if(verifPlayerMoved()==true && number_of_turns!=0)
            {
                player_turn++;
                drawTable(coord_north,coord_west,bonolTable);
                Beep(1000,100);
                emptyBonolTableGrid();
                number_of_turns=0;

                    mins=0;
                    sec=0;

                    //deck.sizeOfUndo++;

                    if(player==1 || player==2)
                    {
                           //deck.undoArray[deck.sizeOfUndo].row=row_undo;
                          // deck.undoArray[deck.sizeOfUndo].column=column_undo;
                          // deck.undoArray[deck.sizeOfUndo].rot=rotation_number;
                          // cout<<"nextturn"<<rotation_number;
                          sizeUndo++;
                           for(i=1;i<=4;i++)
                                for(j=1;j<=4;j++)
                                    bonolTableUndo[i][j][sizeUndo]=bonolTable[i][j];

                            cout<<"NextTurn"<<sizeUndo<<endl;


                            for(i=1;i<=4;i++)
                                {for(j=1;j<=4;j++)
                                    continue_file<<bonolTable[i][j]<<' ';
                                    cout<<endl;
                                }

                                //continueGame();
                    }

            }


    }

        showCurrentPlayer();
}
int matrixSeparatedSpaces[5][5];

int squareNeighbours(int row,int column)
{

    if(matrixSeparatedSpaces[row][column-1]!=WHITE && matrixSeparatedSpaces[row][column+1]!=WHITE && matrixSeparatedSpaces[row+1][column]!=WHITE && matrixSeparatedSpaces[row][column]!=WHITE)
                                                return 1;

    else if(matrixSeparatedSpaces[row+1][column]==WHITE)
            {matrixSeparatedSpaces[row][column]=-1;return 1+squareNeighbours(row+1,column);}

        else if(matrixSeparatedSpaces[row][column+1]==WHITE)
                {matrixSeparatedSpaces[row][column]=-1;return 1+squareNeighbours(row,column+1);}

             else if(matrixSeparatedSpaces[row-1][column]==WHITE)
                    {matrixSeparatedSpaces[row][column]=-1;return 1+squareNeighbours(row-1,column);}

                  else if(matrixSeparatedSpaces[row][column-1]==WHITE)
                        {matrixSeparatedSpaces[row][column]=-1;return 1+squareNeighbours(row,column-1);}
   // return 0;
}
void AINormal(int playercolor)
{//tot timpul sunt 6 patrate albe

    int i,j,separatedSpaces=0,numberOfWhites=6,maxSeparatedSpaces=0,x_max=0,y_max=0,x,y,nrmax=0,imax,jmax,q,w,nridentice=0;

    for(i=1;i<=4;i++)
        for(j=1;j<=4;j++)
            if(bonolTable[i][j]==GREEN)
                bonolTableGrid[i][j]=bonolTable[i][j];

    if(playercolor==GREEN)
    {
        for(i=1;i<=4;i++)
            for(j=1;j<=4;j++)
                {if((bonolTable[i][j]==WHITE || bonolTable[i][j]==playercolor || bonolTable[i][j]==-1) && isAnyMovePossible(i,j,playercolor)>=1 && matrixSeparatedSpaces[i][j]!=-1)
                    {

                        x=getCoordX(j);
                        y=getCoordY(i);

                        ProbableMove(x,y);

                        for(q=1;q<=squareNumber;q++)
                            for(w=1;w<=squareNumber;w++)
                                matrixSeparatedSpaces[q][w]=bonolTable[q][w];

                                 for(i=1;i<=4;i++)
                                    {for(j=1;j<=4;j++)
                                        cout<<squareNeighbours(i,j)<<' ';
                                        cout<<endl;
                                   }

                                 for(q=1;q<=squareNumber;q++)
                                    for(w=1;w<=squareNumber;w++)
                                        if(numberOfWhites>0)
                                        {
                                            cout<<squareNeighbours(i,j)<<"=neighbours "<<numberOfWhites<<"=Whites"<<separatedSpaces<<"=separt"<<endl;

                                            if(squareNeighbours(q,w)!=0)
                                                {numberOfWhites-=squareNeighbours(q,w); separatedSpaces++;}


                                        }

                                    if(maxSeparatedSpaces<=separatedSpaces)
                                        {
                                            maxSeparatedSpaces=separatedSpaces;
                                            x_max=x;y_max=y;
                                            //cout<<x_max<<' '<<y_max<<endl;
                                            cout<<"separ"<<maxSeparatedSpaces<<"New"<<endl;

                                        }


                    }

                }




        ProbableMove(x_max,y_max);

        for(i=1;i<=4;i++)
            for(j=1;j<=4;j++)
                if(bonolTableGrid[i][j]==GREEN)
                        nridentice++;


                        cout<<"nridentice"<<nridentice;

            if(nridentice==4)
                {ProbableMove(x_max,y_max);ProbableMove(x_max,y_max);}

        player_turn++;
        nextTurn();


        for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==LIGHTGRAY)
                  {
                        bonolTable[i][j]=WHITE;
                         i=5;j=5;
                  }


            }

        drawTable(coord_north,coord_north,bonolTable);
        drawTable(coord_north+600,coord_north,bonolTable);

         for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==WHITE && nrmax==2)
                    {bonolTable[i][j]=LIGHTGRAY;i=5;j=5;}
                else if(bonolTable[i][j]==WHITE)
                            nrmax++;

            }

        drawTable(coord_north,coord_west,bonolTable);
        drawTable(coord_north+600,coord_west,bonolTable);

        player_turn++;
        nextTurn();

    }
    emptyBonolTableGrid();
    separatedSpaces=0;
    }

void AI(int playercolor)
{//tot timpul sunt 6 patrate albe

    int i,j,separatedSpaces=0,numberOfWhites=6,maxSeparatedSpaces=0,x_max=0,y_max=0,x,y,nrmax=0,imax,jmax,q,w;


    if(playercolor==GREEN)
    {
         for(q=1;q<=squareNumber;q++)
            for(w=1;w<=squareNumber;w++)
                matrixSeparatedSpaces[q][w]=bonolTable[q][w];

        for(i=1;i<=4;i++)
            for(j=1;j<=4;j++)
                {if((bonolTable[i][j]==WHITE || bonolTable[i][j]==playercolor || bonolTable[i][j]==-1) && isAnyMovePossible(i,j,playercolor)>=1 && matrixSeparatedSpaces[i][j]!=-1)
                    {
                        x=getCoordX(j);
                        y=getCoordY(i);

                        ProbableMove(x,y);


                                 for(i=1;i<=4;i++)
                                    {for(j=1;j<=4;j++)
                                        if(matrixSeparatedSpaces[i][j]==WHITE)
                                           cout<<squareNeighbours(i,j)<<' ';
                                    else cout<<"-1 ";
                                        cout<<endl;
                                    }


                                while(numberOfWhites>0)
                                    {
                                       // cout<<squareNeighbours(i,j)<<"=neighbours "<<numberOfWhites<<"=Whites"<<separatedSpaces<<"=separt"<<endl;

                                        if(squareNeighbours(i,j)>0)
                                            numberOfWhites-=squareNeighbours(i,j);
                                        else if(i<4)
                                                i++;
                                             else if(j<4)
                                                    j++;
                                                    else numberOfWhites=0;
                                        separatedSpaces++;
                                    }

                                    if(maxSeparatedSpaces<=separatedSpaces)
                                        {
                                            maxSeparatedSpaces=separatedSpaces;
                                            x_max=x;y_max=y;
                                            imax=i;
                                            jmax=j;
                                            //cout<<x_max<<' '<<y_max<<endl;
                                            cout<<maxSeparatedSpaces<<"New"<<endl;

                                        }


                    }

                }




        ProbableMove(x_max,y_max);
        //ProbableMove(x_max,y_max);

        player_turn++;
        nextTurn();




        for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==LIGHTGRAY)
                    {bonolTable[i][j]=WHITE;i=5;j=5;}

            }
        drawTable(coord_north,coord_west,bonolTable);

        cout<<imax<<' '<<jmax;

        if(bonolTable[imax+1][jmax-1]==WHITE)
            bonolTable[imax+1][jmax-1]=LIGHTGRAY;
        else
        if(bonolTable[imax+1][jmax]==WHITE)
            bonolTable[imax+1][jmax]=LIGHTGRAY;
        else
         if(bonolTable[imax+1][jmax+1]==WHITE)
            bonolTable[imax+1][jmax+1]=LIGHTGRAY;
        else
        if(bonolTable[imax][jmax-1]==WHITE)
            bonolTable[imax][jmax-1]=LIGHTGRAY;
        else
        if(bonolTable[imax][jmax+1]==WHITE)
            bonolTable[imax][jmax+1]=LIGHTGRAY;
        else
            if(bonolTable[imax-1][jmax-1]==WHITE)
            bonolTable[imax-1][jmax-1]=LIGHTGRAY;
        else
         if(bonolTable[imax-1][jmax]==WHITE)
            bonolTable[imax-1][jmax]=LIGHTGRAY;
        else
         if(bonolTable[imax-1][jmax+1]==WHITE)
            bonolTable[imax-1][jmax+1]=LIGHTGRAY;
        else
         for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==WHITE)
                   {
                       bonolTable[i][j]=LIGHTGRAY;
                        i=5;j=5;
                   }
            }



        drawTable(coord_north+600,coord_west,bonolTable);

        player_turn++;
        nextTurn();

    }
    /*
    else if(playercolor==BLUE)
         {
            for(q=1;q<=squareNumber;q++)
            for(w=1;w<=squareNumber;w++)
                matrixSeparatedSpaces[q][w]=bonolTable[q][w];

        for(i=1;i<=4;i++)
            for(j=1;j<=4;j++)
                {if((bonolTable[i][j]==WHITE || bonolTable[i][j]==playercolor || bonolTable[i][j]==-1) && isAnyMovePossible(i,j,playercolor)>=1 && matrixSeparatedSpaces[i][j]!=-1)
                    {
                        x=getCoordX(j);
                        y=getCoordY(i);

                        ProbableMove(x,y);


                                // for(i=1;i<=4;i++)
                                //    {for(j=1;j<=4;j++)
                                //        if(matrixSeparatedSpaces[i][j]==WHITE)
                                //           cout<<squareNeighbours(i,j)<<' ';
                                    //else cout<<"-1 ";
                                //        cout<<endl;
                                 //   }


                                while(numberOfWhites>0)
                                    {
                                       // cout<<squareNeighbours(i,j)<<"=neighbours "<<numberOfWhites<<"=Whites"<<separatedSpaces<<"=separt"<<endl;

                                        if(squareNeighbours(i,j)>0)
                                            numberOfWhites-=squareNeighbours(i,j);
                                        else if(i<4)
                                                i++;
                                             else if(j<4)
                                                    j++;
                                                    else numberOfWhites=0;
                                        separatedSpaces++;
                                    }

                                    if(maxSeparatedSpaces<=separatedSpaces)
                                        {
                                            maxSeparatedSpaces=separatedSpaces;
                                            x_max=x;y_max=y;
                                            imax=i;
                                            jmax=j;
                                            //cout<<x_max<<' '<<y_max<<endl;
                                            cout<<maxSeparatedSpaces<<"New"<<endl;

                                        }


                    }

                }




        ProbableMove(x_max,y_max);
        //ProbableMove(x_max,y_max);

        player_turn++;
        nextTurn();




        for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==LIGHTGRAY)
                    {bonolTable[i][j]=WHITE;i=5;j=5;}

            }
        drawTable(coord_north,coord_west,bonolTable);

        cout<<imax<<' '<<jmax;

        if(bonolTable[imax+1][jmax-1]==WHITE)
            bonolTable[imax+1][jmax-1]=LIGHTGRAY;
        else
        if(bonolTable[imax+1][jmax]==WHITE)
            bonolTable[imax+1][jmax]=LIGHTGRAY;
        else
         if(bonolTable[imax+1][jmax+1]==WHITE)
            bonolTable[imax+1][jmax+1]=LIGHTGRAY;
        else
        if(bonolTable[imax][jmax-1]==WHITE)
            bonolTable[imax][jmax-1]=LIGHTGRAY;
        else
        if(bonolTable[imax][jmax+1]==WHITE)
            bonolTable[imax][jmax+1]=LIGHTGRAY;
        else
            if(bonolTable[imax-1][jmax-1]==WHITE)
            bonolTable[imax-1][jmax-1]=LIGHTGRAY;
        else
         if(bonolTable[imax-1][jmax]==WHITE)
            bonolTable[imax-1][jmax]=LIGHTGRAY;
        else
         if(bonolTable[imax-1][jmax+1]==WHITE)
            bonolTable[imax-1][jmax+1]=LIGHTGRAY;
        else
         for(i=1;i<=squareNumber;i++)
            for(j=1;j<=squareNumber;j++)
            {
                if(bonolTable[i][j]==WHITE)
                   {
                       bonolTable[i][j]=LIGHTGRAY;
                        i=5;j=5;
                   }
            }



        drawTable(coord_north+600,coord_west,bonolTable);

        player_turn++;
        nextTurn();
         }
         */
    emptyBonolTableGrid();
    separatedSpaces=0;
}


void initContinueGame()
{int i,j;

    ifstream continue_file1("continue.in");

    for(i=1;i<=squareNumber;i++)
        for(j=1;j<=squareNumber;j++)
            continue_file1>>bonolTable[i][j];

    continue_file1.close();
}

int main()
{
    int window_x=0,window_y=0,player;
    int sec = 0;
    int mins = 0;
    char sir[10],sir2[10],buton[25];


    initwindow(1600,1000,"Bonol Game",window_x,window_y);

         drawMainMenu();

         do
        {
            int tasta=0;
            do
            {
                 detectButtonLocation(tasta);
            }
            while(tasta==0);



                switch(tasta)
             {
                case 13:{inithighscore();setbkcolor(MyBLUE);cleardevice();PlaySound(music,NULL,SND_ASYNC);clearmouseclick(WM_LBUTTONDOWN);

                            if(continue_pressed==true)
                                initContinueGame();
                            else initTable();

                            makeScreenColor();drawTable(coord_north,coord_west,bonolTable);drawTable(coord_north+600,coord_west,bonolTable);done=false;

                            setcolor(DARKGRAY);
                            outtextxy(250,50,"PLAYING AREA");
                            outtextxy(875,50,"PREVIEW");
                            showCurrentPlayer();
                            strcpy(buton,"undo.bmp");
                                if(language==1)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_rom.bmp");
                                }
                                else if(language==2)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_fre.bmp");
                                }
                            readimagefile(buton,200,600,400,800);
                            strcpy(buton,"back2.bmp");
                                if(language==1)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_rom.bmp");
                                }
                                else if(language==2)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_fre.bmp");
                                }
                            readimagefile(buton,500,600,700,800);
                            strcpy(buton,"reset.bmp");
                                if(language==1)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_rom.bmp");
                                }
                                else if(language==2)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_fre.bmp");
                                }
                            readimagefile(buton,800,600,1000,800);


                            readimagefile("time.bmp",1200,100,1400,300);

                            backToMenu=false;

                                        do
                                    {
                                        if(player_turn%4==1)
                                                player=1;
                                        else if(player_turn%4==2)
                                                    player=7;
                                             else if(player_turn%4==3)
                                                        player=2;
                                                  else player=7;

                                        if(detectMissingPlayer()==false && rotation_number!=0 && player==7)
                                                         detectGreySquare();

                                        detectPlayerPiece();

                                        if(detectMissingPlayer()==false && rotation_number!=0 && returnGreyNumber()==2)
                                                            {nextTurn();}
                                        else clearmouseclick(WM_RBUTTONDOWN);

                                    }
                                        while(!ifPlayerLost() && !backToMenu);

                        }
                        case 99:{

                                 if(backToMenu==false)
                                 {  continue_pressed=false;
                                      done=false;
                                    player_turn++;

                                    if(player_turn%4==1)
                                                player=1;
                                        else if(player_turn%4==2)
                                                    player=7;
                                             else if(player_turn%4==3)
                                                        player=2;
                                                  else player=7;

                                    setbkcolor(BLACK);
                                    cleardevice();

                                    drawMainMenu();

                                    //if(menu==1)
                                    //clas();

                                    if(player==1)
                                  {readimagefile("won1.bmp",500,0,900,350);won1++;}
                                    else if(player==2)
                                        {readimagefile("won2.bmp",500,0,900,350);won2++;}
                                         else if(player1_time_lost==true)
                                                   {readimagefile("won2.bmp",500,0,900,350);player1_time_lost=false;won1++;}
                                                        else if(player2_timer_lost==true)
                                                                  {readimagefile("won1.bmp",500,0,900,350);player2_timer_lost=false;won2++;}

                                cout<<"player "<<player<<" HAS WON!"<<endl;

                                readimagefile("try.bmp",650,250,750,350);
                                //drawclas(won1,won2);
                                 }
                                 else {
                                        setbkcolor(BLACK);cleardevice();drawMainMenu();backToMenu=false;
                                      }




                        }
                    break;
                case 98:{PlaySound(music,NULL,SND_ASYNC);clearmouseclick(WM_LBUTTONDOWN);initTable();initUndoTable();makeScreenColor();drawTable(coord_north,coord_west,bonolTable);drawTable(coord_north+600,coord_west,bonolTable);done=false;

                            setcolor(DARKGRAY);
                            outtextxy(250,50,"PLAYING AREA");
                            outtextxy(875,50,"PREVIEW");
                            showCurrentPlayer();
                            readimagefile("back2.bmp",500,600,700,800);
                            readimagefile("time.bmp",1200,100,1400,300);

                            strcpy(buton,"back2.bmp");
                                if(language==1)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_rom.bmp");
                                }
                                else if(language==2)
                                {
                                    buton[strlen(buton)-4]=0;
                                    strcat(buton,"_fre.bmp");
                                }
                            readimagefile(buton,500,600,700,800);

                                        do
                                    {
                                        if(player_turn%4==1)
                                                player=1;
                                        else if(player_turn%4==2)
                                                    player=7;
                                             else if(player_turn%4==3)
                                                        player=2;
                                                  else player=7;

                                        if(detectMissingPlayer()==false && rotation_number!=0 && player==7)
                                                         detectGreySquare();
                                        else clearmouseclick(WM_MBUTTONDOWN);

                                        detectPlayerPiece();

                                        if(detectMissingPlayer()==false && rotation_number!=0 && returnGreyNumber()==2)
                                                            {nextTurn();}
                                        else clearmouseclick(WM_RBUTTONDOWN);

                                        AINormal(player);
                                            //eazymode(player);
                                            //AIeazy(player);

                                    }
                                    while(!ifPlayerLost() && !backToMenu);
                        }

                case 97:{
                         if(backToMenu==false)
                         {
                           done=false;
                           player_turn++;
                                    if(player_turn%4==1)
                                                player=1;
                                        else if(player_turn%4==2)
                                                    player=7;
                                             else if(player_turn%4==3)
                                                        player=2;
                                                  else player=7;

                                setbkcolor(player);
                                cleardevice();

                                if(player==1)
                                    {readimagefile("won1.bmp",500,0,900,350);PlaySound("saxguy.wav",NULL,SND_ASYNC);}
                                else if(player==2)
                                        {readimagefile("won2.bmp",500,0,900,350);PlaySound("darkness.wav",NULL,SND_ASYNC);}


                                cout<<"player "<<player<<" HAS WON!"<<endl;
                         }
                         else {
                                setbkcolor(BLACK);cleardevice();drawMainMenu();backToMenu=false;
                              }


                        }
                break;
                case 27:{
                            done=true;
                        }


             }
        }
        while(!done);



    return 0;

}
