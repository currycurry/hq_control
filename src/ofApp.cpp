#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// initialize the accelerometer
	ofxAccelerometer.setup();
	ofxCompass.setup();

	ofAddListener(ofxAndroidGPS::locationChangedE, this, &ofApp::onLocationChanged);

	//start gps
	gps_object.startGPS();

	//mGravity.resize( 3 );
	//mGeomagnetic.resize( 3 );
	//orientation.resize( 3 );
	ALPHA = .25; //for low pass filter
	azimuth = 0.0;

	font.loadFont( "2.ttf", 25, true, false, true, 0.3, 72);
	radar.loadImage( "radarSwoop1.png");
	ofSetCircleResolution( 50 );
	ofSetLineWidth( 5 );
	ofBackground(0, 0, 0);

	fullscreen = true;

    margin = 100;
    swoop_radius = ofGetHeight() / 2 - ( margin / 2 );
    radarSpeed = .8;
    counter = 0;
    fontHeight = 25;
    //minimum_screen.set( ofGetWidth() / 4 + margin, margin );
    //maximum_screen.set( ofGetWidth() / 4 + ofGetHeight() - margin, ofGetHeight() - margin );
    minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
    maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );

    goldenSpike.set( -119.2065, 40.7864 );

    minimum_world.set( -119.2357, 40.8063 );
    maximum_world.set( -119.1802, 40.7644 );

    //minimum_world_spike_centered( minimum_world.x - goldenSpike.x, minimum_world.y - goldenSpike.y );
    //maximum_world_spike_centered( maximum_world.x - goldenSpike.x, maximum_world.y - goldenSpike.y );


	pentagonPoint.resize( 5 );
	centeredPentagonPoint.resize( 5 );
	pentagonPoint[ 0 ].set( -119.2357, 40.783 );
	pentagonPoint[ 1 ].set( -119.2199, 40.8063 );
	pentagonPoint[ 2 ].set( -119.1857, 40.8022 );
	pentagonPoint[ 3 ].set( -119.1802 , 40.7762 );
	pentagonPoint[ 4 ].set( -119.2111, 40.7644 );

	for ( int i = 0; i < pentagonPoint.size(); i ++ ){
	    centeredPentagonPoint[ i ] = centerToSpike( pentagonPoint[ i ]);
	}

    ofEnableSmoothing();
    ofEnableAntiAliasing();

    ofSetFrameRate( 40 );

    /*#ifdef TARGET_OPENGLES
    	shader.load("shadersES2/shader");
    #else
    	if(ofIsGLProgrammableRenderer()){
    		shader.load("shadersGL3/shader");
    	}else{
    		shader.load("shadersGL2/shader");
    	}
    #endif*/
}
//--------------------------------------------------------------
void ofApp::onLocationChanged(ofxLocation & location) {
    currentLocation = location;
}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetFullscreen( fullscreen );
    minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
    maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );

    orientation = ofxAccelerometer.getOrientation();
    azimuth = fmod( orientation.x * 180 / 3.14159f + 180, 360 );

	accel = ofxAccelerometer.getForce();

	compass_values = ofxCompass.getCompassValues();
	normal_compass.x = compass_values.x / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );
    normal_compass.y = compass_values.y / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );
    normal_compass.z = compass_values.z / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );

    normal_compass_2d.x = compass_values.x / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y );
    normal_compass_2d.y = compass_values.y / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y );

	messages[0] = "G(X) = " + ofToString(accel.x,2);
	messages[1] = "G(Y) = " + ofToString(accel.y,2);
	messages[2] = "G(Z) = " + ofToString(accel.z,2);
    messages[3] = "C(X) = " + ofToString(normal_compass.x);
    messages[4] = "C(Y) = " + ofToString(normal_compass.y);
    messages[5] = "C(Z) = " + ofToString(normal_compass.z);
    messages[6] = "O(X) = " + ofToString(orientation.x);
    messages[7] = "O(Y) = " + ofToString(orientation.y);
    messages[8] = "O(Z) = " + ofToString(orientation.z);
	messages[9] = "LATITUDE = " + ofToString(currentLocation.latitude);
	messages[10] = "LONGITUDE = " + ofToString(currentLocation.longitude);
	messages[11] = "ALTITUDE = " + ofToString(currentLocation.altitude);
	messages[12] = "SPEED = " + ofToString(currentLocation.speed);
	messages[13] = "BEARING = " + ofToString(currentLocation.bearing);
    messages[14] = "AZIMUTH = " + ofToString(azimuth);

	normAccel = accel.getNormalized();

	car_location = centerToSpike( currentLocation.longitude - 45.267, currentLocation.latitude + .0749 );
	counter = counter + 0.02f;

	if ( ofGetFrameNum() % 20 == 0 ) {
	    drawCar = !drawCar;
	}


}

