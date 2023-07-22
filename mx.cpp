//Texturen!!!
#include <ctype.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <mem.h> //fr memset temp
#include <time.h>
#include <fstream.h>
#include <iostream.h>
// #include <iostream.h>
#include <stdlib.h>


//
int MXframe=0;
int MXE1=0; //Effekt Palette
//mouse
int MouseX=0;
int MouseY=0;
int MouseB=0;
//Mouse


//VGA>>>>>
typedef unsigned char byte;
byte far *MXVGA =(byte far*)0xA0000000L;//VGA start Adresse
//<<<<<

// Tables in external files
extern unsigned char ZS[909];
extern unsigned char MXsprite[10][256];
extern unsigned char Map[2048];

//###########################################################################
// DC-Plattform / Globals
//###########################################################################

//Scrollen -1
int ODCpx=0;
int ODCpy=0;
//Scrollen -2
int ODCpx2=0;
int ODCpy2=0;

float ODpx=0;
float ODpy=0;

//Map Size festlegen >>>>>
int ODCmx=32;
int ODCmy=64;


//############################################################################
//Grafik  Mode-X 320x200x8 Funktionen
//   *                    *             *             *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################



//########################### LATCH Copy Register setzen #####################
void MXlatch(int MXs)
{

// 1=Copy Modus / 0=Modus "Pset" >>>>>
    if (MXs==1)

    {    outp(0x3c4,0x02);//select plane mask register
        outp(0x3c5,0x0f);//4 Plane select
        outp(0x3ce,0x08);//select CPU bit masking function
        outp(0x3cf,0x00);//unmask all bits
        MXs=MXVGA[0];    //1x CPU lesen

    }
    else
    {
        outp(0x3c4,0x02);//select plane mask register
        outp(0x3c5,0x0f);//4 Plane select
        outp(0x3ce,0x08);//select CPU bit masking function
        outp(0x3cf,0xff);//unmask all bits
        MXs=MXVGA[0];    //latch fllen
    }
}
//<<<<<


//########################### Latch Copy #####################################
void MXcopy(int MXsx,int MXsy,int MXx,int MXy,int MXb,int MXh)
{
    unsigned int MXpx;
    unsigned int MXpy;

    unsigned int MXspx;
    unsigned int MXspy;
    MXspx=MXsx;
    MXspy=MXsy;

    unsigned char MXp;
    MXpx=MXx;
    MXpy=MXy;

    MXb=MXx+MXb;
    MXh=MXy+MXh;

    do
    {
        MXp=MXVGA[((MXspx>>2)+(MXspy<<6)+(MXspy<<4))];    //CPU pseudo lesen
        MXVGA[((MXpx>>2)+(MXpy<<6)+(MXpy<<4))]=0;    //VRAM schreiben

        MXpx++;
        MXspx++;

        if (MXpx>=MXb)    //Zeilenwechsel "X"=0
        {
            MXpx=MXx;
            MXpy++;
            MXspx=MXsx;
            MXspy++;
        }
    }while(MXpy<MXh);
    MXp++; // Compile zufrieden stellen
}
//<<<<<


//############################ MXini() #######################################
void MXini()
{
    asm {
            mov    ax, 13h
            int    10h         //Modus 13

            mov    dx, 3c4h    //Port 3c4h / Sequencer Address Register
            mov    al, 4       //Index 4 / select memory mode
            out    dx, al
            inc    dx          //Port 3c5h Memory mode set
            in     al, dx
            and    al, not 08h
            or     al, 04h
            out    dx, al
            mov    dx, 3ceh
            mov    al, 5
            out    dx, al
            inc    dx
            in     al, dx
            and    al, not 10h
            out    dx, al
            dec    dx
            mov    al, 6
            out    dx, al
            inc    dx
            in     al, dx
            and    al, not 02h
            out    dx, al
            mov    dx, 3c4h
            mov    ax, (0fh shl 8) + 2
            out    dx, ax
            mov    ax, 0a000h
            mov    es, ax
            sub    di, di
            mov    ax, 0000h //{8080h}
            mov    cx, 32768
            cld
            rep    stosw            //CLS0

            mov    dx, 3d4h
            mov    al, 14h
            out    dx, al
            inc    dx
            in     al, dx
            and    al, not 40h
            out    dx, al
            dec    dx
            mov    al, 17h
            out    dx, al
            inc    dx
            in     al, dx
            or     al, 40h
            out    dx, al

            mov    dx, 3d4h
            mov    al, 13h
            out    dx, al
            inc    dx
            mov    al, 40  //[Size]      //horizontal wide 320 / 8 = 40
            out    dx, al
            }
}
//<<<<<










