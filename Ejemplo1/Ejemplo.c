#include <gb/gb.h>
#include <stdio.h>
#include "Tile1.c"
#include "Tile2.c"
#include "Mapa.c"
#include "Nivel1.c"
#include "Nivel1objs.c"
#include "Nivel2.c"
#include "Nivel2objs.c"

#define MAX_OBJS    3
#define MAX_MONEDAS 10

unsigned char* ListaNiveles[]=
{
    Nivel1Label, Nivel2Label,
};

unsigned char* ListaNivelesObjetos[]=
{
    Nivel1objsLabel, Nivel2objsLabel,
};

struct Objeto
{
    UBYTE spritids[4];
    UINT8 x, y, act, tipo, dir;
	signed vy;
};

unsigned char oldplx, plx, ply, pldir, camx;
struct Objeto jugador, enemigo[MAX_OBJS], bala[MAX_OBJS], moneda[MAX_MONEDAS];
signed char plvy, scrollX;
unsigned char A_pressed, B_pressed;
unsigned char animFrame, enemFrame;

unsigned int i, j;
unsigned char en, b, m, mn, nivel_id;

void moverObjeto(struct Objeto* obj, UINT8 x, UINT8 y)
{
    move_sprite(obj->spritids[0],x,y);
    move_sprite(obj->spritids[1],x+8,y);
    move_sprite(obj->spritids[2],x,y+8);
    move_sprite(obj->spritids[3],x+8,y+8);
}

void setupBalas()
{
    set_sprite_tile(15,13);
    bala[b%MAX_OBJS].spritids[0]=15;
    set_sprite_tile(16,13);
    bala[b%MAX_OBJS].spritids[1]=16;
    set_sprite_prop(16, 32); 

    bala[b%MAX_OBJS].act=1;

    bala[b%MAX_OBJS].x = plx;
    bala[b%MAX_OBJS].y = ply;

    bala[b%MAX_OBJS].dir = pldir;
}

void setupJugador()
{
	plx=((i%32)<<3)+8;
    ply=((i>>5)<<3)+16;
	
	//plx=16;
	//ply=16;
	
    oldplx=plx;
	
	pldir=1;
	
    jugador.x = plx;
    jugador.y = ply;

    set_sprite_tile(1,1);
    jugador.spritids[0]=1;
    set_sprite_tile(2,2);
    jugador.spritids[1]=2;
    set_sprite_tile(3,3);
    jugador.spritids[2]=3;
    set_sprite_tile(4,4);
    jugador.spritids[3]=4;
}

void setupEnemigo()
{
    enemigo[en].x = ((i%32)<<3)+8;
    enemigo[en].y = ((i>>5)<<3)+16;
    enemigo[en].act = 1;

    switch (enemigo[en].tipo)
    {
        case 1:
            set_sprite_tile(7,7);
            enemigo[en].spritids[0]=7;
            set_sprite_tile(8,8);
            enemigo[en].spritids[1]=8;
            set_sprite_tile(9,9);
            enemigo[en].spritids[2]=9;
            set_sprite_tile(10,10);
            enemigo[en].spritids[3]=10;
        break;

        case 2:
            set_sprite_tile(11,11);
            enemigo[en].spritids[0]=11;
            set_sprite_tile(12,11);
            enemigo[en].spritids[1]=12;
            set_sprite_tile(13,12);
            enemigo[en].spritids[2]=13;
            set_sprite_tile(14,12);
            enemigo[en].spritids[3]=14;

            set_sprite_prop(12, 32);
            set_sprite_prop(14, 32);    
        break;
    }
}

void setupMoneda()
{
    moneda[m].x = (i%32)<<3;
    moneda[m].y = (i>>5)<<3;
    moneda[m].act = 1;
}


void cargarObjetos()
{
    en=0;
	m=0;

    for(i=0;i<512;++i)
    {
        if ((ListaNivelesObjetos[nivel_id])[i]==7)//Personaje principal
        {
            setupJugador();
        }
        if ((ListaNivelesObjetos[nivel_id])[i]==8)//Enemigo1
        {
            enemigo[en].tipo=1; setupEnemigo(); ++en;
        }
        if ((ListaNivelesObjetos[nivel_id])[i]==9)//Enemigo2
        {
            enemigo[en].tipo=2; setupEnemigo(); ++en;
        }
		if ((ListaNivelesObjetos[nivel_id])[i]==1)//monedad
        {
            setupMoneda(); ++m;
        }
    }
}

void matarObjeto(struct Objeto* obj)
{
    obj->x = 0;
    obj->y = 0;
    obj->act = 0;

    moverObjeto(obj, 0, 0);
}

