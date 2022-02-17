
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   Elston Ma
//  Date: 17 May 2021


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>

#define RESTITUTION 0.40
#define UP_THRUST 15
#define OTHER_THRUST 12

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
// editted by Elston Ma for CS134 Final Project
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();

	// other cameras setup
	trackCam.setPosition(0, 150, -250);
	trackCam.lookAt(glm::vec3(0, 0, 0));

	fixedTrackCam.setPosition(0, 150, 0);
	fixedTrackCam.lookAt(glm::vec3(0, 0, 0));

	landerCam.setPosition(0, 30, 0);
	landerCam.lookAt(glm::vec3(0, 0, 0));

	viewCam = &cam;

	// load texture
	ofDisableArbTex();
	if (!ofLoadImage(partTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}
	// load shader
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif
	// exhaust forces
	exhTurb = new TurbulenceForce(ofVec3f(-3, -3, -3), ofVec3f(3, 3, 3));
	exhGrav = new GravityForce(ofVec3f(0, -1.64, 0));
	exhRadi = new ImpulseRadialForce(50);
	exhRadi->setHeight(0.1);
	// exhaust emitter setup
	exhaust.sys->addForce(exhTurb);
	exhaust.sys->addForce(exhGrav);
	exhaust.sys->addForce(exhRadi);
	exhaust.setPosition(ofVec3f(0, 10, 0));
	exhaust.setVelocity(ofVec3f(0, -15, 0));
	exhaust.setEmitterType(DirectionalEmitter);
	exhaust.setGroupSize(50);
	exhaust.setLifespan(1);
	exhaust.setOneShot(true);
	exhaust.setParticleRadius(5);
	//exhaust.start();

	// explosion forces
	boomTurb = new TurbulenceForce(ofVec3f(-3, -3, -3), ofVec3f(3, 3, 3));
	boomGrav = new  GravityForce(ofVec3f(0, -1.64, 0));
	boomRadi = new ImpulseRadialForce(400);
	boomRadi->setHeight(1);
	boom.sys->addForce(boomTurb);
	boom.sys->addForce(boomGrav);
	boom.sys->addForce(boomRadi);
	boom.setPosition(ofVec3f(20, 10, 0));
	boom.setVelocity(ofVec3f(0, 0, 0));
	boom.setEmitterType(RadialEmitter);
	boom.setGroupSize(750);
	boom.setLifespan(2);
	boom.setOneShot(true);
	boom.setParticleRadius(5);

	ofEnableSmoothing();
	ofEnableDepthTest();

	// background image setup
	if (bkgImage.load("images/FinPro_starfield.png")) {
		validBkg = true;
	}

	// load sounds
	if (exhaustSound.load("sounds/ExhaustSound.wav")) {
		exhLoaded = true;
	}
	if (boomSound.load("sounds/BoomSound.wav")) {
		boomLoaded = true;
	}

	// setup rudimentary lighting 
	//
	//initLightingAndMaterials();
	zoneLight.setup();
	zoneLight.enable();
	//zoneLight.disable();
	zoneLight.setSpotlight();
	zoneLight.setScale(0.05);
	zoneLight.setSpotlightCutOff(40);
	zoneLight.setAttenuation(.2, .001, .001);
	zoneLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	zoneLight.setDiffuseColor(ofFloatColor(0.3, 0.3, 0.3));
	zoneLight.setSpecularColor(ofFloatColor(0.6, 0.6, 0.6));
	zoneLight.rotate(-75,  ofVec3f(1, 0, 0));
	zoneLight.setPosition(0, 45, 20);

	areaLight.setup();
	areaLight.enable();
	//areaLight.disable();
	areaLight.setAreaLight(5, 5);
	areaLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	areaLight.setDiffuseColor(ofFloatColor(0.8, 0.8, 0.8));
	areaLight.setSpecularColor(ofFloatColor(1, 1, 1));
	areaLight.setPosition(0, 150, 0);

	landerLight.setup();
	landerLight.enable();
	//landerLight.disable();
	landerLight.setSpotlight();
	landerLight.setScale(0.05);
	landerLight.setSpotlightCutOff(20);
	landerLight.setAttenuation(.2, .001, .001);
	landerLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	landerLight.setDiffuseColor(ofFloatColor(0.5, 0.5, 0.5));
	landerLight.setSpecularColor(ofFloatColor(0.8, 0.8, 0.8));
	landerLight.rotate(-90, ofVec3f(1, 0, 0));
	landerLight.setPosition(100, 20, 100);

	mars.loadModel("geo/moon-houdini.obj");
	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = true;

	//  Create Octree for testing.
	//
	float beforeCreation = ofGetElapsedTimeMillis();
	octree.create(mars.getMesh(0), 20);
	
	cout << "Time taken to build Octree: " 
		<< ofGetElapsedTimeMillis() - beforeCreation << " milliseconds" << endl;

	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;
	

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

}