//############################ MXoff() #######################################
void MXoff()
{
    asm {
	    mov    ax, 03h
	    int    10h         //Modus 13
   }
}
//<<<<<


//############################ MXpaletteS(x,y,Color,"Char-Array") #############################
//mit hintergrundfarbe!
void MXpaletteS(int MXpnr,unsigned char MXpr,unsigned char MXpg,unsigned char MXpb)
{
    asm  {
        mov    ah,10h
            mov    al,10h
            mov    bx,MXpnr     //Nummer DAC-Farbregister
            mov ch,MXpg    //grn
            mov cl,MXpb    //blau
            mov dh,MXpr    //rot
            int    10h
            }
}
//<<<<<










//############################ Palette setzen 216 Farben +51/+51/+51 #########
void MXpalette(void)
{

    unsigned int MXr=0;
    unsigned int MXg=0;
    unsigned int MXb=0;
    unsigned int MXc=16;
    do
    {

        MXpaletteS(MXc,MXr,MXg,MXb);//Wert / Rot / Grn / Blau
        MXc++;

        MXr=MXr+51;

        if (MXr>255) {
            MXr=0;
            MXg=MXg+51;
        }

        if (MXg>255) {
            MXr=0;
            MXg=0;
            MXb=MXb+51;
        }

    }while(MXc<232);
}

//<<<<<




//XXX RAM Buffer alt XXX
//############################ MXbuffer() ####################################
int MXbuffer()
{
//RAM Buffer
    unsigned char MXbuffer2[64000];//mp

    unsigned char *MXbuffer;
    typedef unsigned char byte;
    byte far *VGA =(byte far*)0xA0000000L;//VGA start Adresse

    if ((MXbuffer = (unsigned char *) malloc(320*100))==NULL) return(0);

    int MXx=30;
    int MXy=0;
    unsigned char MXc=14;
//MXbuffer[(MXy<<8) + (MXy<<6) + (MXx)] = MXc; //Pixel setzen in buffer
//MXbuffer[(MXy<<8) + (MXy<<6) + (MXx)] = MXc; //Pixel setzen in buffer

    MXbuffer2[(MXy<<8) + (MXy<<6) + (MXx)] = MXc; //Pixel setzen in buffer
    MXbuffer2[0] = 2; //Pixel setzen in buffer
    MXbuffer2[80] = 2; //Pixel setzen in buffer
    MXbuffer2[160] = 2; //Pixel setzen in buffer
    MXbuffer2[240] = 2; //Pixel setzen in buffer
    MXbuffer2[320] = 2; //Pixel setzen in buffer
    MXbuffer2[5] = 2; //Pixel setzen in buffer

/*
//0x13 Mode all Planes
outpw(0x3c4,0xff02);//Plane Setzen,Plane 0xff02=4 Planes
memcpy(VGA,MXbuffer,64000); //16000 = 320*200 / 4
*/

//unchained Mode 4 planes used!!!
    MXbuffer2[0] = 2; //Pixel setzen in buffer
    outpw(0x3c4,0xf102);//Plane 1
    memcpy(VGA,MXbuffer2,16000); //16000 = 320*200 / 4
    MXbuffer2[0] = 12; //Pixel setzen in buffer
    outpw(0x3c4,0xf202);//Plane 1
    memcpy(VGA,MXbuffer2,16000); //16000 = 320*200 / 4
    MXbuffer2[0] = 3; //Pixel setzen in buffer
    outpw(0x3c4,0xf402);//Plane 1
    memcpy(VGA,MXbuffer2,16000); //16000 = 320*200 / 4
    MXbuffer2[0] = 4; //Pixel setzen in buffer
    outpw(0x3c4,0xf802);//Plane 1
    memcpy(VGA,MXbuffer2,16000); //16000 = 320*200 / 4

}
//<<<<<


