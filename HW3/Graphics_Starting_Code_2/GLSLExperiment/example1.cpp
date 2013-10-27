// Two-Dimensional Sierpinski Gasket       
// Generated using randomly selected vertices and bisection

#include "Angel.h"
#include <math.h>
#include <stack>
//----------------------------------------------------------------------------
int width = 0;
int height = 0;
int numvertex;
int numface;
int index1 = 0;
int index = 0;
int order = 0;
int direction;
int flag = 0;
int Rflag = 0;
int mflag = 0;
int nflag = 0;
int pflag = 0;
int eflag = 0;
int len[5];
int iter[5];
int number = 0;
int numtreenode=0;
int numgroundpoints=0;
float xmax[2] ,xmin[2] ,ymax[2], ymin[2] ,zmax[2], zmin[2];
float movex = 0,movey = 0,movez = 0;
float scalex,scaley,scalez;
float ROTATE_Y = 0;
float step1 = 0;
float pulsestep=1;

float initX,initY,initZ;

char start[5][50000];
char F[5][100];

#define x_positive 1
#define x_negative 2
#define y_positive 3
#define y_negative 4
#define z_positive 5
#define z_negative 6
#define Rotate 20
#define normalL 7
#define pulse 8
#define cube 9
//GLuint vColor = glGetAttribLocation( program, "vColor" ); 
// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);
void readPLYfile(char *FileName);



typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[50000];
color4 colors[50000];
point4 vertices[50000];
point4 normal[50000];
point4 normalvertice[50000];
point4 normalcolors[50000];
point4 normal2[50000];
point4 groundpoints[1000];
point4 groundcolors[1000];
point4 spherepoints[50000];
point4 spherecolors[50000];
point4 cylinderpoints[50000];
point4 cylindercolors[50000];
point4 rot[5];
point4 currentPosition;
point4 currentAngle;
point4 currentDirection;



char LFile[5][20]={
	"lsys1.txt",
	"lsys2.txt",
	"lsys3.txt",
	"lsys4.txt"
	};


void generateGeometry( void )
{	
    //colorcube();

    // Create a vertex array object
	//readPLYfile("cow.ply");
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}
void do_iteration(int n,char stream[],int order){
	int count,i=0,j=0,l=0,m=0;
	char temp[50000];
	/*char *head = F[];
	char *head1 = temp;*/
	for(count=0;count<n;count++){
		i=0;
		j=0;
		m=0;
		while(stream[i] != 0){
			j=0;
			if(stream[i] == 'F'){
				while(F[order][j]!=NULL){
					temp[m] = F[order][j];
					j++;
					m++;
				}
				temp[m]='\0';
			}
			else if(stream[i] != 'F'){
				temp[m] = stream[i];
				j++;
				m++;
				temp[m]='\0';
			}
			i++;
		}
		memset(stream, 0, 256);
		l=strlen(temp);
		for(l=0;l<strlen(temp);l++){
			stream[l]=temp[l];
		}

	}
	
}
void iteration(){
	int i;
	for(i=0;i<4;i++){
		do_iteration(iter[i],start[i],i);
	}
}
void readLfile(int order){
	FILE *filestream;
	int i = 0;
	char line[256];
	if((filestream = fopen(LFile[order],"r") )== NULL)
		printf("file not exists");

	while(!feof(filestream)){
		memset(line,0,256);
		fscanf(filestream,"%s",line);

		if(strcmp(line,"len:") == 0)
			break;
	}

   fscanf(filestream,"%d",&len[order]);

   memset(line,0,256);
   fscanf(filestream,"%s",line);

   fscanf(filestream,"%d",&iter[order]);

   
   memset(line,0,256);
   fscanf(filestream,"%s",line);

   fscanf(filestream,"%f %f %f",&rot[order].x,&rot[order].y,&rot[order].z);

   
   memset(line,0,256);
   fscanf(filestream,"%s ",line);

   
   fscanf(filestream,"%c",start[order]);

   memset(line,0,256);
   fscanf(filestream,"%s",line);

   fscanf(filestream,"%s",F[order]);

  
   fclose(filestream);
}

void readdata(){
	int i;
	for(i=0;i<4;i++){
		readLfile(i);
	}
}


