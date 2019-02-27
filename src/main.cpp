// CS247_Assignment1.cpp : Defines the entry point for the console application.
// Abdulellah Abualshour

#include <glew.h>
#include <glut.h>

#include <iostream>
#include <algorithm>

// ==========================================================================
// Declarations
int printOglError(char *file, int line);
void LoadData(char* filename);
void DownloadVolumeAsTexture();

unsigned short* data_array;
unsigned short vol_dim[3];
GLuint vol_texture;
int viewport_x, viewport_y;
int current_axis;
int current_slice[3];
bool data_loaded;

// ==========================================================================
// Handle of the window we're rendering to
static GLint window;

// ==========================================================================
// Handle OpenGL errors
int printOglError(char *file, int line)
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

#define printOpenGLError() printOglError((char *)__FILE__, __LINE__)

// ==========================================================================
// Parse GL_VERSION and return the major and minor numbers in the supplied
// integers.
// If it fails for any reason, major and minor will be set to 0.
// Assumes a valid OpenGL context.
void getGlVersion(int *major, int *minor)
{
	const char* verstr = (const char*)glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf_s(verstr, "%d.%d", major, minor) != 2)) {
		*major = *minor = 0;
		fprintf(stderr, "Invalid GL_VERSION format!!!\n");
	}
}

// ==========================================================================
// Cycle clear colors
static void NextClearColor(void)
{
	static int color = 0;

	switch (color++)
	{
	case 0:
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case 1:
		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		break;
	default:
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		color = 0;
		break;
	}
}

// ==========================================================================
// GLUT
static void display(void)
{
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ================================================================================
	// TODO: Draw with OpenGL
	// ================================================================================

	// hook the texture to GL
	glBindTexture(GL_TEXTURE_3D, vol_texture);

	// draw squares on same window
	float tex_coord[12];

	// Define the texture coordinates for YZ-Plane (around the X axis) (top left)
	float s = (float)current_slice[0] / (float)vol_dim[0]; // normalize the current_slice location to be between 0 and 1
	tex_coord[0] = s; //s
	tex_coord[1] = 0; //t
	tex_coord[2] = 1; //r
	tex_coord[3] = s; //s
	tex_coord[4] = 1; //t
	tex_coord[5] = 1; //r
	tex_coord[6] = s; //s
	tex_coord[7] = 1; //t
	tex_coord[8] = 0; //r
	tex_coord[9] = s; //s
	tex_coord[10] = 0; //t
	tex_coord[11] = 0; //r

					   // draw the texture for the first square
	glBegin(GL_QUADS);
	glTexCoord3f(tex_coord[0], tex_coord[1], tex_coord[2]);
	glVertex3f(-1.0, 1.0, 0.0);
	glTexCoord3f(tex_coord[3], tex_coord[4], tex_coord[5]);
	glVertex3f(-1.0, 0.0, 0.0);
	glTexCoord3f(tex_coord[6], tex_coord[7], tex_coord[8]);
	glVertex3f(-0.15, 0.0, 0.0);
	glTexCoord3f(tex_coord[9], tex_coord[10], tex_coord[11]);
	glVertex3f(-0.15, 1.0, 0.0);

	glEnd();

	// Define the texture coordinates for XZ-Plane (around the y axis) (top right)
	float t = (float)current_slice[1] / (float)vol_dim[1]; // normalize the current_slice location to be between 0 and 1
	tex_coord[0] = 0;
	tex_coord[1] = t;
	tex_coord[2] = 1;
	tex_coord[3] = 1;
	tex_coord[4] = t;
	tex_coord[5] = 1;
	tex_coord[6] = 1;
	tex_coord[7] = t;
	tex_coord[8] = 0;
	tex_coord[9] = 0;
	tex_coord[10] = t;
	tex_coord[11] = 0;

	// draw the texture for the second square
	glBegin(GL_QUADS);
	glTexCoord3f(tex_coord[0], tex_coord[1], tex_coord[2]);
	glVertex3f(0.01, 1.0, 1.0);
	glTexCoord3f(tex_coord[3], tex_coord[4], tex_coord[5]);
	glVertex3f(0.01, 0.0, 1.0);
	glTexCoord3f(tex_coord[6], tex_coord[7], tex_coord[8]);
	glVertex3f(1.0, 0.0, 1.0);
	glTexCoord3f(tex_coord[9], tex_coord[10], tex_coord[11]);
	glVertex3f(1.0, 1.0, 1.0);
	glEnd();

	// Define the texture coordinates for XY-Plane (around the z axis) (bottom)
	float r = (float)current_slice[2] / (float)vol_dim[2]; // normalize the current_slice location to be between 0 and 1
	tex_coord[0] = 0;
	tex_coord[1] = 1;
	tex_coord[2] = r;
	tex_coord[3] = 1;
	tex_coord[4] = 1;
	tex_coord[5] = r;
	tex_coord[6] = 1;
	tex_coord[7] = 0;
	tex_coord[8] = r;
	tex_coord[9] = 0;
	tex_coord[10] = 0;
	tex_coord[11] = r;

	// draw the texture for the third square
	glBegin(GL_QUADS);
	glTexCoord3f(tex_coord[0], tex_coord[1], tex_coord[2]);
	glVertex3f(-0.49, 0.0, 1.0);
	glTexCoord3f(tex_coord[3], tex_coord[4], tex_coord[5]);
	glVertex3f(-0.49, -1.0, 1.0);
	glTexCoord3f(tex_coord[6], tex_coord[7], tex_coord[8]);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord3f(tex_coord[9], tex_coord[10], tex_coord[11]);
	glVertex3f(1.0, 0.0, 1.0);
	glEnd();

	//unbind volume texture
	glBindTexture(GL_TEXTURE_3D, 0);

	glFlush();
	glutSwapBuffers();
}