//############################ MXpset(x,y,Color) #############################
void MXpset(int MXx , int MXy,char MXc){

    asm  {
        mov    ax,MXy   //[y] Position
            xor    bx,bx
            mov    bl,40 //[size] Breite in Pixel / 8
            imul   bx
            shl    ax,1
            mov    bx,ax
            mov    ax, MXx //[X Position
            mov    cx, ax
            shr    ax, 2
            add    bx, ax
            and    cx, 00000011b
            mov    ah, 1
            shl    ah, cl
            mov    dx, 3c4h                  //{ Sequencer Register    }
            mov    al, 2                     //{ Map Mask Index        }
            out    dx, ax

            mov    ax, 0a000h
            mov    es, ax
            mov    al, MXc //[col] Farbe
            mov    es: [bx], al
            }
}
//<<<<<


//############################ MXmove(Scroll pixel) ##########################
void MXflip(unsigned int MXm)
{
    if (MXframe==0) MXm=0;
    if (MXframe==200) MXm=16000;
    asm  {
        mov    bx, MXm //Block +/-
            mov    ah, bh
            mov    al, 0ch

            mov    dx, 3d4h
            out    dx, ax

            mov    ah, bl
            mov    al, 0dh
            mov    dx, 3d4h
            out    dx, ax
            }

    if (MXframe==0){
        MXframe=200;
    }
    else {
        MXframe=0;
    }
}
//<<<<<


//############################ MXmove(Scroll pixel) ##########################
void MXmove(int MXm)
{
    asm  {
        mov    bx, MXm //Block +/-
            mov    ah, bh
            mov    al, 0ch

            mov    dx, 3d4h
            out    dx, ax

            mov    ah, bl
            mov    al, 0dh
            mov    dx, 3d4h
            out    dx, ax
            }
}
//<<<<<


//############################ CLS(Color) Clear-Screen  ######################
void MXcls(unsigned char MXc)
{
//!!Latch(0)!!
    memset(MXVGA,MXc,16000);//Adresse,Farbe,Range
}

//<<<<<


//############################################################################
//DC-Plattform Grafikbereich   *                 *        *            *
//   *                    *             *             *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################



//############################ DC-Plattform Kachel Bodentextur ######################
void DCtexB    (int DCx,int DCy,int DCc)
{
    unsigned char MXc=0;
    int MXs=0;
    unsigned char temp=0;
    unsigned char temp2=0;


//Bankwechsel >>>>>
    asm  {
            mov    ax, DCx //[X Position
            mov    cx, ax
            shr    ax, 2
            add    bx, ax
            and    cx, 00000011b
            mov    ah, 1
            shl    ah, cl

            mov    dx, 3c4h                  //{ Sequencer Register    }
            mov    al, 2                     //{ Map Mask Index        }
            out    dx, ax
            }
//<<<<<


    do
    {
        do
        {
//PSET >>>>>
            MXc=MXsprite[DCc-1][MXs];//DCc=Textur MXs Punkt

            asm  {
                    mov    ax,DCy   //[y] Position
                    xor    bx,bx
                    mov    bl,40 //[size] Breite in Pixel / 8
                    imul   bx
                    shl    ax,1
                    mov    bx,ax
                    mov    ax, DCx //[X Position
                    mov    cx, ax
                    shr    ax, 2
                    add    bx, ax
                    and    cx, 00000011b
                    mov    ah, 1
                    shl    ah, cl
                    //ohne Bankwechsel
                    //     mov    dx, 3c4h   //Sequencer Register
                    //     mov    al, 2      //Map Mask Index
                    //     out    dx, ax
                    mov    ax, 0a000h
                    mov    es, ax
                    mov    al, MXc //[col] Farbe
                    mov    es: [bx], al
            }

            MXs++;
            DCy++;

            temp++;

        } while(temp<16);

        temp=0;
        temp2=temp2+1;
        DCy=DCy-16;
        DCx=DCx+1;

//Bankwechsel >>>>>
        asm  {
            mov    ax, DCx //[X Position
                mov    cx, ax
                shr    ax, 2
                add    bx, ax
                and    cx, 00000011b
                mov    ah, 1
                shl    ah, cl
                mov    dx, 3c4h                  //{ Sequencer Register    }
                mov    al, 2                     //{ Map Mask Index        }
                out    dx, ax
                }
//<<<<<

    } while(temp2<16);

}
//<<<<<