// developed by Elston Ma for CS134 Final Project
void ofApp::landerIntegrate() {
	float framerate = ofGetFrameRate();
	if (framerate < 1.0) return;

	float dt = 1.0 / framerate;

	glm::vec3 landerPos = lander.getPosition();
	glm::vec3 landerPosUpdate = landerPos + (landerVel * dt);
	lander.setPosition(landerPosUpdate.x, landerPosUpdate.y, landerPosUpdate.z);

	glm::vec3 accel = landerAcc;
	accel += landerForces;
	landerVel += accel * dt;

	landerVel *= landerDamp;

	float landerRot = lander.getRotationAngle(1);
	float landerRotUpdate = landerRot + (landerRotVel * dt);
	lander.setRotation(1, landerRotUpdate, 0, 1, 0);

	float rotAccel = landerRotAcc;
	rotAccel += landerRotFor;
	landerRotVel += rotAccel * dt;

	landerRotVel *= landerRotDamp;

	landerRotFor = 0.0;
	landerForces = glm::vec3(0, 0, 0);
}

// load vertex buffer for rendering exhaust particles
// developed by Elston Ma for CS134 Final Project
void ofApp::loadExhaustVbo() {
	if (exhaust.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < exhaust.sys->particles.size(); i++) {
		points.push_back(exhaust.sys->particles[i].position);
		sizes.push_back(ofVec3f(exhaust.particleRadius));
	}

	int total = (int)points.size();
	exhVbo.clear();
	exhVbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	exhVbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}
