#include "stdlib.h"
#include "stdio.h"
#include <GL/glut.h>
#include <ctype.h>
#include <math.h>
#include "include/objetos.hxx"
#include "include/texto.hxx"
#include "include/materiales.hxx"
#include "include/luces.hxx"



// variables que definen la posicion de la camara en coordenadas polares
GLfloat Observer_distance;
GLfloat Observer_angle_x;
GLfloat Observer_angle_y;

// variables que controlan la ventana y la transformacion de perspectiva
GLfloat Window_width,Window_height,Front_plane,Back_plane;

// variables que determninan la posicion y tamaño de la ventana X
int UI_window_pos_x=50,UI_window_pos_y=50,UI_window_width=500,UI_window_height=500;

//**************************************************************************
//
//***************************************************************************
//variable que determina si se muestra puntos, lineas,solido,ajedrez,smooth
int drawforma=1;
float posicion_luz_focal[]= {-5.0f,  10.0f, 10.0f};
float posicion_luz_puntual[]={1.0f,2.0f,3.0f};
int direction[3];

_cubo cubo(3); //CUBO A MOSTRAR
_piramide piramide(3,3); //PIRAMIDE A MOSTRAR
_suelo suelo(10);
float rgb[3]; //array de colores para el texto
//Archivo cargado
_objeto_archivo obj_cargado;


// -- CREAMOS LAS LUCES ------------------------------------------------------
luces *luz = new luces[2];
// -- CREAMOS EL TEXTO -------------------------------------------------------
texto textos;
// -- CREAMOS EL MATERIAL ----------------------------------------------------
materiales *mat = new materiales[2];
// -- ACTIVA LUCES -----------------------------------------------------------
bool act1=false,act2=false;


void clear_window()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}


//**************************************************************************
// Funcion para definir la transformación de proyeccion
//***************************************************************************

void change_projection()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// formato(x_minimo,x_maximo, y_minimo, y_maximo,Front_plane, plano_traser)
	//  Front_plane>0  Back_plane>PlanoDelantero)
	glFrustum(-Window_width,Window_width,-Window_height,Window_height,Front_plane,Back_plane);
}

//**************************************************************************
// Funcion para definir la transformación de vista (posicionar la camara)
//***************************************************************************

void change_observer()
{

	// posicion del observador
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-Observer_distance);
	glRotatef(Observer_angle_x,1,0,0);
	glRotatef(Observer_angle_y,0,1,0);
}

//**************************************************************************
// Funcion que dibuja los ejes utilizando la primitiva grafica de lineas
//***************************************************************************

void draw_axis()
{
	glBegin(GL_LINES);
	// eje X, color rojo
	glColor3f(1,0,0);
	glVertex3f(-AXIS_SIZE,0,0);
	glVertex3f(AXIS_SIZE,0,0);
	// eje Y, color verde
	glColor3f(0,1,0);
	glVertex3f(0,-AXIS_SIZE,0);
	glVertex3f(0,AXIS_SIZE,0);
	// eje Z, color azul
	glColor3f(0,0,1);
	glVertex3f(0,0,-AXIS_SIZE);
	glVertex3f(0,0,AXIS_SIZE);
	glEnd();
}

//**************************************************************************
//Funcion que carga los materiales
//***************************************************************************

void carga_materiales(int i,float r, float g, float b){
	mat[i].ambient(r,g,b);// Definición del color del material
	mat[i].emissive(0.0f,0.0f,0.0f);// Emisión de luz del propio objeto
	mat[i].specular(0.9f,0.9f,0.8f);// Definición del brillo del objeto
	mat[i].diffuse(  1.0f, 1.0f, 1.0f );//color cuando esta iluminados
	mat[i].phongsize(0.0f);
	mat[i].shininess(40.0f);
	mat[i].activa();
	
}
//**************************************************************************
//Funcion que carga la luz
//***************************************************************************

void carga_luz(float xf, float yf,float zf,float xp, float yp,float zp,int* direction){
	luz[0].position(  xf,  yf, zf );
	luz[0].diffuse(   0.5f,  0.5f, 0.5f );
	luz[0].ambient(   0.2f,  0.2f, 0.2f );
	luz[0].specular(0.5f,  0.5f, 0.5f);
	luz[0].direction(direction[0],direction[1],direction[2]);
	luz[0].focal( GL_LIGHT0 );
	
	luz[1].position(  xp,  yp, zp );
	luz[1].diffuse(   0.0f,  0.0f, 0.0f );
	luz[1].ambient(   0.0f,  0.0f, 0.0f );
	luz[1].puntual( GL_LIGHT1 );
}