//############################ DC-Plattform Kachel einfarbig #################
void DCtexCls(int DCx,int DCy,int DCc)
{

    int MXs=0;
//int DCc=1;
    unsigned char temp=0;
    unsigned char temp2=0;

//DCx=-16+x*16+y*16;
//DCy=100+y*8-x*8;

    do
    {
        do
        {
            MXpset(DCx,DCy,DCc);

            MXs++;
            DCx++;

            temp++;

        } while(temp<16);

        temp=0;
        temp2=temp2+1;
        DCx=DCx-16;
        DCy=DCy+1;
    } while(temp2<16);

}
//<<<<<


//############################################################################
//DC-Plattform Grafikbereich Hintergrund berechnen darstellen aktualisieren   *
//    *                   *             *             *               *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################


//!!Hauptfunktion!!
void DC(int DCpx ,int DCpy)
{
    int x;
    int y;
    int DCx;
    int DCy;
    int DCc;
    int ODCc;
    x=-9;
    y=-6;
    do
    {
//X/Y in KachelPosition umrechnen!
        DCx=160-8+x*16;
        DCy=104+y*16;


        DCc=Map[(x+DCpx)+(y+DCpy)*ODCmx];//Karte Textur an aktueller stele
        ODCc=Map[(x+ODCpx2)+(y+ODCpy2)*ODCmx];//alte Karten Textur aktuell dargestellt

//Wenn Kachel ausserhalb map Kachel 0
        if (x+DCpx <0 || y+DCpy <0 ) DCc=0;
        if (x+DCpx >=ODCmx || y+DCpy >=ODCmy ) DCc=0;

//Wenn vorherige Kachel ausserhalb map Kachel 0
        if (x+ODCpx2 <0 || y+ODCpy2 <0 ) ODCc=0;
        if (x+ODCpx2 >=ODCmx || y+ODCpy2 >=ODCmy ) ODCc=0;

        if (DCc !=ODCc)//ODCc
        {
            if (DCc > 0 ) DCtexB(DCx,(DCy+MXframe),DCc);
//Leere Felder schwarz >>>>>
            if (DCc <= 0 ) DCtexCls(DCx,(DCy+MXframe),0);
        }


        x=x+1;

        if (x>9)
        {
            x=-9;
            y=y+1;

        }
//y=y+1;
    }while(y<6);


//Aktuelle Position in alte Position speichern!

    ODCpx2=ODCpx;
    ODCpy2=ODCpy;

    ODCpx=DCpx;
    ODCpy=DCpy;

//spieler
    DCx=160-8+(ODpx-DCpx)*16;
    DCy=104+(ODpy-DCpy)*16;
    DCtexCls(DCx,DCy+MXframe,55);
}
//<<<<<


//############################ DC-Plattform MAP #############################
void DCmap()
{
    char tx=0;
    char ty=0;

    do {
        MXpset(tx,ty,Map[tx+ty*ODCmx]);
        tx=tx+1;
        if (tx >= ODCmx)
        {
            tx=0;
            ty=ty+1;
        }
    } while(ty<ODCmy);


    MXpset(ODpx,ODpy,12);

//MXpset(ODpx-5,ODpy-5,12);
//MXpset(ODpx-5,ODpy+5,12);
//MXpset(ODpx+5,ODpy-5,12);
//MXpset(ODpx+5,ODpy+5,12);
}


//############################################################################
//Benchmark / Testsysteme       *                  *          *            *
//    *                   *             *             *               *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################


