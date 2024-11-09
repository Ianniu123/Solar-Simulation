#include "ofApp.h"
#include <glm/gtc/random.hpp> 
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetRandomSeed(0);

    initializeParticles();

}

void ofApp::initializeParticles(){
    for (int i = 0; i < 5000; i++) {
        glm::vec3 position = glm::ballRand(100.0);
        particles[i].setRadius(1.0);
        particles[i].setResolution(64.0);
        particles[i].setPosition(position);
        
        lights[i].setPointLight();
        lights[i].setPosition(position);
        lights[i].setScale(0.01);


        masses[i] = ofRandom(1.0);
        
        glm::vec3 velocity(0, 0, 0);
        velocities[i] = velocity;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    int time = ofGetElapsedTimeMillis();

    float mass_sun = 2000;

    float omega_equator = 0.0015;         // Angular velocity at the equator
    float delta_omega = 0.0005;           // Differential rotation coefficient (adjust as needed)            
    
    float T_core = 15000;      // Core temperature in Kelvin
    float alpha = 2.0;            // Controls the temperature gradient steepness
    float target_radius = 100.0;
    float boltzman_constant = 0.0000138;

    float dt = ofGetLastFrameTime();

    for (int i = 0; i < 5000; i++) {
        glm::vec3 position = particles[i].getPosition();
        glm::vec3 velocity = velocities[i];
        float x = position.x;
        float y = position.y;
        float z = position.z;
        float mass = masses[i];

        float dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        float ax = 0;
        float ay = 0;
        float az = 0;

        // Set particle speed proportional to the square root of the temperature
        float normalized_radius = (target_radius - dist) / target_radius;
        float temperature = T_core * pow(normalized_radius, alpha);
        
        //https://en.wikipedia.org/wiki/Thermal_velocity#cite_note-Gurnett-2
        velocity.x = sqrt(2 * boltzman_constant * temperature / mass) / 1000;
        velocity.y = sqrt(2 * boltzman_constant * temperature / mass) / 1000;
        velocity.z = sqrt(2 * boltzman_constant * temperature / mass) / 1000;
        
        // Calculating differential rotation https://en.wikipedia.org/wiki/Differential_rotation
        // https://mecharithm.com/learning/lesson/velocities-in-robotics-angular-velocities-twists-10
        if (dist != 0) {
            float polar_angle = acos(y / dist);  // Calculate polar angle in radians
            float latitude = M_PI / 2 - polar_angle;  // Convert to latitude in radians
            float angular_velocity = omega_equator - delta_omega * pow(sin(latitude), 2);
            
            velocity.x += -angular_velocity * z;
            velocity.z += angular_velocity * x;          
        }

        //Calculating Forces
        if (dist > 5) {
            float F_gravity = 0.00667 * mass_sun / (dist * dist);
            ax += -F_gravity * x / dist;
            ay += -F_gravity * y / dist;
            az += -F_gravity * z / dist;
        }

        if (dist > 5) {
            float F_pressure = 5 * exp(-dist / 5);
            ax += F_pressure * x / dist;
            ay += F_pressure * y / dist;
            az += F_pressure * z / dist;
        }

        velocity.x += ax * dt;
        velocity.y += ay * dt;
        velocity.z += az * dt;

        //apply damping
        position.x += velocity.x * exp(-0.02 * dt);
        position.y += velocity.y * exp(-0.02 * dt);
        position.z += velocity.z * exp(-0.02 * dt);

        particles[i].setPosition(position);
        lights[i].setPosition(position);
        velocities[i] = velocity;

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(20);
    ofEnableDepthTest();
    ofEnableLighting();
    ofSetColor(ofColor::white);

    cam.begin();
    // granule.begin();
    // granule.setUniform1f("time", ofGetElapsedTimef());

    float T_core = 15000;      // Core temperature
    float alpha = 2.0;            // Controls the temperature gradient steepness
    float target_radius = 100.0;

    glm::vec3 coreColor = glm::vec3(255.0, 255.0, 255.0);  // Pure white for the hottest part of the core
    glm::vec3 hotColor = glm::vec3(1.0, 1.0, 0.8);  // White/Blue for core temperatures
    glm::vec3 coldColor = glm::vec3(1.0, 0.5, 0.3); // Red/Orange for surface temperatures

    for (int i = 0; i < 5000; i++) {
        glm::vec3 position = particles[i].getPosition();
        float x = position.x;
        float y = position.y;
        float z = position.z;

        float dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        // Calculate temperature based on radial distance
        float normalized_radius = (target_radius - dist) / target_radius;
        float temperature = T_core * pow(normalized_radius, alpha);

        // Set particle color based on temperature
        glm::vec3 color;
        if (temperature > T_core * 0.6) {  // Hottest part near core
            color = coreColor;
        } else {  // Surface and cooler layers
            color = glm::mix(coldColor, hotColor, temperature / T_core) * 255.0;
        }
        ofSetColor(color.x, color.y, color.z);
        
        particles[i].draw();
        lights[i].draw();
    }
    // granule.end();

    cam.end();

    ofDisableLighting();
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 'W':
        case 'w':
            cam.dolly(-10);
            break;
        case 'A':
        case 'a':
            cam.truck(-10);
            break;
        case 'S':
        case 's':
            cam.dolly(10);
            break;
        case 'D':
        case 'd':
            cam.truck(10);
            break;
        case 'V':
        case 'v':
            cam.boom(10);
            break;
        case 'C':
        case 'c':
            cam.boom(-10);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    /*
    cam.setGlobalOrientation(glm::vec3(0,glm::pi<float>(), 0));

    // rotate camera from starting point according to mouse position
    cam.panDeg(-0.1 * (x - ofGetWidth()/2));
    cam.tiltDeg(-0.1 * (y - ofGetHeight()/2));
    */
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