UBYTE colision_en(UINT8 newx, UINT8 newy)
{
    UINT16 indexTLx, indexTLy, tileindexTL;
    UBYTE resultado;

    indexTLx = (newx>>3)&31;
    indexTLy = (newy>>3)<<5;
    tileindexTL = indexTLy + indexTLx;

    resultado = (ListaNiveles[nivel_id])[tileindexTL]>=2;

    return resultado;
}

void animarEnemigo()
{
    ++enemFrame;

    if (enemFrame&8)
    {
        set_sprite_tile(7+!enemigo[i].dir,0);
        set_sprite_tile(8-!enemigo[i].dir,0);
        set_sprite_tile(9+!enemigo[i].dir,5);
        set_sprite_tile(10-!enemigo[i].dir,6);
    }
    else
    {
        set_sprite_tile(7+!enemigo[i].dir,7);
        set_sprite_tile(8-!enemigo[i].dir,8);
        set_sprite_tile(9+!enemigo[i].dir,9);
        set_sprite_tile(10-!enemigo[i].dir,10);
    }
}

void actualizarEnemigo()
{
    for (i=0;i<en;++i)
    {
        if (enemigo[i].act)
        {
            switch (enemigo[i].tipo)
            {
                case 1: //ENEMIGO 1
                    if (!enemigo[i].dir)
                    {
                        --enemigo[i].x;
                        set_sprite_prop(7, 32);
                        set_sprite_prop(8, 32);
                        set_sprite_prop(9, 32);
                        set_sprite_prop(10, 32);

                        animarEnemigo();
                    }
                    else if (enemigo[i].dir) 
                    {
                        ++enemigo[i].x;
                        set_sprite_prop(7, 0);
                        set_sprite_prop(8, 0);
                        set_sprite_prop(9, 0);
                        set_sprite_prop(10, 0);

                        animarEnemigo();
                    }

                    if (enemigo[i].vy>2) enemigo[i].vy=2; else ++enemigo[i].vy;

                    if (colision_en(enemigo[i].x-8,enemigo[i].y-1)||enemigo[i].x<8) enemigo[i].dir=1;
                    if (colision_en(enemigo[i].x+8,enemigo[i].y-1)||enemigo[i].x>256-8) enemigo[i].dir=0;
                    
                    if (colision_en(enemigo[i].x-7,enemigo[i].y)||colision_en(enemigo[i].x+7,enemigo[i].y)) enemigo[i].y=(enemigo[i].y>>3)<<3;
                    else enemigo[i].y+=enemigo[i].vy;
                break;

                case 2: //ENEMIGO 2
                    if (enemigo[i].vy>2) enemigo[i].vy=2; else ++enemigo[i].vy;

                    if (colision_en(enemigo[i].x,enemigo[i].y)||colision_en(enemigo[i].x+7,enemigo[i].y))
                    {
                        --enemigo[i].y;
                        enemigo[i].vy=-10;
                    }
                    else enemigo[i].y+=enemigo[i].vy;
                break;
            }

            moverObjeto(&enemigo[i], enemigo[i].x-camx, enemigo[i].y);
        }
        else matarObjeto(&enemigo[i]);
    }
}


UBYTE colisionObjetos(struct Objeto* uno, struct Objeto* dos)
{
    return (uno->x >= dos->x && uno->x <= dos->x + 8) && (uno->y >= dos->y && uno->y <= dos->y + 8)
    || (dos->x >= uno->x && dos->x <= uno->x + 8) && (dos->y >= uno->y && dos->y <= uno->y + 8);
}

void actualizarBalas()
{
    for (i=0;i<MAX_OBJS;++i)
    {
        if (bala[i].act)
        {
            if (bala[i].dir) bala[i].x+=4; else bala[i].x-=4;

            for (j=0;j<MAX_OBJS;++j)
            {
                if (colisionObjetos(&bala[i],&enemigo[j]))
                {
                    enemigo[j].act=0;
                    bala[i].act=0;
					
					NR10_REG = 0x1E;
                    NR11_REG = 0x80;
                    NR12_REG = 0x43;
                    NR13_REG = 0x73;
                    NR14_REG = 0x86;
                }
            }
            if (bala[i].x<8||bala[i].x>256-8) bala[i].act=0;
            moverObjeto(&bala[i], bala[i].x-camx, bala[i].y);
        }
        else matarObjeto(&bala[i]);
    }
}


void actualizarMonedas()
{
    for (i=0;i<MAX_MONEDAS;++i)
    {
        if (colisionObjetos(&jugador,&moneda[i]) && moneda[i].act) 
        {
            moneda[i].act=0;
            set_bkg_tiles(moneda[i].x>>3,moneda[i].y>>3,1,1,0);
			
			++mn;
			
			NR21_REG = 0x80;
            NR22_REG = 0x43;
            NR23_REG = 0x6D;
            NR24_REG = 0x86;
        }
    }
}