void readCylinder(char *FileName){
	FILE *plyfile;
	char line[256];
	int i=0,j=0;
	//int numvertex;
	//int numface;
	int num, one, two, three;
	float temp[3][3];
	float x,y,z;
	float c = 1.0;

	if((plyfile = fopen(FileName, "rt")) == NULL) // Open The File
	{
		printf("no such file exist!");
		exit(0);
	}

	for(i=0;i<6;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

    fscanf(plyfile,"%d",&numvertex);

	for(i=0;i<11;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

	fscanf(plyfile,"%d",&numface);

	for(i=0;i<6;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

	//for(i=0;i<numvertex;i++){
		//colors[i] = color4( 0.0, 0.0, 1.0, 1.0 );
	//}

	for(i=0;i<numvertex;i++){
		fscanf(plyfile,"%f %f %f", &z, &x, &y);
		vertices[i] = point4(x,y,z,c);
		if(i == 0){
		xmax[1] = x;
		xmin[1] = x;
		ymax[1] = y;
		ymin[1] = y;
		zmax[1] = z;
		zmin[1] = z;
		}
		else{ 
			if(x >= xmax[1])
				xmax[1] = x;
			else if(x <= xmin[1])
				xmin[1] = x;

			if(y >= ymax[1])
				ymax[1] = y;
			else if(y <= ymin[1])
				ymin[1] = y;

			if(z >= zmax[1])
				zmax[1] = z;
			else if(z <= zmin[1])
				zmin[1] = z;

		}
	}

	index1 = 0;
	number = 0;
	for(i=0;i<numface;i++){
		fscanf(plyfile,"%d",&num);
		if(num != 3)
			exit(0);
		fscanf(plyfile, "%d %d %d", &one, &two, &three);
		cylindercolors[index1] = color4(1.0,0.0,0.0,1.0);
		cylinderpoints[index1] = vertices[one];
		index1++;
		cylindercolors[index1] = color4(1.0,0.0,0.0,1.0);
		cylinderpoints[index1] = vertices[two];
		index1++;
		cylindercolors[index1] = color4(1.0,0.0,0.0,1.0);
		cylinderpoints[index1] = vertices[three];
		index1++;
	}
	fclose(plyfile);
}
void readSphere(char* FileName){
	FILE *plyfile;
	char line[256];
	int i=0,j=0;
	//int numvertex;
	//int numface;
	int num, one, two, three;
	float temp[3][3];
	float x,y,z;
	float c = 1.0;

	if((plyfile = fopen(FileName, "rt")) == NULL) // Open The File
	{
		printf("no such file exist!");
		exit(0);
	}

	for(i=0;i<6;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

    fscanf(plyfile,"%d",&numvertex);

	for(i=0;i<11;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

	fscanf(plyfile,"%d",&numface);

	for(i=0;i<6;i++){
		memset(line, 0, 256);
		fscanf(plyfile, "%s",line);
	}

	//for(i=0;i<numvertex;i++){
		//colors[i] = color4( 0.0, 0.0, 1.0, 1.0 );
	//}

	for(i=0;i<numvertex;i++){
		fscanf(plyfile,"%f %f %f", &x, &y, &z);
		vertices[i] = point4(x,y,z,c);
		if(i == 0){
		xmax[0] = x;
		xmin[0] = x;
		ymax[0] = y;
		ymin[0] = y;
		zmax[0] = z;
		zmin[0] = z;
		}
		else{ 
			if(x >= xmax[0])
				xmax[0] = x;
			else if(x <= xmin[0])
				xmin[0] = x;

			if(y >= ymax[0])
				ymax[0] = y;
			else if(y <= ymin[0])
				ymin[0] = y;

			if(z >= zmax[0])
				zmax[0] = z;
			else if(z <= zmin[0])
				zmin[0] = z;

		}
	}

	index = 0;
	number = 0;
	for(i=0;i<numface;i++){
		fscanf(plyfile,"%d",&num);
		if(num != 3)
			exit(0);
		fscanf(plyfile, "%d %d %d", &one, &two, &three);
		spherecolors[index] = color4(1.0,0.0,0.0,1.0);
		spherepoints[index] = vertices[one];
		index++;
		spherecolors[index] = color4(1.0,0.0,0.0,1.0);
		spherepoints[index] = vertices[two];
		index++;
		spherecolors[index] = color4(1.0,0.0,0.0,1.0);
		spherepoints[index] = vertices[three];
		index++;
	}

	fclose(plyfile);
}
void drawCylinder(){
	point4 temp = point4(0.0,1.0,0.0,1.0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	Angel::mat4 modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	modelMat = modelMat * Angel::Translate(currentPosition.x,currentPosition.y,currentPosition.z)*Angel::RotateY(currentAngle.y-90.0f)*Angel::RotateX(currentAngle.x)*Angel::RotateZ(currentAngle.z);
	//modelMat = modelMat * Angel::Scale(scalex,scaley,scalez);
	temp = modelMat*temp;
	currentPosition.x=temp.x;
	currentPosition.y=temp.y;
	currentPosition.z=temp.z;
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	 glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderpoints) + sizeof(cylindercolors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderpoints), cylinderpoints );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderpoints), sizeof(cylindercolors), cylindercolors );

	GLuint vColor = glGetAttribLocation( program, "vColor" );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(cylinderpoints)) );

	//readPLYfile("airplane.ply");
	glEnable( GL_DEPTH_TEST );
	glDrawArrays( GL_TRIANGLES, 0, index1 );
	glDisable( GL_DEPTH_TEST ); 
}
void drawGround(){
	int i;
	float x = -250.0;
	for(i=0;i<100;i++){
		groundpoints[numgroundpoints] = point4(x,0,-1000,1.0);
		groundpoints[numgroundpoints] = point4(0,1,0,1);
		numgroundpoints++;

		groundpoints[numgroundpoints] = point4(x,0,1000,1.0);
		groundpoints[numgroundpoints] = point4(0,1,0,1);
		numgroundpoints++;
		x+=5;

	}

	Angel::mat4 modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	modelMat = modelMat * Angel::Translate(0,0,0)*Angel::RotateY(0.0f) * Angel::RotateX(0.0f)*Angel::RotateZ(0.0f);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );

 glBufferData( GL_ARRAY_BUFFER, sizeof(groundpoints) + sizeof(groundcolors),
                  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(groundpoints), groundpoints );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(groundpoints), sizeof(groundcolors), groundcolors );
        glEnable(GL_DEPTH_TEST);
        glDrawArrays(GL_LINES,0,numgroundpoints);
        glDisable(GL_DEPTH_TEST);

}
void drawSphere( void )
{
	
	
	Angel::mat4 modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	modelMat = modelMat * Angel::Translate(currentPosition.x,currentPosition.y,currentPosition.z);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	

	glBufferData( GL_ARRAY_BUFFER, sizeof(spherepoints) + sizeof(spherecolors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(spherepoints), spherepoints );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(spherepoints), sizeof(spherecolors), spherecolors );

	GLuint vColor = glGetAttribLocation( program, "vColor" );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(spherepoints)) );

	//readPLYfile("airplane.ply");
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glEnable( GL_DEPTH_TEST );
	glDrawArrays( GL_TRIANGLES, 0, index );
	glDisable( GL_DEPTH_TEST ); 
}
void drawCube(void)
{
	// change to GL_FILL
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDisable( GL_DEPTH_TEST ); 
}