// load vertex buffer for rendering explosion particles
// developed by Elston Ma for CS134 Final Project
void ofApp::loadBoomVbo() {
	if (boom.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < boom.sys->particles.size(); i++) {
		points.push_back(boom.sys->particles[i].position);
		sizes.push_back(ofVec3f(boom.particleRadius));
	}

	int total = (int)points.size();
	boomVbo.clear();
	boomVbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	boomVbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
// developed by Elston Ma for CS134 Final Project
void ofApp::update() {
	// update exhaust emitter always
	exhaust.update();
	// update explosion emitter always
	boom.update();

	// check on sound
	if (exhPlay) {
		playExh();
	}

	// check if lander is loaded to point tracking camera on lander
	// track camera will be oriented on the back of lander
	// track camera will follow movement of lander
	// track camera will be thrusted and rotated with the lander
	// so that lander will always be oriented in same position
	// relative to camera so direction of controls remain consistent
	// in track camera view
	if (bLanderLoaded) {
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0),
			glm::radians(lander.getRotationAngle(1)), glm::vec3(0, 1, 0));
		glm::vec3 posLEM = lander.getPosition();
		glm::vec3 trackPos = glm::vec3(posLEM.x, posLEM.y + 10, posLEM.z - 50);

		glm::vec3 rotVec = trackPos - posLEM;

		glm::vec3 rotLEM = (glm::vec3)(rotMat * glm::vec4(rotVec, 0));

		glm::vec3 finalTrackPos = posLEM + rotLEM;
		// camera that follows lander
		trackCam.setPosition(finalTrackPos.x, finalTrackPos.y, finalTrackPos.z);
		trackCam.lookAt(posLEM);

		// camera that tracks the lander from a fixed point
		fixedTrackCam.lookAt(posLEM);
		// onboard lander camera, sees what is on the bottom of lander
		// zoomed view, destination appears closer than actual
		landerCam.setPosition(posLEM.x, posLEM.y, posLEM.z);
		landerCam.lookAt(glm::vec3(posLEM.x, posLEM.y - 1, posLEM.z));

		// have lander light follow the lander to give it consistent lighting
		landerLight.setPosition(posLEM.x, posLEM.y + 20, posLEM.z);

		// attach exhaust emitter to lander if loaded
		exhaust.setPosition(lander.getPosition());
	} else {
		trackCam.setPosition(0, 150, -250);
		trackCam.lookAt(glm::vec3(0, 0, 0));

		fixedTrackCam.lookAt(glm::vec3(0, 0, 0));

		landerCam.setPosition(0, 30, 0);
		landerCam.lookAt(glm::vec3(0, 0, 0));
	}
	// only integrate physics to lander if loaded and in simulation start mode
	if (bLanderLoaded && startSim) {
		// turn thrusters off if out of fuel
		if (fuel <= 0 && countFuel) {
			usableThrust = false;
		}

		// constant gravity force pulling down on lander
		// can be toggled on or off for testing
		if (grav) landerForces += glm::vec3(0, -1.64, 0);

		// check for lander collision with moon
		checkLanderCollision();

		// turbulence forces for fun but only when off the ground
		if (!landerCollided) {
			landerForces += glm::vec3(ofRandom(-2, 2), ofRandom(-2, 2), ofRandom(-2, 2));
		}

		if (landerCollided && landerVel.y < 0) {
			// apply impulse force if collision detected
			// and moving downward

			// kill thrusters when contact made with terrain
			usableThrust = false;

			// stored all intersected nodes in a vector when checking for collisions
			// use first node in vector as the reference point to get the normal vector
			// needed for the impulse equation, this is sufficient because the group of points
			// colliding with the lander should be roughly at the same angle
			glm::vec3 norm = octree.mesh.getNormal(collidedNodeHolder[0].points[0]);

			glm::vec3 bounceF = (RESTITUTION + 1.0) *
				((-glm::dot(landerVel, norm)) * norm);

			contactForce = glm::length(bounceF);

			landerForces += ofGetFrameRate() * bounceF;

			if (contactForce > 4.25) {
				boom.setPosition(lander.getPosition());
				boom.sys->reset();
				boom.start();

				// play the boom sound
				boomSound.play();

				// additional explosion force added if explosion triggered
				// will cover up the impulse force
				landerForces += glm::vec3(400, 400, 400);
				landingIndicator = "Successful Landing: False";
			}

			// stop sim if lander is firmly on ground
			if (contactForce < 0.09) { 
				startSim = false;

				// check for valid landing pos when lander rests on ground
				glm::vec3 lPos = lander.getPosition();
				bool xLanding = lPos.x < 22 && lPos.x > -22;
				bool zLanding = lPos.z < 22 && lPos.z > -22;
				if (xLanding && zLanding) {
					landingIndicator = "Successful Landing: True";
				}
				else {
					landingIndicator = "Successful Landing: False";
				}
			}
		}

		//if (grav && landerCollided) landerForces += glm::vec3(0, 1.61, 0);

		landerIntegrate();

		// check altitude of lander
		if (onAGL) {
			ofVec3f landerXMoon;
			rayOctreeFromLander(landerXMoon);
			if (landerOverMoon) {
				glm::vec3 thePt = octree.mesh.getVertex(landerNode.points[0]);
				glm::vec3 landPos = lander.getPosition();
				glm::vec3 distVec = landPos - thePt;
				landerToMoon = glm::length(distVec);
			}
		}
	}
}

// sound playing function
// developed by Elston Ma for CS134 Final Project
void ofApp::playExh() {
	float aTime = ofGetElapsedTimeMillis();
	if ((aTime - timeHolder) > (200)) {
		exhaustSound.play();
		timeHolder = aTime;
	}
}