static void play(void)
{
	int thisTime = glutGet(GLUT_ELAPSED_TIME);

	glutPostRedisplay();
}

static void key(unsigned char keyPressed, int x, int y)
{
	switch (keyPressed) {

	case 'w':
		current_slice[current_axis] = std::min((current_slice[current_axis] + 1), vol_dim[current_axis] - 1);
		fprintf(stderr, "increasing current slice: %i\n", current_slice[current_axis]);
		break;
	case 's':
		current_slice[current_axis] = std::max((current_slice[current_axis] - 1), 0);
		fprintf(stderr, "decreasing current slice: %i\n", current_slice[current_axis]);
		break;
	case 'a': // optional
		current_axis = ((current_axis + 1) % 3);
		fprintf(stderr, "toggling viewing axis to: %i\n", current_axis);
		break;
	case '1':
		LoadData("../../Datasets/lobster.dat");
		break;
	case '2':
		LoadData("../../Datasets/skewed_head.dat");
		break;
	case '3':
		LoadData("../../Datasets/dataset1.dat");
		break;
	case '4':
		LoadData("../../Datasets/dataset2.dat");
		break;
	case 'b':
		NextClearColor();
		break;
	default:
		fprintf(stderr, "\nKeyboard commands:\n\n"
			"b - Toggle among background clear colors\n"
			"w - Increase current slice\n"
			"s - Decrease current slice\n"
			"a - Toggle viewing axis\n"
			"1 - Load lobster dataset\n"
			"2 - Load head dataset\n");
		break;
	}
}

static void mouse(int button, int state, int x, int y) {}

static void motion(int x, int y) {}

static void reshape(int w, int h)
{
	float aspect = (float)w / (float)h;

	viewport_x = w;
	viewport_y = h;

	glViewport(0, 0, viewport_x, viewport_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, viewport_x, viewport_y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// for aspect ratio perservation
	float ratio = (float)vol_dim[0] / (float)vol_dim[1]; // this is our desired width to height ratio
	if (ratio <= aspect) { //  if the desired ratio is less than or equal to aspect, we adjust the left and right clipping planes (multiply by aspect)
		glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -10.0, 10.0);
	}
	else { // else, we divide the top and bottom by aspect
		glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -10.0, 10.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

static void special(int key, int x, int y)
{
	switch (key) {

	case GLUT_KEY_HOME:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;

	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;
	}
}

// ==========================================================================
// Data
void LoadData(char* filename)
{
	fprintf(stderr, "loading data %s\n", filename);

	FILE* fp;
	fopen_s(&fp, filename, "rb"); // open file, read only, binary mode 
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s for reading.\n", filename);
		return;
	}

	memset(vol_dim, 0, sizeof(unsigned short) * 3);

	//read volume dimension
	fread(&vol_dim[0], sizeof(unsigned short), 1, fp);
	fread(&vol_dim[1], sizeof(unsigned short), 1, fp);
	fread(&vol_dim[2], sizeof(unsigned short), 1, fp);

	fprintf(stderr, "volume dimensions: x: %i, y: %i, z:%i \n", vol_dim[0], vol_dim[1], vol_dim[2]);

	if (data_array != NULL) {
		delete[] data_array;
	}

	data_array = new unsigned short[vol_dim[0] * vol_dim[1] * vol_dim[2]]; //for intensity volume

	for (int z = 0; z < vol_dim[2]; z++) {
		for (int y = 0; y < vol_dim[1]; y++) {
			for (int x = 0; x < vol_dim[0]; x++) {

				fread(&data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])], sizeof(unsigned short), 1, fp);
				data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])] = data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])] << 4;
			}
		}
	}

	fclose(fp);

	current_slice[0] = vol_dim[0] / 2;
	current_slice[1] = vol_dim[1] / 2;
	current_slice[2] = vol_dim[2] / 2;

	DownloadVolumeAsTexture();

	data_loaded = true;
}

void DownloadVolumeAsTexture()
{
	fprintf(stderr, "downloading volume to 3D texture\n");

	// ================================================================================
	// TODO: Set up and download texture to GPU
	// ================================================================================

	// Enable 3D texture mode
	glEnable(GL_TEXTURE_3D);

	// Generate texture name
	glGenTextures(1, &vol_texture);

	// hook it to OpenGL
	glBindTexture(GL_TEXTURE_3D, vol_texture);

	// Define parameters
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // set the texture environment parameter to REPLACE
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //min filter to linear
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // mag filter to linear
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // wrap_s to clamp to edge
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // wrap_t to clamp to edge
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // wrap_r to clamp to edge

																		 // Now we download the texture to the GPU: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage3D.xhtml
	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, vol_dim[0], vol_dim[1], vol_dim[2], 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, (GLvoid *)data_array);

	// unbind at the end
	glBindTexture(GL_TEXTURE_3D, 0);

	std::cout << "Downloaded successfully.\n";

}

int main(int argc, char* argv[])
{
	data_array = NULL;
	viewport_x = 0;
	viewport_y = 0;
	current_slice[0] = 0;
	current_slice[1] = 0;
	current_slice[2] = 0;
	current_axis = 0;
	data_loaded = false;

	int gl_major, gl_minor;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	window = glutCreateWindow("AMCS/CS247 Scientific Visualization - Slice Viewer");

	glutIdleFunc(play);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutSpecialFunc(special);

	// Initialize the "OpenGL Extension Wrangler" library
	glewInit();

	// query and display OpenGL version
	getGlVersion(&gl_major, &gl_minor);
	printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);

	NextClearColor();

	// display help
	key('?', 0, 0);

	glutMainLoop();

	return 0;
}