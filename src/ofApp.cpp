#include "ofApp.h"
#include <glm/gtc/random.hpp> 
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    
    // Initialize particle data
    positions.resize(numParticles);
    velocities.resize(numParticles);
    masses.resize(numParticles);
    
    initializeParticles();
    
    // Setup transform feedback shader
    string updateVertSource = R"(
        #version 150
        uniform mat4 modelViewProjectionMatrix;
        uniform float time;
        uniform float deltaTime;
        uniform float mass_sun;
        uniform float omega_equator;
        uniform float delta_omega;
        uniform float T_core;
        uniform float alpha;
        uniform float target_radius;
        uniform float boltzman_constant;
        
        in vec4 position;
        in vec4 velocity;
        in float mass;
        
        out vec4 outPosition;
        out vec4 outVelocity;

        const float M_PI = 3.1415926535897932384626433832795;

        void main() {
            vec3 pos = position.xyz;
            vec3 vel = velocity.xyz * 0;
            
            float dist = length(pos);
            vec3 acceleration = vec3(0.0);
            
            // Temperature calculation
            float normalized_radius = (target_radius - dist) / target_radius;
            float temperature = T_core * pow(normalized_radius, alpha);
            
            // Thermal velocity
            vec3 thermal_vel = vec3(sqrt(2.0 * boltzman_constant * temperature / mass) / 1000.0);
            vel += thermal_vel;
            
            // Differential rotation
            if (dist > 0.0) {
                float polar_angle = acos(pos.y / dist);
                float latitude = M_PI / 2.0 - polar_angle;
                float angular_velocity = omega_equator - delta_omega * pow(sin(latitude), 2.0);
                
                vel.x += -angular_velocity * pos.z;
                vel.z += angular_velocity * pos.x;
            }
            
            // Gravity
            if (dist > 5.0) {
                float F_gravity = 0.00667 * mass_sun / (dist * dist);
                acceleration -= F_gravity * pos / dist;
            }
            
            // Pressure
            if (dist > 5.0) {
                float F_pressure = 5.0 * exp(-dist / 5.0);
                acceleration += F_pressure * pos / dist;
            }
            
            // Update velocity and position
            vel += acceleration * deltaTime;
            pos += vel * exp(-0.02 * deltaTime);
            
            outPosition = vec4(pos, 1.0);
            outVelocity = vec4(vel, 0.0);
        }
    )";
    
    string updateFragSource = R"(
        #version 150
        out vec4 fragColor;
        void main() {
            fragColor = vec4(0.0);
        }
    )";
    
    updateShader.setupShaderFromSource(GL_VERTEX_SHADER, updateVertSource);
    updateShader.setupShaderFromSource(GL_FRAGMENT_SHADER, updateFragSource);
    
    // Setup transform feedback
    const GLchar* feedbackVaryings[] = { "outPosition", "outVelocity" };
    glTransformFeedbackVaryings(updateShader.getProgram(), 2, 
                               feedbackVaryings, GL_SEPARATE_ATTRIBS);
    updateShader.linkProgram();
    
    // Setup draw shader
    drawShader.load("shaders/particle");
    
    // Setup double-buffered VBOs
    for(int i = 0; i < 2; i++) {
        vbos[i].setVertexData((float*)&positions[0], 4, positions.size(), GL_DYNAMIC_DRAW, 0);
        vbos[i].setAttributeData(1, (float*)&velocities[0], 4, velocities.size(), GL_DYNAMIC_DRAW);
        vbos[i].setAttributeData(2, &masses[0], 1, masses.size(), GL_DYNAMIC_DRAW);
    }
    
    // Create transform feedback buffer
    glGenTransformFeedbacks(1, &feedbackBuffer);
    
    // Create query object for transform feedback
    glGenQueries(1, &query);
    
    currentVbo = 0;
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackBuffer);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbos[1].getVertId());
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbos[1].getAttributeId(1));
}

void ofApp::initializeParticles(){
    for (int i = 0; i < numParticles; i++) {
        glm::vec3 position = glm::ballRand(100.0);
        positions[i] = glm::vec4(position, 1.0);
        velocities[i] = glm::vec4(0.0);
        masses[i] = ofRandom(1.0);
    }
}

//--------------------------------------------------------------
void ofApp::update() {
    // Bind update shader
    updateShader.begin();
    // Set uniforms
    updateShader.setUniform1f("deltaTime", ofGetLastFrameTime());
    updateShader.setUniform1f("time", ofGetElapsedTimef());
    updateShader.setUniform1f("mass_sun", 2000.0);
    updateShader.setUniform1f("omega_equator", 0.0015);
    updateShader.setUniform1f("delta_omega", 0.0005);
    updateShader.setUniform1f("T_core", 15000.0);
    updateShader.setUniform1f("alpha", 2.0);
    updateShader.setUniform1f("target_radius", 100.0);
    updateShader.setUniform1f("boltzman_constant", 0.0000138);

    // Begin transform feedback
    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackBuffer);
    
    // Bind destination buffers
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbos[1-currentVbo].getVertId());
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbos[1-currentVbo].getAttributeId(1));
    
    glBeginTransformFeedback(GL_POINTS);
    
    // Bind source VBO
    vbos[currentVbo].bind();
    
    // Draw points
    glDrawArrays(GL_POINTS, 0, positions.size());
    
    // End transform feedback
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    
    // Unbind
    vbos[currentVbo].unbind();
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    
    // Swap buffers
    currentVbo = 1 - currentVbo;
    
    updateShader.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(20);
    ofEnableDepthTest();
    cam.begin();
    
    drawShader.begin();
    drawShader.setUniform1f("time", ofGetElapsedTimef());
    drawShader.setUniform1f("mass_sun", 2000.0);
    drawShader.setUniform1f("omega_equator", 0.0015);
    drawShader.setUniform1f("delta_omega", 0.0005);
    drawShader.setUniform1f("T_core", 15000.0);
    drawShader.setUniform1f("alpha", 2.0);
    drawShader.setUniform1f("target_radius", 100.0);
    drawShader.setUniform1f("boltzman_constant", 0.0000138);
    
    // Draw particles using current VBO
    vbos[currentVbo].draw(GL_POINTS, 0, positions.size());
    
    drawShader.end();
    
    cam.end();
    
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