//--------------------------------------------------------------
void ofApp::draw(){

    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofBackground( 0, 0, 0 );
    ofSetLineWidth( 5 );

    ofPushMatrix();
    ofTranslate( 0, 0 );
	ofSetColor( 0, 255, 4 );
	ofNoFill();
	ofRect( 0, 0, ofGetWidth() / 4, ofGetHeight() );
	//shader.setUniformTexture("tex", text.getFontTexture(), 0);
	//getFontTexture();
	font.drawString( messages[0], 10, fontHeight + 20 );
	font.drawString( messages[1], 10, ( fontHeight + 20 ) * 2 );
	font.drawString( messages[2], 10, ( fontHeight + 20 ) * 3 );
	font.drawString( messages[3], 10, ( fontHeight + 20 ) * 4 );
	font.drawString( messages[4], 10, ( fontHeight + 20 ) * 5 );
	font.drawString( messages[5], 10, ( fontHeight + 20 ) * 6 );
	font.drawString( messages[6], 10, ( fontHeight + 20 ) * 7 );
	font.drawString( messages[7], 10, ( fontHeight + 20 ) * 8 );
	font.drawString( messages[8], 10, ( fontHeight + 20 ) * 9 );
    font.drawString( messages[9], 10, ( fontHeight + 20 ) * 10 );
    font.drawString( messages[10], 10, ( fontHeight + 20 ) * 11 );
    font.drawString( messages[11], 10, ( fontHeight + 20 ) * 12 );
    font.drawString( messages[12], 10, ( fontHeight + 20 ) * 13 );
    font.drawString( messages[13], 10, ( fontHeight + 20 ) * 14 );
    font.drawString( messages[14], 10, ( fontHeight + 20 ) * 15 );
	ofPopMatrix();

    //shader.begin();
    // orientation data
    ofPushMatrix();
        ofTranslate( ofGetWidth() / 8, ofGetHeight() / 2 + 150 );
        ofCircle( 0, 0, ofGetWidth() / 8 * .55 );
        ofPushMatrix();
            ofRotateZ( azimuth );
            ofLine( 0, 0, 0, ofGetWidth() / 8 * .55);
        ofPopMatrix();

       // ofLine( 0, 0, normal_compass_2d.x * ofGetWidth() / 8 * .6, normal_compass_2d.y * ofGetWidth() / 8 * .6);
    ofPopMatrix();

    // draw accelerometer data
	ofPushMatrix();
	    ofTranslate( ofGetWidth()/8, ofGetHeight() * 7 / 8 );
	    ofLine( 0, 0, normAccel.x * ( ofGetWidth() / 10 ), 0);
	    ofLine( 0, 0, 0, -normAccel.y * ( ofGetWidth() / 10 ));
	    ofLine( 0, 0, normAccel.z * ( ofGetWidth() / 10 ) * .707, normAccel.z * ( ofGetWidth() / 10 ) * .707 );
	ofPopMatrix();




    ofPushMatrix();
    //center map to golden spike
        ofTranslate( ofMap( goldenSpike.x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x ) - 100,
                 ofMap( goldenSpike.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y), 0 ); //golden spike as new (0,0)

        //radar sweep
        ofPushMatrix();
            ofRotate(ofGetFrameNum() * radarSpeed , 0, 0, 1);//rotate from centre
            ofEnableAlphaBlending();
            radar.draw( -swoop_radius, -swoop_radius, swoop_radius * 2, swoop_radius * 2 );
        ofPopMatrix();
        ofRotateZ( -45 );

        //radar sweep
        //ofCircle( 0, 0, swoop_radius );
        //ofLine( 0, 0, swoop_radius * -sin( counter ), swoop_radius  * cos( counter ));

        //pentagon
        ofBeginShape();
        //ofSetColor( 0, 130, 2);
        for ( int i = 0; i < pentagonPoint.size(); i ++ ) {
    	        ofVertex( centeredPentagonPoint[ i ].x, centeredPentagonPoint[ i ].y);
        }
        ofVertex( centeredPentagonPoint[ 0 ].x, centeredPentagonPoint[ 0 ].y );
        ofEndShape();

        //draw car based on gps coordinates
        ofFill();
        //ofSetColor( 0, 255, 255 );
        if ( drawCar ){
            ofCircle( car_location.x, car_location.y, 15 );
        }

	ofPopMatrix();

    //draw north indicator in the lower right hand corner
	ofPushMatrix();
	    ofTranslate( ofGetWidth() - margin / 2, ofGetHeight() - margin / 2, 0 );
	    ofRotateZ( -45 );
	    ofLine( 0, 0, 0, -200 );
	    ofLine( 0, -200, 30, -160 );
	    ofLine( 0, -200, -30, -160 );
	ofPopMatrix();

    //shader.end();

}

