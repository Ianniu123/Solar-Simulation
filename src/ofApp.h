#pragma once

#include "ofMain.h"

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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void initializeParticles();
		
		ofSpherePrimitive sun;
		ofSpherePrimitive particles[5000];
		ofLight lights[5000];
		float masses[5000];
		glm::vec3 velocities[5000];

		ofEasyCam cam;
		
		ofShader granule;
		ofTexture mTex;
};