void drawNext(){
	nflag = 0;
	if(order < 42)
	order++;
	else 
		order = 0;
    display();
}
void normalize(){
	int i;
	float step;
	for(i=0;i<index1;i++){
		cylinderpoints[i].x = 0.1*cylinderpoints[i].x;
		cylinderpoints[i].y = 0.5*cylinderpoints[i].y + 0.5;
		cylinderpoints[i].z = 0.1*cylinderpoints[i].z;


	}
	
	

	step = (xmax[1]-xmin[1])/(xmax[0]-xmin[0]);

	for(i=0;i<index;i++){
		spherepoints[i].x = 0.1*step*spherepoints[i].x;
		spherepoints[i].y = 0.1*step*spherepoints[i].y;
		spherepoints[i].z = 0.1*step*spherepoints[i].z;
}
	

}
void InitPoint(){
	currentPosition.y = -20.0;
	currentPosition.x = -rand()%50;
    currentPosition.z = -rand()%500;

	currentAngle.x = 0.0;
	currentAngle.y = 0.0;
	currentAngle.z = 0.0;

	readCylinder("cylinder.ply");
	readSphere("sphere.ply");
	normalize();
    drawSphere();


};

void getPoint(){
	 InitPoint();

}
void drawtree(int count){
	int i = 0;
	InitPoint();
	stack<point4> treeposition;
	stack<point4> treeangle;
	while(start[count][i] != '\0'){
		switch(start[count][i]){
		case 'F':
			drawCylinder();
			drawSphere();
			break;
		case '+':
			currentAngle.x+=rot[count].x;
			break;
		case '-':
			currentAngle.x-=rot[count].x;
			break;
		case '&':
			currentAngle.y+=rot[count].y;
			break;
		case '^':
			currentAngle.y-=rot[count].y;
			break;
		case '\\':
			currentAngle.z+=rot[count].z;
			break;
		case '/':
			currentAngle.z-=rot[count].z;
			break;
		case '[':
			treeposition.push(currentPosition);
			treeangle.push(currentAngle);
			break;
		case ']':
			currentPosition = treeposition.top();
			currentAngle = treeangle.top();
			treeposition.pop();
			treeangle.pop();
			break;
		}
		i++;
	}
}
void drawforest(){
	 drawGround();
	drawtree(0);
	drawtree(1);
	drawtree(2);
	drawtree(3);
}
void drawPRE(){
	nflag = 0;
	if(order>0)
		order--;
	else 
		order = 42;
	display();
}