//**************************************************************************
//Funcion que dibuja el texto
//***************************************************************************

void dibujar_texto(){
		//pongo el color del texto
	rgb[0]=1.0f;
	rgb[1]=0.0f;
	rgb[2]=0.0f;
	textos.dibujar_texto(5,UI_window_width-24,GLUT_BITMAP_TIMES_ROMAN_24,(char *)"F1 para modo puntos",rgb,UI_window_width,UI_window_height);
	textos.dibujar_texto(5,UI_window_width-52,GLUT_BITMAP_TIMES_ROMAN_24,(char *)"F2 para modo lineas",rgb,UI_window_width,UI_window_height);
	textos.dibujar_texto(5,UI_window_width-80,GLUT_BITMAP_TIMES_ROMAN_24,(char *)"F3 para modo solido",rgb,UI_window_width,UI_window_height);
	textos.dibujar_texto(5,UI_window_width-108,GLUT_BITMAP_TIMES_ROMAN_24,(char *)"F4 para modo ajedrez",rgb,UI_window_width,UI_window_height);
	textos.dibujar_texto(5,UI_window_width-136,GLUT_BITMAP_TIMES_ROMAN_24,(char *)"F5 para modo smooth",rgb,UI_window_width,UI_window_height);
	
}


// -----------------------------------------------------------------------------
// -- Hace una máscara para limitar el dibujo de las Sombras -------------------
// -----------------------------------------------------------------------------
const void Hacer_Mascara_Sombra( void ) {
  glClear( GL_STENCIL_BUFFER_BIT );
  glDisable( GL_DEPTH_TEST );
  glEnable( GL_STENCIL_TEST );
  glStencilFunc( GL_ALWAYS, 1, 1 );
  glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
}

const void Terminar_Mascara_Sombra( void ) {
  glStencilFunc( GL_EQUAL, 1, 1 );
  glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
}

//**************************************************************************
// Funcion que dibuja los reflejos

//***************************************************************************
void reflejos(){
	 if ( glIsEnabled( GL_LIGHT0 ) || glIsEnabled( GL_LIGHT1 )) {
		 direction[0]=1;
		 direction[1]=1; //hacia arriba
		 direction[2]=1;
	glPushMatrix();
	carga_luz( posicion_luz_focal[0],  -(posicion_luz_focal[1]+10), posicion_luz_focal[2],posicion_luz_puntual[0],posicion_luz_puntual[1],posicion_luz_puntual[2],direction);
	glPopMatrix();
	carga_luz( posicion_luz_focal[0],   -(posicion_luz_focal[1]+10), posicion_luz_focal[2],posicion_luz_puntual[0],posicion_luz_puntual[1],posicion_luz_puntual[2],direction);
	// -- DIBUJAR REFLEJOS EN EL SUELO -----------------------------------------
      glPushMatrix();
		glTranslatef( 0, -12, 0 );
        glRotatef( 180, 1.0f, 0.0f, 0.0f );
        carga_materiales(1,1.0f,0.0f,0.0f); //materiales color rojo
          mat[1].alpha( 0.4f );
          mat[1].activa();
            cubo.draw_solido_smooth(); // -- CUBO REFLEJADO --
          mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
          mat[1].alpha( 1.0f );
        
	  glPopMatrix();
	  
	  carga_materiales(1,0.0f,0.0f,0.0f); //materiales color negro
	  glPushMatrix();
	    glTranslatef( 3, -12, 0 );
        glRotatef(-90,0,0,1);
        glRotatef(180,0,1,0);
          mat[1].alpha( 0.4f );
          mat[1].activa();
	  piramide.draw_solido_smooth();
		  mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
          mat[1].alpha( 1.0f );
	  glPopMatrix();
		
	 glPushMatrix();
	 glTranslatef( -3, -12, 0 );
        glRotatef(90,0,0,1);
        glRotatef(180,0,1,0);
          mat[1].alpha( 0.4f );
          mat[1].activa();
	  piramide.draw_solido_smooth();
		  mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
          mat[1].alpha( 1.0f );
	 glPopMatrix();
	 
	 
	 
	glPushMatrix();
	glTranslatef( -5, -8, 8 );
        glRotatef( 180, 1.0f, 0.0f, 0.0f );
          mat[1].alpha( 0.4f );
          mat[1].activa();
			obj_cargado.draw_solido_smooth();
		mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
        mat[1].alpha( 1.0f );
	glPopMatrix();
			
	glPushMatrix();
	glTranslatef( 0, -8, 8 );
        glRotatef( 180, 1.0f, 0.0f, 0.0f );
          mat[1].alpha( 0.4f );
          mat[1].activa();
			obj_cargado.draw_solido_smooth();
		mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
        mat[1].alpha( 1.0f );
	glPopMatrix();	
		
		glPushMatrix();
	glTranslatef( 5, -8, 8 );
        glRotatef( 180, 1.0f, 0.0f, 0.0f );
          mat[1].alpha( 0.4f );
          mat[1].activa();
			obj_cargado.draw_solido_smooth();
		mat[1].desactiva();   // -- OJO, IMPORTANTE DESACTIVAR MATERIAL --
        mat[1].alpha( 1.0f );
	glPopMatrix();
	//glColorMask(1,1,1,1);
  }
	
}
void suelo_reflejos(){
	    glEnable( GL_STENCIL_TEST );
		glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			
			carga_materiales(0,1.0f,1.0f,1.0f); //suelo color blanco
			glTranslatef(0.0,-7,0.0);
			suelo.draw_solido_smooth();
			glTranslatef(0.0,7,0.0);
			
			glDisable(GL_CULL_FACE);
		glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF  );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
		glDisable(GL_DEPTH_TEST);
}