//Speedtest 6000 Kacheln setzen Modell texB war am schnellsten andere entfernt!
//
//Plattform 386DX40 5-6 Sekunden 6000 Kacheln
//
//DosBox default <1 Sekunden
void FPStest()
{

    int temp=0;
    time_t otime=time(0);

    ODCpx=32;
    ODCpy=32;


    do{
        //DC(5,5);

        //Versuche
        //>>kachel<< wilde anordnung mit geringstem Rechenaufwand,keine ModeX Optimierung m”glich!
        //>>DCtex<< Horizontale Darstellung langsam zu hoher Rechenaufwand,keine ModeX Optimierung m”glich!



        //DCtexB vertikaler Bildaufbau!!!
        //Nutz ModeX meist Optimal
        //Register/Masken wechsel von 256 auf 32 reduziert!!
        //Erh”ht die Leistung von 10 auf 6 Sekunden Ladezeit!
        DCtexB(100,100,1);


        temp++;
    } while(temp<6000);

    asm {
        mov    ax, 0h
            int    10h         //{ Get into MCGA Mode }
            }

    otime=time(0)-otime;
    printf("%ld",otime);
    delay(2000);

    MXini();
}
//<<<<<


//############################ MXprintc(x,y,Color,"Char-Array") #############################
void MXprint(int MXx , int MXy,int MXcolor,char MXc[40])
{
    int temp=0;

    do
    {
        int MXchr = (MXc[temp]-33)*9;
        int MXzy=0;
        unsigned char MXz =  ZS[MXchr+MXzy];

        if (MXc[temp] < 33 || MXc[temp] > 132 ) goto wende;

        do
        {
            MXz =  ZS[MXchr+MXzy];

            outp(0x3c4,2);//select map mask register
            outp(0x3c5,MXz);//write 2^plane
            MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp)] = MXcolor;



            outp(0x3c4,2);//select map mask register
            outp(0x3c5, MXz>>4);//write 2^plane
            MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp+1)] = MXcolor;

            MXzy++;
        } while(MXzy<9);

    wende:

        temp++;
    } while(temp<30);
}
//<<<<<


//############################ MXprintc(x,y,Color,"Char-Array") #############################
//mit hintergrundfarbe!
void MXprint(int MXx , int MXy,int MXcolor,int MXcolor2,char MXc[40]){

    int temp=0;

    do
    {
        int MXchr = (MXc[temp]-33)*9;
        int MXzy=0;
        unsigned char MXz =  ZS[MXchr+MXzy];

        do
        {

            MXz =  ZS[MXchr+MXzy];

            if (MXc[temp] < 33 || MXc[temp] > 132 )
            {
                //Hintergrund
                outp(0x3c4,2);//select map mask register
                outp(0x3c5,255);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp)] = MXcolor2;

                outp(0x3c4,2);//select map mask register
                outp(0x3c5, 255);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp+1)] = MXcolor2;

            }
            else
            {
                outp(0x3c4,2);//select map mask register
                outp(0x3c5,MXz);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp)] = MXcolor;

                outp(0x3c4,2);//select map mask register
                outp(0x3c5, MXz>>4);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp+1)] = MXcolor;

                //Hintergrund
                outp(0x3c4,2);//select map mask register
                outp(0x3c5,~MXz);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp)] = MXcolor2;

                outp(0x3c4,2);//select map mask register
                outp(0x3c5, ~MXz>>4);//write 2^plane
                MXVGA[((MXy+MXzy)<<6) + ((MXy+MXzy)<<4) + (MXx+temp+temp+1)] = MXcolor2;
            }

            MXzy++;
        } while(MXzy<9);


        temp++;
    } while(temp<30);
}
//<<<<<


//############################################################################
//DOS MOUSE Treiber muss geladen sein     *            *           *
//    *                   *             *             *               *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################


//############################ Mouse aviable ja=FFFFh / nein = 0 ###########
int MouseDetect(){

    int mouse=0;
    asm  {
        mov    ax,0   //Mouse an
            int    33h
            mov    mouse,AX
            }
    return(mouse);
}
//<<<<<


//############################ Mouse Show ##################################
//ModeX zeigt Cursor fehlerhaft!!
int MouseShow(){

    int mouse=0;
    asm  {
        mov    ax,1   //Mouse an
            int    33h
            }
    return(mouse);
}
//<<<<<



