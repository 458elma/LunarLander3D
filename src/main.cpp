#include "ofMain.h"
#include "ofApp.h"

// Elston Ma - CS134 - Final Project

//========================================================================
int main( ){
	ofSetupOpenGL(1280, 1024,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
