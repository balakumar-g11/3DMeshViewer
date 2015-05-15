#include <iostream>
#include <string>
#include "include/app/CViewerApp.h"

#include <glut.h>
#include <gl/GL.h>
#include <glui.h>

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using namespace std;
using namespace MeshViewer::App::Viewer;

int lightSelect = 0;
int lightOnOff = 1;
int cameraType = 0;
int renderingType = 2;
int shadingType = 1;
int currButton = -1;
int boundBoxOption = 0;
int coordOption = 1;
int edgeOption = 0;
int groundOption = 1;
int lastPosX = 0;
int lastPosY = 0;
bool isLoaded(false);
CVector3D cLookAt(0, 0, 0);
CVector3D cEye(5, 3, 5);
CVector3D cEye3D(5, 3, 5);
CVector3D cEyeXZ(0.000001, 3, 0);
CVector3D cEyeYZ(5, 0, 0);
CVector3D cEyeXY(0, 0, 5);
GLUI_FileBrowser* fileBrowser;

GLUI_Scrollbar *redScroll, *greenScroll, *blueScroll;

GLfloat object_position[] = { 0.0, 0.0, 0.0 };

GLfloat object_rotation[] = { 1.0, 0.0, 0.0, 0.0,
								0.0, 1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								0.0, 0.0, 0.0, 1.0 };

GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0};
GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular_light[] = { 0.2, 0.2, 0.2, 1.0};
GLfloat lmodel_ambient[] = { 1.0, 0.1, 0.1, 1.0 };

void initRendering()
{
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
}

void Display()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cEye.x, cEye.y, cEye.z, cLookAt.x, cLookAt.y, cLookAt.z, 0, 1, 0);
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLineWidth(1.0f);
	if (groundOption == 1)
		CViewerApp::getInstance()->drawGround();

	if (coordOption == 1)
		CViewerApp::getInstance()->drawAxes();

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_light);
	glMateriali(GL_FRONT, GL_SHININESS, 100);

	if (isLoaded)
	{
		if (renderingType == 2)
			CViewerApp::getInstance()->showPolygonModel();
		else if (renderingType == 1)
			CViewerApp::getInstance()->showWireFrame();
		else if (renderingType == 0)
			CViewerApp::getInstance()->showPointCloud();
		else
		{
			CViewerApp::getInstance()->showPolygonModel();
			renderingType = 2;
		}
	}
		
	glutSwapBuffers();
}


void Reshape(int w, int h)
{
	GLUI_Master.auto_set_viewport();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)w / (GLdouble)h, 1.0, 100.0);
}

void mouseButton(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		currButton = button;
	}
	else
	{
		if (button == currButton)
		{
			currButton = -1;
		}
	}
	lastPosX = x;
	lastPosY = y;
}

void mouseMotion(int x, int y)
{
	int dx = x - lastPosX;
	int dy = y - lastPosY;

	double scale, length, theta;
	CVector3D cNEye1;
	CVector3D cNEye2;
	CVector3D cFar;
	CVector3D cIdent;
	CVector3D cRes;

	switch (currButton)
	{
	case GLUT_MIDDLE_BUTTON:
	{
		cFar = cLookAt - cEye;
		cIdent = CVector3D(0, 1, 0);

		scale = sqrt(cFar.length()) * 0.003;

		cRes = CVector3D::crossProduct(cFar, cIdent);
		cIdent = CVector3D::crossProduct(cRes, cFar);

		double mag = cRes.length();
		cRes = cRes / mag;

		mag = cIdent.length();
		cIdent = cIdent / mag;

		cEye.x += -cRes.x * dx * scale + cIdent.x * dy * scale;
		cEye.y += -cRes.y * dx * scale + cIdent.y * dy * scale;
		cEye.z += -cRes.z * dx * scale + cIdent.z * dy * scale;

		cLookAt.x += -cRes.x * dx * scale + cIdent.x * dy * scale;
		cLookAt.y += -cRes.y * dx * scale + cIdent.y * dy * scale;
		cLookAt.z += -cRes.z * dx * scale + cIdent.z * dy * scale;

		break;
	}
	case GLUT_RIGHT_BUTTON:
	{
		cFar = cLookAt - cEye;

		length = cFar.length();
		cFar = cFar / length;

		length -= sqrt(length)*dx*0.03;

		cEye.x = cLookAt.x - length * cFar.x;
		cEye.y = cLookAt.y - length * cFar.y;
		cEye.z = cLookAt.z - length * cFar.z;

		if (length < 1)
		{
			cLookAt = cEye + cFar;
		}

		break;
	}
	case GLUT_LEFT_BUTTON:
	{
		if (cameraType != CameraType::CAMERA_3D)
			break;

		cNEye1 = cEye - cLookAt;

		theta = -dx * 0.003;

		cNEye2.x = (double)cos(theta) * cNEye1.x + (double)sin(theta) * cNEye1.z;
		cNEye2.y = cNEye1.y;
		cNEye2.z = -(double)sin(theta) * cNEye1.x + (double)cos(theta) * cNEye1.z;

		theta = -dy * 0.003;

		cFar.x = -cNEye2.x;
		cFar.y = -cNEye2.y;
		cFar.z = -cNEye2.z;
		cIdent = CVector3D(0, 1, 0);

		cRes = CVector3D::crossProduct(cFar, cIdent);
		cIdent = CVector3D::crossProduct(cRes, cFar);

		length = cFar.length();
		cFar = cFar / length;

		double mag = cIdent.length();
		cIdent = cIdent / mag;

		cNEye1.x = length * ((double)cos(theta) * cFar.x + (double)sin(theta) * cIdent.x);
		cNEye1.y = length * ((double)cos(theta) * cFar.y + (double)sin(theta) * cIdent.y);
		cNEye1.z = length * ((double)cos(theta) * cFar.z + (double)sin(theta) * cIdent.z);

		cEye = cLookAt - cNEye1;

		break;
	}
	}

	lastPosX = x;
	lastPosY = y;

	glutPostRedisplay();
}