//--------------------------------------------------------------
// editted by Elston Ma for CS134 Final Project
void ofApp::draw() {
	loadExhaustVbo();
	loadBoomVbo();

	if (validBkg) {
		glDepthMask(false);
		ofSetColor(255, 255, 255, 255);
		bkgImage.draw(0, 0);
		glDepthMask(true);
	} else {
		ofBackground(ofColor::black);
	}

	//cam.begin();
	viewCam->begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		//cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}

	ofPopMatrix();
	//cam.end();
	viewCam->end();

	// drawing shaded particles
	glDepthMask(GL_FALSE);
	ofSetColor(255, 0, 0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	shader.begin();
	viewCam->begin();

	exhaust.draw();
	partTex.bind();
	exhVbo.draw(GL_POINTS, 0, (int)exhaust.sys->particles.size());
	partTex.unbind();

	boom.draw();
	partTex.bind();
	boomVbo.draw(GL_POINTS, 0, (int)boom.sys->particles.size());
	partTex.unbind();

	viewCam->end();
	shader.end();
	ofDisablePointSprites();
	ofDisableBlendMode();
	glDepthMask(GL_TRUE);

	glDepthMask(false);
	if (!bHide) gui.draw();
	ofSetColor(255, 0, 0, 255);
	string started = "Simulation: ";
	if (startSim) {
		started += "ON";
	}
	else {
		started += "OFF";
	}
	ofDrawBitmapString(started, ofPoint(ofGetWindowWidth() - 150, 15));
	string moveCam = "Move EasyCam: ";
	if (cam.getMouseInputEnabled()) {
		moveCam += "ON";
	}
	else {
		moveCam += "OFF";
	}
	ofDrawBitmapString(moveCam, ofPoint(ofGetWindowWidth() - 150, 30));
	string gravStatus = "Gravity: ";
	if (grav) {
		gravStatus += "ON";
	}
	else {
		gravStatus += "OFF";
	}
	ofDrawBitmapString(gravStatus, ofPoint(ofGetWindowWidth() - 150, 45));
	string canThrust = "Thrusters: ";
	if (usableThrust) {
		canThrust += "ON";
	} else {
		canThrust += "OFF";
	}
	ofDrawBitmapString(canThrust, ofPoint(ofGetWindowWidth() - 150, 60));
	string altDisplay = "Altitude (AGL): ";
	if (onAGL) {
		altDisplay += to_string(landerToMoon);
	}
	else {
		altDisplay += "OFF";
	}
	ofDrawBitmapString(altDisplay, ofPoint(ofGetWindowWidth() / 2, 15));
	string bounceDisplay = "Force of Last Contact: ";
	bounceDisplay += to_string(contactForce);
	ofDrawBitmapString(bounceDisplay, ofPoint(ofGetWindowWidth() / 2, 30));
	string fuelDisplay = "Fuel: ";
	if (countFuel) {
		fuelDisplay += to_string(fuel);
	} else {
		fuelDisplay += "OFF";
	}
	ofDrawBitmapString(fuelDisplay, ofPoint(ofGetWindowWidth() / 2, 45));
	if (usableThrust) {
		landingIndicator = "Successful Landing: Not Landed Yet";
	} 
	ofDrawBitmapString(landingIndicator, ofPoint(ofGetWindowWidth() / 2, 60));

	glDepthMask(true);
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

// editted by Elston Ma for CS134 Final Project
void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'I':
	case 'i':
		// toggle ray intersection console log timing test
		runRayIntTest = !runRayIntTest;
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_UP:
		// up thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			if (exhLoaded) {
				exhPlay = true;
			}
			landerForces += glm::vec3(0, UP_THRUST, 0);
			//exhaust.setLifespan(3);
			exhaust.sys->reset();
			exhaust.start();
		}
		break;
	case OF_KEY_DOWN:
		// down thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			landerForces += glm::vec3(0, -OTHER_THRUST, 0);
		}
		break;
	case OF_KEY_LEFT:
		// left thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), 
				glm::radians(lander.getRotationAngle(1)), glm::vec3(0, 1, 0));
			landerForces += (glm::vec3)(rotMat * glm::vec4(OTHER_THRUST, 0, 0, 0));
			//landerForces += glm::vec3(OTHER_THRUST, 0, 0);
		}
		break;
	case OF_KEY_RIGHT:
		// right thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0),
				glm::radians(lander.getRotationAngle(1)), glm::vec3(0, 1, 0));
			landerForces += (glm::vec3)(rotMat * glm::vec4(-OTHER_THRUST, 0, 0, 0));
			//landerForces += glm::vec3(-OTHER_THRUST, 0, 0);
		}
		break;
	case 'E':
	case 'e':
		// forward thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0),
				glm::radians(lander.getRotationAngle(1)), glm::vec3(0, 1, 0));
			landerForces += (glm::vec3)(rotMat * glm::vec4(0, 0, OTHER_THRUST, 0));
			//landerForces += glm::vec3(0, 0, OTHER_THRUST);
		}
		break;
	case 'D':
	case 'd':
		// backward thrust for lander
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0),
				glm::radians(lander.getRotationAngle(1)), glm::vec3(0, 1, 0));
			landerForces += (glm::vec3)(rotMat * glm::vec4(0, 0, -OTHER_THRUST, 0));
			//landerForces += glm::vec3(0, 0, -OTHER_THRUST);
		}
		break;
	case 'Z':
	case 'z':
		// counterclockwise rotation on y-axix
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			landerRotFor += OTHER_THRUST;
		}
		break;
	case 'X':
	case 'x':
		// clockwise rotation on y-axis
		if (bLanderLoaded && startSim && usableThrust && fuel > 0) {
			if (countFuel) {
				fuel--;
			}
			landerRotFor -= OTHER_THRUST;
		}
		break;
	case ' ':
		// press space to start sim
		startSim = !startSim;
		break;
	case 'G':
	case 'g':
		// toggle gravity for testing movement
		grav = !grav;
		break;
	case 'A':
	case 'a':
		// toggle altitude ray intersection algorithm
		onAGL = !onAGL;
		break;
	case 'Q':
	case 'q':
		// toggle thruster usability for diagnostic reasons
		usableThrust = !usableThrust;
		break;
	case 'Y':
	case 'y':
		// toggle if fuel should be counted for diagnostic reasons
		countFuel = !countFuel;
		// reset fuel for diagnostics
		fuel = 60 * 120;
		break;
	case '1':
		cam.enableMouseInput();
		viewCam = &cam;
		break;
	case '2':
		cam.disableMouseInput();
		viewCam = &trackCam;
		break;
	case '3':
		cam.disableMouseInput();
		viewCam = &fixedTrackCam;
		break;
	case '4':
		cam.disableMouseInput();
		viewCam = &landerCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:
		// developed by Elston Ma for CS134 Final Project
		//exhaust.stop();
		//exhaust.setLifespan(0);
		exhaustSound.stop();
		exhPlay = false;
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