//############################ Mouse Hide ##################################
int MouseHide(){

    int mouse=0;
    asm  {
        mov    ax,2   //Mouse an
            int    33h
            }
    return(mouse);
}
//<<<<<


//############################ Mouse Get X,Y,Button ###################
int MouseGet(){


    asm  {
        mov    ax,3   //Mouse Get
            int    33h

            mov    MouseB,BX //Bit >> 1 Left / 2 Right / 3 Center
            mov    MouseX,CX
            mov    MouseY,DX
            }
    return(1);
}
//<<<<<


//############################ Mouse Set X,Y ###############################
int MouseSet(int MouseSetX,int MouseSetY){


    asm  {

        mov    CX,MouseSetX
            mov    DX,MouseSetY

            mov    ax,4   //Mouse Set
            int    33h

            mov    MouseB,BX //Bit >> 1 Left / 2 Right / 3 Center
            mov    MouseX,CX
            mov    MouseY,DX
            }
    return(1);
}
//<<<<<


//############################ Mouse X-Range Min,Max ###############################
int MouseXrange(int MouseMin,int MouseMax){


    asm  {

        mov    CX,MouseMin
            mov    DX,MouseMax

            mov    ax,7   //Mouse Set
            int    33h

            }
    return(1);
}
//<<<<<


//############################ Mouse Y-Range Min,Max ###############################
int MouseYrange(int MouseMin,int MouseMax){


    asm  {

        mov    CX,MouseMin
            mov    DX,MouseMax

            mov    ax,8   //Mouse Set
            int    33h

            }
    return(1);
}
//<<<<<


void EF1(void)
{
//54/9
    if (MXE1==1){

        MXpaletteS(240,0,0,235);

        MXpaletteS(241,10,40,255);//Wert / Rot / Grn / Blau
        MXpaletteS(242,10,20,255);
        MXpaletteS(243,0,20,235);
        MXpaletteS(244,0,30,235);


    }
    if (MXE1==2){
        MXpaletteS(241,10,35,255);
        MXpaletteS(242,10,25,255);
        MXpaletteS(243,0,15,235);
        MXpaletteS(243,0,35,235);
    }
    if (MXE1==3){
        MXpaletteS(241,10,30,255);
        MXpaletteS(242,10,30,255);
        MXpaletteS(243,0,20,235);
        MXpaletteS(243,0,40,235);
    }
    if (MXE1==4){
        MXpaletteS(241,10,25,255);
        MXpaletteS(242,10,35,255);
        MXpaletteS(243,0,25,235);
        MXpaletteS(243,0,35,235);
    }
    if (MXE1==5){
        MXpaletteS(241,10,20,255);
        MXpaletteS(242,10,40,255);
        MXpaletteS(243,0,30,235);
        MXpaletteS(243,0,30,235);
//max
    }
    if (MXE1==6){
        MXpaletteS(241,10,25,255);
        MXpaletteS(242,10,35,255);
        MXpaletteS(243,0,35,235);
        MXpaletteS(243,0,25,235);
    }
    if (MXE1==7){
        MXpaletteS(241,10,30,255);
        MXpaletteS(242,10,30,255);
        MXpaletteS(243,0,40,235);
        MXpaletteS(243,0,20,235);
    }
    if (MXE1==8){
        MXpaletteS(241,10,35,255);
        MXpaletteS(242,10,25,255);
        MXpaletteS(243,0,35,235);
        MXpaletteS(243,0,25,235);
    }
    if (MXE1==9){
        MXpaletteS(241,10,40,255);
        MXpaletteS(242,10,20,255);
        MXpaletteS(243,0,30,235);
        MXpaletteS(243,0,30,235);
    }
    MXE1++;
    if (MXE1>8) MXE1=1;
}
//<<<<<








//Palette setzen komplett 51 Wert Schritte 216 Farben + 16 Standard = 24 Reserve Effekte
void Palette(void)
{

    unsigned int MXr=0;
    unsigned int MXg=0;
    unsigned int MXb=0;
    unsigned int MXc=16;
    do
    {

        MXpaletteS(MXc,MXr,MXg,MXb);//Wert / Rot / Grn / Blau
        MXc++;

        MXr=MXr+51;

        if (MXr>255) {
            MXr=0;
            MXg=MXg+51;
        }

        if (MXg>255) {
            MXr=0;
            MXg=0;
            MXb=MXb+51;
        }

    }while(MXc<232);
    delay(2000);


}