void lightSelected(int optionGroup)
{
	switch (lightSelect)
	{
	case 0:
		redScroll->set_float_val(lmodel_ambient[0]);
		greenScroll->set_float_val(lmodel_ambient[1]);
		blueScroll->set_float_val(lmodel_ambient[2]);
		break;
	case 1:
		redScroll->set_float_val(white_light[0]);
		greenScroll->set_float_val(white_light[1]);
		blueScroll->set_float_val(white_light[2]);
		break;
	case 2:
		redScroll->set_float_val(specular_light[0]);
		greenScroll->set_float_val(specular_light[1]);
		blueScroll->set_float_val(specular_light[2]);
		break;
	default:
		break;
	}
}

void lightingOptions(int optionGroup)
{
	switch (optionGroup)
	{
	case 1:
		CViewerApp::getInstance()->LightOnOff(lightOnOff);
	case 2:
		if (lightSelect == 0)
			lmodel_ambient[0] = redScroll->get_float_val();
		else if (lightSelect == 1)
			white_light[0] = redScroll->get_float_val();
		else if (lightSelect == 2)
			specular_light[0] = redScroll->get_float_val();
		break;
	case 3:
		if (lightSelect == 0)
			lmodel_ambient[1] = greenScroll->get_float_val();
		else if (lightSelect == 1)
			white_light[1] = greenScroll->get_float_val();
		else if (lightSelect == 2)
			specular_light[1] = greenScroll->get_float_val();
		break;
	case 4:
		if (lightSelect == 0)
			lmodel_ambient[2] = blueScroll->get_float_val();
		else if (lightSelect == 1)
			white_light[2] = blueScroll->get_float_val();
		else if (lightSelect == 2)
			specular_light[2] = blueScroll->get_float_val();
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void shadingTypeControl(int optionGroup)
{
	CViewerApp::getInstance()->changeShadingType(shadingType);
}

void updateRotation()
{
	CViewerApp::getInstance()->m_bRotation[0] = object_rotation[0];
	CViewerApp::getInstance()->m_bRotation[1] = object_rotation[1];
	CViewerApp::getInstance()->m_bRotation[2] = object_rotation[2];
	CViewerApp::getInstance()->m_bRotation[3] = object_rotation[3];
	CViewerApp::getInstance()->m_bRotation[4] = object_rotation[4];
	CViewerApp::getInstance()->m_bRotation[5] = object_rotation[5];
	CViewerApp::getInstance()->m_bRotation[6] = object_rotation[6];
	CViewerApp::getInstance()->m_bRotation[7] = object_rotation[7];
	CViewerApp::getInstance()->m_bRotation[8] = object_rotation[8];
	CViewerApp::getInstance()->m_bRotation[9] = object_rotation[9];
	CViewerApp::getInstance()->m_bRotation[10] = object_rotation[10];
	CViewerApp::getInstance()->m_bRotation[11] = object_rotation[11];
	CViewerApp::getInstance()->m_bRotation[12] = object_rotation[12];
	CViewerApp::getInstance()->m_bRotation[13] = object_rotation[13];
	CViewerApp::getInstance()->m_bRotation[14] = object_rotation[14];
	CViewerApp::getInstance()->m_bRotation[15] = object_rotation[15];
}

void translationOption(int optionGroup)
{
	if (!isLoaded)
		return;
	
	switch (optionGroup)
	{
	case 1:
		CViewerApp::getInstance()->m_bTranslation.x = object_position[0];
		break;
	case 2:
		CViewerApp::getInstance()->m_bTranslation.y = object_position[1];
		break;
	case 3:
		CViewerApp::getInstance()->m_bTranslation.z = object_position[2];
		break;
	case 4:
		updateRotation();
		break;
	default:
		break;
	}

	glutPostRedisplay();

}

void boundingBoxOptions(int optionGroup)
{
	if (optionGroup == 1)
		CViewerApp::getInstance()->ShowBoundingBox(boundBoxOption);
	else if (optionGroup == 2)
		CViewerApp::getInstance()->ShowEdges(edgeOption);
}

void cameraOptionsControl(int optionGroup)
{
	switch (cameraType)
	{
	case CameraType::CAMERA_3D:
		cEye = cEye3D;
		break;
	case CameraType::CAMERA_XZ:
		cEye = cEyeXZ;
		break;
	case CameraType::CAMERA_YZ:
		cEye = cEyeYZ;
		break;
	case CameraType::CAMERA_XY:
		cEye = cEyeXY;
		break;
	default:
		cEye = cEye3D;
		cameraType = CameraType::CAMERA_3D;
		break;
	}

	glutPostRedisplay();
}


void fileBrowserControl(int optionGroup)
{
	string fileName = fileBrowser->get_file();
	string delim = ".";
	
	string token = fileName.substr(fileName.find(delim), fileName.size());

	if (token.compare(".m") != 0)
		return;

	isLoaded = false;

	CViewerApp::getInstance()->loadFile(fileName);

	isLoaded = true;

	glutPostRedisplay();
}

int main()
{
	//Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	//Create Window
	int glutWindow = glutCreateWindow("MeshViewer");
	initRendering();
	
	GLUI *gluiWindow = GLUI_Master.create_glui_subwindow(glutWindow, GLUI_SUBWINDOW_RIGHT);
	GLUI_Master.set_glutDisplayFunc(Display);
	GLUI_Master.set_glutReshapeFunc(Reshape);
	GLUI_Master.set_glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	GLUI *bottomWindow = GLUI_Master.create_glui_subwindow(glutWindow, GLUI_SUBWINDOW_BOTTOM);
	
	GLUI_Panel* filePanel = gluiWindow->add_panel("Open File");

	fileBrowser = new GLUI_FileBrowser(filePanel, "", false, 1, fileBrowserControl);

	gluiWindow->add_separator();
		
	GLUI_Panel *optionsPanel = gluiWindow->add_panel("Object Display");

	GLUI_Panel *renderPanel = gluiWindow->add_panel_to_panel(optionsPanel, "Render Type");
	
	GLUI_RadioGroup *renderType = gluiWindow->add_radiogroup_to_panel(renderPanel, &renderingType);

	GLUI_RadioButton *point = gluiWindow->add_radiobutton_to_group(renderType, "Points");
	GLUI_RadioButton *wireframe = gluiWindow->add_radiobutton_to_group(renderType, "Wireframe");
	GLUI_RadioButton *fill = gluiWindow->add_radiobutton_to_group(renderType, "Fill");

	gluiWindow->add_separator_to_panel(optionsPanel);

	GLUI_Panel *shadePanel = gluiWindow->add_panel_to_panel(optionsPanel, "Shading Type");

	GLUI_RadioGroup *shadeType = gluiWindow->add_radiogroup_to_panel(shadePanel, &shadingType, 2, shadingTypeControl);

	GLUI_RadioButton *flat = gluiWindow->add_radiobutton_to_group(shadeType, "Flat");
	GLUI_RadioButton *smooth = gluiWindow->add_radiobutton_to_group(shadeType, "Smooth");

	gluiWindow->add_separator_to_panel(optionsPanel);
	
	GLUI_Rollout* rollOut = gluiWindow->add_rollout_to_panel(optionsPanel, "Display Options", 0, 1);

	GLUI_Checkbox* boundingBox = new GLUI_Checkbox(rollOut, "AABB", &boundBoxOption, 1, boundingBoxOptions);
	GLUI_Checkbox* coordinateAxes = new GLUI_Checkbox(rollOut, "Co-ordinates", &coordOption);
	GLUI_Checkbox* ground = new GLUI_Checkbox(rollOut, "Ground", &groundOption);
	GLUI_Checkbox* traiangulation = new GLUI_Checkbox(rollOut, "Edges", &edgeOption, 2, boundingBoxOptions);
	
	GLUI_Panel *cameraPanel = bottomWindow->add_panel("Camera Options");

	GLUI_RadioGroup *cameraControl = bottomWindow->add_radiogroup_to_panel(cameraPanel, &cameraType, 1, cameraOptionsControl);

	GLUI_RadioButton *perspective = bottomWindow->add_radiobutton_to_group(cameraControl, "Perspective");
	GLUI_RadioButton *xy = bottomWindow->add_radiobutton_to_group(cameraControl, "XZ");
	GLUI_RadioButton *yz = bottomWindow->add_radiobutton_to_group(cameraControl, "YZ");
	GLUI_RadioButton *zx = bottomWindow->add_radiobutton_to_group(cameraControl, "XY");

	bottomWindow->add_column();
	
	GLUI_Panel *lightPanel = bottomWindow->add_panel("Light Options");

	GLUI_Panel* ltPosPanel = bottomWindow->add_panel_to_panel(lightPanel, "Position");

	GLUI_Translation* lightX = bottomWindow->add_translation_to_panel(ltPosPanel, "Light X", GLUI_TRANSLATION_X, &light_position[0]);
	lightX->set_speed(0.1);
	bottomWindow->add_column_to_panel(ltPosPanel);
	GLUI_Translation* lightY = bottomWindow->add_translation_to_panel(ltPosPanel, "Light Y", GLUI_TRANSLATION_Y, &light_position[1]);
	lightY->set_speed(0.1);
	bottomWindow->add_column_to_panel(ltPosPanel);
	GLUI_Translation* lightZ = bottomWindow->add_translation_to_panel(ltPosPanel, "Light Z", GLUI_TRANSLATION_Z, &light_position[2]);
	lightZ->set_speed(0.1);

	bottomWindow->add_column_to_panel(lightPanel);

	GLUI_Checkbox* lighting = new GLUI_Checkbox(lightPanel, "Light On/Off", &lightOnOff, 1, lightingOptions);

	bottomWindow->add_separator_to_panel(lightPanel);

	GLUI_RadioGroup* lightGroup = bottomWindow->add_radiogroup_to_panel(lightPanel, &lightSelect, 1, lightSelected);

	GLUI_RadioButton *ambience = bottomWindow->add_radiobutton_to_group(lightGroup, "Ambience");
	GLUI_RadioButton *diffuse = bottomWindow->add_radiobutton_to_group(lightGroup, "Diffuse");
	GLUI_RadioButton *specular = bottomWindow->add_radiobutton_to_group(lightGroup, "Specular");

	bottomWindow->add_column_to_panel(lightPanel);
	
	GLUI_StaticText* redText = bottomWindow->add_statictext_to_panel(lightPanel, "Red : ");
	redScroll = new GLUI_Scrollbar(lightPanel, "Red", GLUI_SCROLL_HORIZONTAL, GLUI_SCROLL_FLOAT, 2, lightingOptions);
	redScroll->set_float_limits(0.0, 1.0);
	redScroll->set_float_val(lmodel_ambient[0]);

	GLUI_StaticText* greenText = bottomWindow->add_statictext_to_panel(lightPanel, "Green : ");
	greenScroll = new GLUI_Scrollbar(lightPanel, "Green", GLUI_SCROLL_HORIZONTAL, GLUI_SCROLL_FLOAT, 3, lightingOptions);
	greenScroll->set_float_limits(0.0, 1.0);
	greenScroll->set_float_val(lmodel_ambient[1]);

	GLUI_StaticText* blueText = bottomWindow->add_statictext_to_panel(lightPanel, "Blue : ");
	blueScroll = new GLUI_Scrollbar(lightPanel, "Blue", GLUI_SCROLL_HORIZONTAL, GLUI_SCROLL_FLOAT, 4, lightingOptions);
	blueScroll->set_float_limits(0.0, 1.0);
	blueScroll->set_float_val(lmodel_ambient[2]);

	bottomWindow->add_column();

	GLUI_Panel *translatePanel = bottomWindow->add_panel("Object Translation");
	GLUI_Translation* objectX = bottomWindow->add_translation_to_panel(translatePanel, "Translate X", GLUI_TRANSLATION_X, &object_position[0], 1, translationOption);
	objectX->set_speed(0.005);
	bottomWindow->add_column_to_panel(translatePanel);
	GLUI_Translation* objectY = bottomWindow->add_translation_to_panel(translatePanel, "Translate Y", GLUI_TRANSLATION_Y, &object_position[1], 2, translationOption);
	objectY->set_speed(0.005);
	bottomWindow->add_column_to_panel(translatePanel);
	GLUI_Translation* objectZ = bottomWindow->add_translation_to_panel(translatePanel, "Translate Z", GLUI_TRANSLATION_Z, &object_position[2], 3, translationOption);
	objectZ->set_speed(0.005);
	bottomWindow->add_column_to_panel(translatePanel);
	GLUI_Rotation* rotation = bottomWindow->add_rotation_to_panel(translatePanel, "Rotation", object_rotation, 4, translationOption);

	gluiWindow->set_main_gfx_window(glutWindow);
	bottomWindow->set_main_gfx_window(glutWindow);

	GLUI_Master.auto_set_viewport();
	
	//Continue the loop
	glutMainLoop();

	CViewerApp::deleteInstance();

	return EXIT_SUCCESS;
}