//**************************************************************************
// Funcion que dibuja los objetos
//***************************************************************************

void draw_objects()
{
	
	switch(drawforma){
		case 1:
			cubo.draw_puntos(1,0,0,3);
			
			glPushMatrix();
			obj_cargado.draw_puntos(1,0,0,3);
			glPopMatrix();
			
			glPushMatrix();
			glTranslatef(-3,0,0.0);
			glRotatef(90,0,0,1);
			piramide.draw_puntos(1,0,0,3);
			glPopMatrix();
			
			glPushMatrix();
			glTranslatef(3,0,0.0);
			glRotatef(-90,0,0,1);
			piramide.draw_puntos(1,0,0,3);
			glPopMatrix();
		break;
		case 2:
			  
			cubo.draw_aristas(1,0,0,2);
			
			glPushMatrix();
			glTranslatef(0,5,0.0);
			obj_cargado.draw_aristas(1,0,0,3);
			glPopMatrix();
			   
			glPushMatrix();
			glTranslatef(-3,0,0.0);
			glRotatef(90,0,0,1);
			piramide.draw_aristas(1,0,0,2);
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(3,0,0.0);
			glRotatef(-90,0,0,1);
			piramide.draw_aristas(1,0,0,2);
			glPopMatrix();
		break;
		case 3:
			cubo.draw_solido(1,1,0);
			
			glPushMatrix();
			glTranslatef(0,5,0.0);
			obj_cargado.draw_solido(0,0,1);
			glPopMatrix();
			
			glPushMatrix();
			glTranslatef(3,0,0.0);
			glRotatef(-90,0,0,1);
			piramide.draw_solido(1,0,0);
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(-3,0,0.0);
			glRotatef(90,0,0,1);
			piramide.draw_solido(1,0,0);
			glPopMatrix();
		break;
		case 4:
			cubo.draw_solido_ajedrez(1,1,0,0,1,1);
		
			
			glPushMatrix();
			glTranslatef(0,5,0.0);
			obj_cargado.draw_solido_ajedrez(1,1,0,0,1,1);
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(3,0,0.0);
			glRotatef(-90,0,0,1);
			piramide.draw_solido_ajedrez(1,1,0,0,1,1);
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(-3,0,0.0);
			glRotatef(90,0,0,1);
			piramide.draw_solido_ajedrez(1,1,0,0,1,1);
			glPopMatrix();
		break;
		case 5:
			suelo_reflejos();
			
			reflejos();

			glDisable( GL_STENCIL_TEST );
			glEnable(GL_DEPTH_TEST);
			
			direction[0]=1;
			direction[1]=-1;
			direction[2]=1;
			carga_luz( posicion_luz_focal[0],  posicion_luz_focal[1], posicion_luz_focal[2],posicion_luz_puntual[0],posicion_luz_puntual[1],posicion_luz_puntual[2],direction);
			
			glPushMatrix();
			carga_materiales(1,1.0f,0.0f,0.0f); //materiales color rojo
			cubo.draw_solido_smooth();
			glPopMatrix();
			
			glPushMatrix();
			carga_materiales(1,0.0f,0.0f,0.0f); //materiales color negro
			glTranslatef(-5,-4,8);
			obj_cargado.draw_solido_smooth();
			glPopMatrix();
			
			glPushMatrix();
			glTranslatef(0,-4,8);
			obj_cargado.draw_solido_smooth();
			glPopMatrix();
			
			glPushMatrix();
			glTranslatef(5,-4,8);
			obj_cargado.draw_solido_smooth();
			glPopMatrix();
			//Hacer_Mascara_Sombra();
			
			
			
			//Terminar_Mascara_Sombra();
			glPushMatrix();
			glTranslatef(3,0,0.0);
			glRotatef(-90,0,0,1);
			piramide.draw_solido_smooth();
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(-3,0,0.0);
			glRotatef(90,0,0,1);
			piramide.draw_solido_smooth();
			glPopMatrix();
			
			glPushMatrix();
			carga_materiales(0,1.0f,1.0f,1.0f); //suelo color blanco
			glTranslatef(0.0,-7,0.0);
			suelo.draw_solido_smooth();
			glPopMatrix();
		
		break;
		case 6:
			
		
		break;
	}
}