unsigned char Mensaje[] =
{
    36,38,37,28,27,24,42,17,
};

unsigned char Numeros[] =
{
    7,8,9,10,11,12,13,14,15,16,
};

void actualizarMensaje()
{
    set_win_tiles(9,1,1,1,Numeros+((mn/10)%10));
    set_win_tiles(10,1,1,1,Numeros+(mn%10));
}

void animarPersonaje()
{
    ++animFrame;

    if (animFrame&4)
    {
        set_sprite_tile(1+!pldir,0);
        set_sprite_tile(2-!pldir,0);
        set_sprite_tile(3+!pldir,5);
        set_sprite_tile(4-!pldir,6);
    }
    else
    {
        set_sprite_tile(1+!pldir,1);
        set_sprite_tile(2-!pldir,2);
        set_sprite_tile(3+!pldir,3);
        set_sprite_tile(4-!pldir,4);
    }
}

void cargarDatos()
{
    set_sprite_data(0,14,TileLabel2);
    set_sprite_tile(0,0);

    plx=16; ply=16;
    oldplx=plx;

    cargarObjetos();

    set_bkg_tiles(0,0,32,16,ListaNiveles[nivel_id]);
}


void main()
{
	A_pressed=0;
	B_pressed=0;
	
	NR52_REG = 0x80; 
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
	
	cargarDatos();


	SHOW_SPRITES;
	SHOW_BKG;
	SHOW_WIN;
	
	set_bkg_data(0,54,MapaLabel);
    set_bkg_tiles(0,0,32,16,Nivel1Label);
	
	set_win_tiles(1,1,8,1,Mensaje);
    move_win(7,120);
	
	//setupJugador();
	while(1)
	{
	    jugador.x = plx;
		jugador.y = ply;
		
		actualizarEnemigo();
		actualizarBalas();
		actualizarMonedas();
		actualizarMensaje();
	
	    if (joypad() & J_LEFT)
        {
            if (plx>8 && (!colision_en(plx-8,ply-1)))
            --plx;
            pldir=0;
			
            set_sprite_prop(1, 32);
            set_sprite_prop(2, 32);
            set_sprite_prop(3, 32);
            set_sprite_prop(4, 32);
        }
        if (joypad() & J_RIGHT)
        {
            if (plx<256-8 && (!colision_en(plx+8,ply-1)))
            ++plx;
            pldir=1;
			
            set_sprite_prop(1, 0);
            set_sprite_prop(2, 0);
            set_sprite_prop(3, 0);
            set_sprite_prop(4, 0);
        }
        if (joypad() & J_UP)
        {
            --ply;
        }
        if (joypad() & J_DOWN)
        {
            ++ply;
        }
		
		
        animarPersonaje();

        if (!(joypad() & J_LEFT)&&!(joypad() & J_RIGHT)) animFrame=0;
		
		if (plvy>2) plvy=2; else ++plvy;
		
        if (colision_en(plx-7,ply)||colision_en(plx+7,ply)) ply=(ply>>3)<<3;
        else ply+=plvy;
		
		if ((ListaNivelesObjetos[nivel_id])[(((ply>>3)-2)<<5)+(((plx>>3)-1)&31)]==10)
        {
            ++nivel_id;
            scroll_bkg(oldplx-88,0);
            cargarDatos();
        }
		
		if (joypad() & J_A)
        {
            if (!A_pressed)
            {
                --ply;
                A_pressed=1;
                plvy=-10;
				
                NR10_REG = 0x16;
                NR11_REG = 0x40;
                NR12_REG = 0x73;
                NR13_REG = 0x00;
                NR14_REG = 0xC3;
            }
        } else A_pressed=0;
		
		if (joypad() & J_B)
        {
            if (!B_pressed)
            {
                setupBalas();
                ++b;
                B_pressed=1;
				
				NR41_REG = 0x1F;
                NR42_REG = 0xF1;
                NR43_REG = 0x30;
                NR44_REG = 0xC0;
            }
        } else B_pressed=0;
		
		if(oldplx<plx) {scrollX=-1; oldplx=plx;}
        if(oldplx>plx) {scrollX=1; oldplx=plx;}
		
		if (plx>=80-!pldir && plx<256-80+pldir)
        {
            moverObjeto(&jugador, 80, ply);
            scroll_bkg(-scrollX,0);
			camx=plx-80;
        }
        else
		if (plx>=256-80+pldir)
        moverObjeto(&jugador,plx-256-80+pldir-16,ply);
        else
        {
            moverObjeto(&jugador,plx,ply);
            camx=0;
        }

	
		//moverObjeto(&enemigo[0], enemigo[0].x, enemigo[0].y);
	
		if(oldplx==plx) {scrollX=0;}
	
		delay(10);
	}
}