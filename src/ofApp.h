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
		// ofSpherePrimitive particles[10000];
		// ofLight lights[10000];
		// float masses[10000];
		// glm::vec3 velocities[10000];

		ofEasyCam cam;

		ofShader particleShader;
		ofVbo particleVbo;
		vector<glm::vec4> positions;
		vector<glm::vec4> velocities;
		vector<float> masses;

		// Shaders for update and draw
		ofShader updateShader;
		ofShader drawShader;
		
		// Double-buffered VBOs for transform feedback
		ofVbo vbos[2];
		int currentVbo;
		
		// Transform feedback objects
		GLuint feedbackBuffer;
		GLuint query;

		const int numParticles = 10000000;
};