//############################################################################
//MAIN Void... kann noch fast nix!   *          *            *           *
//    *                   *             *             *               *
//             *                   *            *              *           *
// Testsystem 80386DX40 ET4000
//############################################################################
int main(void)
{
    MXini();//setzt MODEX
    MXlatch(0);
    MXcls(0);
    Palette();

//DATEI TEST >>>>>>>>

/*
  char buffer[100];


  ifstream ifs; //Eingabestream
  ifs.open("1.txt");
  ifs.read(buffer,100);

//if (! ifs) << fehler beim ”ffnen!
MXprint(10,10,2,0, buffer);
delay(2000);
ifs.close();

//DATEI TEST <<<<<<<<
*/




    char ch;
    int x=0;
    int y=0;
    int z=0;

//Kachel Position (x,y)
    x=0;
    y=0;


    DC(x,y);

    char DCscroll=3;


//return(0);


//MXprint(10,10,11,100,"Courier 9 Testtext 1234567890 <>            ");
//MXprintc(10,20,11,"abcdefghijklmnopqrstuvwxyz");
//MXprintc(10,30,11,"1234567890 +-*/!$%&/()=?");


//prfen ob Maus verfgbar
//if (MouseDetect() != 0) MXprintc(10,40,11,"- Mouse");



    MouseXrange(0,320);
    MouseYrange(0,200);

    int DCx;
    int DCy;

    int MKx=0;
    int MKy=0;
    unsigned char farbe=0;


    do
    {
//printf("%.10f",d);


        if (kbhit() )
        {

            ch=getche();
//    printf("\key >" , ch);
//cout << "1 | -";
            if (ch=='a') ODpx=ODpx-1;
            if (ch=='d') ODpx=ODpx+1;
            if (ch=='w') ODpy=ODpy-1;
            if (ch=='s') ODpy=ODpy+1;
//Scroll Modus im spiel
            if (ch=='1')  DCscroll=1;
            if (ch=='2')  DCscroll=2;
            if (ch=='3')  DCscroll=3;

            if (ch=='5')  FPStest();

            if (ch=='y') farbe--;
            if (ch=='x') farbe++;


            if (ch=='c')
            {
                MXlatch(1);
                MXcopy( 0, 0, 160,10, 160, 100);
                MXlatch(0);
            }


            int ch=0;

            //Darstellungsmodie
            //wenn spieler am Rand der Grafik -1 steht springt Bild um 3 felder weiter
            if (DCscroll==1){
                if (ODpx-x > 7 ) x=ODpx;
                if (ODpx-x < -7 ) x=ODpx;
                if (ODpy-y > 4 ) y=ODpy;
                if (ODpy-y < -4 ) y=ODpy;
            }
            //wenn spieler am Rand der Grafik -1 steht springt Bild 1 weiter
            if (DCscroll==2){
                if (ODpx-x > 7 ) x=ODpx-7;
                if (ODpx-x < -7 ) x=ODpx+7;
                if (ODpy-y > 4 ) y=ODpy-4;
                if (ODpy-y < -4 ) y=ODpy+4;
            }
            //wenn Spieler immer im Zentrum
            if (DCscroll==3){
                x=ODpx;
                y=ODpy;

            }

            EF1();

//    DC(x,y);//Grafik
//MXflip(16000);


        }
        else
        {
        }

        DC(x,y);//Grafik
        char Farbec[3];

        MXprint(0,0+MXframe,farbe,0,( itoa(farbe,Farbec,10) ));
        MXflip(0);

//    DCmap();//Karte
//    DC(x,y);//Grafik



//Mouse Position abfragenb und zeigen
//MouseGet();
        if (MouseB == 1)
        {
        }


//Position Raster auswerten!<<<<<



    } while(ch != 'q');



    MXoff();

    printf("Ende!\n");
    delay(1000);

}
//Ende........