void keyboard( unsigned char key, int x, int y ){
	switch(key){
	case 'a':
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawtree(0);
		display();
		break;
	case 'b':
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawtree(1);
		display();
		break;
	case 'c':
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawtree(2);
		display();
		break;
	case 'd':
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawtree(3);
		display();
		break;
	case 'e':
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		drawforest();
		glFlush(); 
	glutSwapBuffers();
		break;
	}
}
void mydisplay(){
	
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)60.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 10000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	modelMat = modelMat * Angel::Translate(-(xmax[1]+xmin[1])/2+movex, -(ymax[1]+ymin[1])/2+movey, -sqrt((xmax[1]-xmin[1])*(xmax[1]-xmin[1])+(ymax[1]-ymin[1])*(ymax[1]-ymin[1])+(zmax[1]-zmin[1])*(zmax[1]-zmin[1]))+movez)*Angel::RotateX(0.0f)*Angel::RotateY(ROTATE_Y);
	//modelMat = modelMat * Angel::Scale(0.1,1,0.1);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawCylinder();
}

void display( void )
{
	/*glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	 drawtree();*/
        // clear the window

	
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)60.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 10000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);
	
    glFlush(); 
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

//----------------------------------------------------------------------------

// keyboard handler
//void keyboard( unsigned char key, int x, int y )
//{
//    switch ( key ) {
//	case 'W':
//		if(mflag == 0){
//			glClear(GL_COLOR_BUFFER_BIT);}
//		 flag = 0;
//         Rflag = 0;
//         mflag = 0;
//         nflag = 0;
//         pflag = 0;
//         eflag = 0;
//		 order = 0;
//		 direction = 9;
//		break;
//	case 'N':
//		if(mflag == 0){
//		glClear(GL_COLOR_BUFFER_BIT);
//		movex=0;
//		movey=0;
//		movez=0;
//		drawNext();}
//		break;
//	case 'P':
//		if(mflag == 0){
//		glClear(GL_COLOR_BUFFER_BIT);
//		movex=0;
//		movey=0;
//		movez=0;
//		drawPRE();}
//		break;
//	case 'X':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = x_positive;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'Y':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = y_positive;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'Z':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = z_positive;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'x':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = x_negative;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'y':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = y_negative;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'z':
//		if(flag == 0)
//			flag = 1;
//		else
//			flag = 0;
//		if(flag == 1){
//        direction = z_negative;
//		drawMove();}
//		else 
//			direction = 9;
//		break;
//	case 'R':
//		Rflag = 1;
//		flag = 2;
//		mflag = 1;
//		direction = 20;
//		drawMove();
//        break;
//	case 'm':
//		if(nflag == 0){
//			//glClear(GL_COLOR_BUFFER_BIT);
//			nflag = 1;
//		    display();
//		}
//		else if(nflag ==1){
//			//glClear(GL_COLOR_BUFFER_BIT);
//			nflag = 0;
//			display();
//		}
//		break;
//	case 'B':
//		if(pflag == 0)
//		pflag = 1;
//		else if(pflag == 1){
//			pflag = 0;
//		}
//			direction = 8;
//		drawMove();
//		break;
//	case 'e':
//		if(eflag == 0)
//			eflag =1;
//		else if(eflag == 1)
//			eflag = 0;
//		display();
//		break;
//    case 033:
//        exit( EXIT_SUCCESS );
//        break;
//    }
//}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
	/*readLfile("lsys4.txt");
	do_iteration(iter,start);*/
	readdata();
	iteration();
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 640;
	height = 640;
    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    
	// should run a test here 
	// with different cases
	// this is a good time to find out information about
	// your graphics hardware before you allocate any memory
    glutInitContextVersion( 3, 1 );
    glutInitContextProfile( GLUT_CORE_PROFILE );

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Color Cube" );

	// init glew
    glewInit();

    generateGeometry();

	// assign handlers
    glutDisplayFunc( display );
	//mydisplay() ;
 glutKeyboardFunc( keyboard );
	/*glutIdleFunc(drawMove);*/
	//glutIdleFunc(drawRotate);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
