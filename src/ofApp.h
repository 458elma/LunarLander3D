#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

// Elston Ma - CS134 - Final Project
// editted by Elston Ma for CS134 Final Project

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		// altitude sensor items
		bool onAGL = true;
		bool rayOctreeFromLander(ofVec3f &pointRet);
		bool landerOverMoon = false;
		float landerToMoon = 0.0;
		TreeNode landerNode;

		// check lander to moon collision
		void checkLanderCollision();
		vector<Box> landerBoxHolder;
		vector<TreeNode> collidedNodeHolder;
		bool landerCollided = false;
		float contactForce = 0.0;

		// lander integrator
		void landerIntegrate();
		// initial lander physics values
		// assume mass of lander to be 1 unit
		glm::vec3 landerVel = glm::vec3(0, 0, 0);
		glm::vec3 landerAcc = glm::vec3(0, 0, 0);
		glm::vec3 landerForces = glm::vec3(0, 0, 0);
		float landerDamp = 0.99;
		float landerRotVel = 0.0;
		float landerRotAcc = 0.0;
		float landerRotFor = 0.0;
		float landerRotDamp = 0.99;
		// sim start check
		bool startSim = false;
		// toggle gravity for testing purposes
		bool grav = true;

		// thruster access status
		bool usableThrust = true;

		// fuel setup
		float fuel = 60 * 120;
		bool countFuel = true;

		// sound setup
		ofSoundPlayer exhaustSound;
		bool exhLoaded = false;
		void playExh();
		bool exhPlay = false;
		float timeHolder = ofGetElapsedTimeMillis();
		ofSoundPlayer boomSound;
		bool boomLoaded = false;

		// lights setup
		ofLight zoneLight, areaLight, landerLight;

		// landing area checks
		bool inLandingArea = false;
		string landingIndicator = "Successful Landing: Not Landed Yet";

		// Exhaust emitter setup and shader setup
		void loadExhaustVbo();
		ParticleEmitter exhaust;
		TurbulenceForce* exhTurb;
		GravityForce* exhGrav;
		ImpulseRadialForce* exhRadi;
		ofTexture partTex;
		ofVbo exhVbo;
		ofShader shader;
		// Explosion emitter setup
		void loadBoomVbo();
		ParticleEmitter boom;
		TurbulenceForce* boomTurb;
		GravityForce* boomGrav;
		ImpulseRadialForce* boomRadi;
		ofVbo boomVbo;

		// cameras initialization
		ofCamera* viewCam;
		ofCamera trackCam;
		ofCamera fixedTrackCam;
		ofCamera landerCam;

		ofEasyCam cam;
		ofxAssimpModelLoader mars, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;


		ofxIntSlider numLevels;
		ofxPanel gui;

		// background image fields
		ofImage bkgImage;
		bool validBkg = false;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;

		bool runRayIntTest = false;
		
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;
};