// editted by Elston Ma for CS134 Project 2
bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	float beforeFound = ofGetElapsedTimeMillis();
	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	if (runRayIntTest) {
		cout << "Time taken to search for ray intersection: " <<
			ofGetElapsedTimeMillis() - beforeFound << " milliseconds" << endl;
	}

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
		if (runRayIntTest) cout << "Point found: " << pointRet << endl;
	}
	return pointSelected;
}

// use ray intersection octree algorthm to look for
// point of intersection on moon that lander is directly above
// developed by Elston Ma for CS134 Final Project
bool ofApp::rayOctreeFromLander(ofVec3f &pointRet) {
	ofVec3f landerPt = lander.getPosition();
	ofVec3f rayDir(0, -1, 0);
	Ray theRay = Ray(Vector3(landerPt.x, landerPt.y, landerPt.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	landerOverMoon = octree.intersect(theRay, octree.root, landerNode);

	if (landerOverMoon) {
		pointRet = octree.mesh.getVertex(landerNode.points[0]);
	}

	return landerOverMoon;
}

// use box intersection to check if lander bounding box
// intersects with any leaf node boxes of moon
// developed by Elston Ma for CS134 Final Project
void ofApp::checkLanderCollision() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	landerBoxHolder.clear();
	collidedNodeHolder.clear();
	landerCollided = octree.intersect(bounds, octree.root, landerBoxHolder, collidedNodeHolder);
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}*/


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
// developed by Elston Ma for CS134 Final Project
void ofApp::setCameraTarget() {
	// set easycam to look at lander
	if (bLanderLoaded) {
		glm::vec3 landerP = lander.getPosition();
		cam.setPosition(landerP.x, landerP.y + 30, landerP.z - 50);
		cam.lookAt(landerP);	
	}
}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		//cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
