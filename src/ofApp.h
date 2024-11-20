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
		void initializeShaders();
    	void initializeVboBuffers();
		
		// ofLight lights[10000];
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

		const int NUMPARTICLES = 10000000; //10000000 for best effects
		const float MASS_SUN = 2000.0;
		const float OMEGA_EQUATOR = 0.0002;
		const float DELTA_OMEGA = 0.0002;
		const float TEMPREATURE_CORE = 15000.0;
		const float ALPHA = 3.0;
    	const float TARGET_RADIUS = 100.0;
    	const float BOLTZMAN_CONSTANT = 0.0000138;

};