void draw_scene(void)
{
	clear_window();
	change_observer();
	draw_axis();
	draw_objects();
	dibujar_texto();
	glutSwapBuffers();
}



//***************************************************************************
// Funcion llamada cuando se produce un cambio en el tamaño de la ventana
//
// el evento manda a la funcion:
// nuevo ancho
// nuevo alto
//***************************************************************************

void change_window_size(int Ancho1,int Alto1)
{
	change_projection();
	glViewport(0,0,Ancho1,Alto1);
	glutPostRedisplay();
}


//***************************************************************************
// Funcion llamada cuando se produce aprieta una tecla normal
//
// el evento manda a la funcion:
// codigo de la tecla
// posicion x del raton
// posicion y del raton
//***************************************************************************

void normal_keys(unsigned char Tecla1,int x,int y)
{

if (toupper(Tecla1)=='Q') exit(0);
if(toupper(Tecla1)=='1')
	if(act1==false){
		luz[0].activa(GL_LIGHT0);
		luz[0].ver_focal();	
		act1=true;
	}else
	{
		luz[0].desactiva(GL_LIGHT0);
		act1=false;
	}
if(toupper(Tecla1)=='2')
	if(act2==false){
		luz[1].activa(GL_LIGHT1);
		luz[1].ver_puntual();	
		act2=true;
	}else
	{
		luz[1].desactiva(GL_LIGHT1);
		act2=false;
	}
	if(act1==true){
		if(toupper(Tecla1)=='A'){
			posicion_luz_focal[0]-=0.5;
			luz[0].ver_focal();	
		}
		if(toupper(Tecla1)=='D'){
			posicion_luz_focal[0]+=0.5;
			luz[0].ver_focal();	
		}
		if(toupper(Tecla1)=='S'){
			posicion_luz_focal[1]-=0.5;
			luz[0].ver_focal();	
		}
		if(toupper(Tecla1)=='W'){
			posicion_luz_focal[1]+=0.5;
			luz[0].ver_focal();	
		}
		
		if(toupper(Tecla1)=='Z'){
			posicion_luz_focal[2]+=0.5;
			luz[0].ver_focal();	
		}
		
		if(toupper(Tecla1)=='X'){
			posicion_luz_focal[2]-=0.5;
			luz[0].ver_focal();	
		}
	}


	glutPostRedisplay();
}

//***************************************************************************
// Funcion llamada cuando se produce aprieta una tecla especial
//
// el evento manda a la funcion:
// codigo de la tecla
// posicion x del raton
// posicion y del raton

//***************************************************************************

void special_keys(int Tecla1,int x,int y)
{

switch (Tecla1){
	case GLUT_KEY_LEFT:Observer_angle_y--;break;
	case GLUT_KEY_RIGHT:Observer_angle_y++;break;
	case GLUT_KEY_UP:Observer_angle_x--;break;
	case GLUT_KEY_DOWN:Observer_angle_x++;break;
	case GLUT_KEY_PAGE_UP:Observer_distance*=1.2;break;
	case GLUT_KEY_PAGE_DOWN:Observer_distance/=1.2;break;
	case GLUT_KEY_F1:
		drawforma=1;
	break;
	case GLUT_KEY_F2:
		drawforma=2;
	break;
	case GLUT_KEY_F3:
		drawforma=3;
	break;
	case GLUT_KEY_F4:
		drawforma=4;
	break;
	case GLUT_KEY_F5:
		drawforma=5;
	break;
	case GLUT_KEY_F6:
		drawforma=6;
	break;
	}
glutPostRedisplay();
}