//--------------------------------------------------------------
ofVec2f ofApp::centerToSpike  ( ofVec2f location ){

    ofVec2f centered_location;
    centered_location.x = ofMap( location.x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x) - ofMap( goldenSpike.x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x );
    centered_location.y = ofMap( location.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y) - ofMap( goldenSpike.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y );

    return centered_location;

}

//--------------------------------------------------------------
ofVec2f ofApp::centerToSpike  ( float x, float y ){

    ofVec2f centered_location;
    centered_location.x = ofMap( x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x) - ofMap( goldenSpike.x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x );
    centered_location.y = ofMap( y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y) - ofMap( goldenSpike.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y );

    return centered_location;

}

//--------------------------------------------------------------
vector <float> ofApp::lowPass( vector <float> input, vector <float> output ) {

    //if ( output == null ) return input;
    for ( int i = 0; i < input.size(); i++ ) {
        output[ i ] = output[ i ] + ALPHA * ( input[ i ] - output[ i ]);
    }
    return output;

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    //minimum_screen.set( ofGetWidth() / 4 + margin, margin );
    //maximum_screen.set( ofGetWidth() / 4 + ofGetHeight() - margin, ofGetHeight() - margin );

    ofSetCircleResolution( 50 );

    minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
    maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );


}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){

    if ( x > 0 && x < 200 ) {
        if ( y > 0 && y < 200 ) {
            fullscreen = !fullscreen;
        }
    }


}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){

}

//--------------------------------------------------------------
void ofApp::stop(){

}

//--------------------------------------------------------------
void ofApp::resume(){

	ALPHA = .25; //for low pass filter
    	azimuth = 0.0;

    	font.loadFont( "2.ttf", 25, true, false, true, 0.3, 72);
    	ofSetCircleResolution( 50 );
    	ofSetLineWidth( 5 );
    	ofBackground(0, 0, 0);

    	fullscreen = true;

        margin = 100;
        swoop_radius = ofGetHeight() / 2 - ( margin / 2 );
        counter = 0;
        fontHeight = 25;
        //minimum_screen.set( ofGetWidth() / 4 + margin, margin );
        //maximum_screen.set( ofGetWidth() / 4 + ofGetHeight() - margin, ofGetHeight() - margin );
        minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
        maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );

        goldenSpike.set( -119.2065, 40.7864 );

        minimum_world.set( -119.2357, 40.8063 );
        maximum_world.set( -119.1802, 40.7644 );

        //minimum_world_spike_centered( minimum_world.x - goldenSpike.x, minimum_world.y - goldenSpike.y );
        //maximum_world_spike_centered( maximum_world.x - goldenSpike.x, maximum_world.y - goldenSpike.y );


    	pentagonPoint.resize( 5 );
    	centeredPentagonPoint.resize( 5 );
    	pentagonPoint[ 0 ].set( -119.2357, 40.783 );
    	pentagonPoint[ 1 ].set( -119.2199, 40.8063 );
    	pentagonPoint[ 2 ].set( -119.1857, 40.8022 );
    	pentagonPoint[ 3 ].set( -119.1802 , 40.7762 );
    	pentagonPoint[ 4 ].set( -119.2111, 40.7644 );

    	for ( int i = 0; i < pentagonPoint.size(); i ++ ){
    	    centeredPentagonPoint[ i ] = centerToSpike( pentagonPoint[ i ]);
    	}

        ofEnableSmoothing();
        ofEnableAntiAliasing();

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){



}

//--------------------------------------------------------------
void ofApp::unloadTextures(){



}


//--------------------------------------------------------------
bool ofApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

}

//--------------------------------------------------------------
void ofApp::cancelPressed(){

}