//***************************************************************************
// Funcion de incializacion
//***************************************************************************

void initialize(int argc, char**argv)
{
	bool tapa_sup=true,tapa_inf=true;
	int secciones=6;

			
	if(argc>1){///si se pasa un parametro para poner el nombre del .ply
		obj_cargado.leer_archivo(argv[1]); ///nombre del .py
		if(argc>2)
			if(strcmp(argv[2],"revolucionar")==0){ ///si quiere revolucionar el .ply en caso de que no se pueda fallará
					if(argc>2)///el numero de secciones que se quiere poner al .ply
						secciones=atoi(argv[3]);
					if(argc>3)///si se quiere dibujar la tapa de arriba o no
						if(argv[4][0]=='0')
							tapa_sup=false;
					if(argc>4)///si se quiere dibujar la tapa de abajo o no
						if(argv[5][0]=='0')
							tapa_inf=false;
				///revoluciono el perfil pasado
				obj_cargado.revolucionar_perfil(secciones,tapa_sup,tapa_inf);
			}
	}
	else{///sino se pasa parametro dibujo el perfil revolucionado con las tapas
		
		
		obj_cargado.leer_archivo("perfil");
		obj_cargado.revolucionar_perfil(secciones,tapa_sup,tapa_inf);
	}
	// se inicalizan la ventana y los planos de corte
	Window_width=5;
	Window_height=5;
	Front_plane=15;
	Back_plane=1000;

	// se inicia la posicion del observador, en el eje z
	Observer_distance=3*Front_plane;
	Observer_angle_x=3;
	Observer_angle_y=0;

	// se indica cual sera el color para limpiar la ventana	(r,v,a,al)
	// blanco=(1,1,1,1) rojo=(1,0,0,1), ...
	glClearColor(1,1,1,1);

	// se habilita el z-bufer
	glEnable(GL_DEPTH_TEST);
	//
	change_projection();
	//
	glViewport(0,0,UI_window_width,UI_window_height);
}


//***************************************************************************
// Programa principal
//
// Se encarga de iniciar la ventana, asignar las funciones e comenzar el
// bucle de eventos
//***************************************************************************

int main(int argc, char **argv)
{
	// se llama a la inicialización de glut
	glutInit(&argc, argv);
	
	// se indica las caracteristicas que se desean para la visualización con OpenGL
	// Las posibilidades son:
	// GLUT_SIMPLE -> memoria de imagen simple
	// GLUT_DOUBLE -> memoria de imagen doble
	// GLUT_INDEX -> memoria de imagen con color indizado
	// GLUT_RGB -> memoria de imagen con componentes rojo, verde y azul para cada pixel
	// GLUT_RGBA -> memoria de imagen con componentes rojo, verde, azul y alfa para cada pixel
	// GLUT_DEPTH -> memoria de profundidad o z-bufer
	// GLUT_STENCIL -> memoria de estarcido
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// posicion de la esquina inferior izquierdad de la ventana
	glutInitWindowPosition(UI_window_pos_x,UI_window_pos_y);
	
	// tamaño de la ventana (ancho y alto)
	glutInitWindowSize(UI_window_width,UI_window_height);
	
	// llamada para crear la ventana, indicando el titulo (no se visualiza hasta que se llama
	// al bucle de eventos)
	glutCreateWindow("Práctica 1");
	
	// asignación de la funcion llamada "dibujar" al evento de dibujo
	glutDisplayFunc(draw_scene);
	
	// asignación de la funcion llamada "cambiar_tamanio_ventana" al evento correspondiente
	glutReshapeFunc(change_window_size);
	// asignación de la funcion llamada "tecla_normal" al evento correspondiente
	glutKeyboardFunc(normal_keys);
	// asignación de la funcion llamada "tecla_Especial" al evento correspondiente
	glutSpecialFunc(special_keys);

	// funcion de inicialización
	initialize(argc,argv);
	// inicio del bucle de eventos
	glutMainLoop();
	return 0;
}
