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

	font.loadFont( "Courier_New_Bold.ttf", 24 );
	ofSetLineWidth( 3 );
	ofBackground(0, 0, 0);

	fullscreen = true;

    margin = 100;
    counter = 0;
    minimum_screen.set( ofGetWidth() / 4 + margin, margin );
    maximum_screen.set( ofGetWidth() / 4 + 1300, margin + 1300 );

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
void ofApp::onLocationChanged(ofxLocation & location) {
    currentLocation = location;
}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetFullscreen( fullscreen );

    orientation = ofxAccelerometer.getOrientation();
    azimuth = orientation.x * 180 / 3.14159f + 180;

	accel = ofxAccelerometer.getForce();

	compass_values = ofxCompass.getCompassValues();
	messages[0] = "G(X) = " + ofToString(accel.x,2);
	messages[1] = "G(Y) = " + ofToString(accel.y,2);
	messages[2] = "G(Z) = " + ofToString(accel.z,2);
    messages[3] = "C(X) = " + ofToString(compass_values.x);
    messages[4] = "C(Y) = " + ofToString(compass_values.y);
    messages[5] = "C(Z) = " + ofToString(compass_values.z);
	messages[6] = "LATITUDE = " + ofToString(currentLocation.latitude);
	messages[7] = "LONGITUDE = " + ofToString(currentLocation.longitude);
	messages[8] = "ALTITUDE = " + ofToString(currentLocation.altitude);
	messages[9] = "SPEED = " + ofToString(currentLocation.speed);
	messages[10] = "BEARING = " + ofToString(currentLocation.bearing);
	messages[11] = "O(X) = " + ofToString(orientation.x);
    messages[12] = "O(Y) = " + ofToString(orientation.y);
    messages[13] = "O(Z) = " + ofToString(orientation.z);
    messages[14] = "azimuth? = " + ofToString(azimuth);





	normAccel = accel.getNormalized();

	car_location = centerToSpike( currentLocation.longitude - 45.2, currentLocation.latitude + .0822 );
	counter = counter + 0.02f;
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofBackground( 0, 0, 0 );
    ofSetLineWidth( 3 );

    ofPushMatrix();
    ofTranslate( 0, 0 );
	ofSetColor( 0, 255, 4 );
	ofNoFill();
	ofRect( 0, 0, ofGetWidth() / 4, ofGetHeight() );
	font.drawString( messages[0], 10, font.stringHeight(messages[0]) + 20 );
	font.drawString( messages[1], 10, ( font.stringHeight(messages[6]) + 20 ) * 2 );
	font.drawString( messages[2], 10, ( font.stringHeight(messages[6]) + 20 ) * 3 );
	font.drawString( messages[3], 10, ( font.stringHeight(messages[6]) + 20 ) * 4 );
	font.drawString( messages[4], 10, ( font.stringHeight(messages[6]) + 20 ) * 5 );
	font.drawString( messages[5], 10, ( font.stringHeight(messages[6]) + 20 ) * 6 );
	font.drawString( messages[6], 10, ( font.stringHeight(messages[6]) + 20 ) * 7 );
	font.drawString( messages[7], 10, ( font.stringHeight(messages[6]) + 20 ) * 8 );
	font.drawString( messages[8], 10, ( font.stringHeight(messages[6]) + 20 ) * 9 );
    font.drawString( messages[9], 10, ( font.stringHeight(messages[6]) + 20 ) * 10 );
    font.drawString( messages[10], 10, ( font.stringHeight(messages[6]) + 20 ) * 11 );
    font.drawString( messages[11], 10, ( font.stringHeight(messages[6]) + 20 ) * 12 );
    font.drawString( messages[12], 10, ( font.stringHeight(messages[6]) + 20 ) * 13 );
    font.drawString( messages[13], 10, ( font.stringHeight(messages[6]) + 20 ) * 14 );
    font.drawString( messages[14], 10, ( font.stringHeight(messages[6]) + 20 ) * 15 );
	ofPopMatrix();

    // orientation data
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 8, ofGetHeight() / 2 + 100 );
    ofCircle( 0, 0, ofGetWidth() / 8 * .6 );
        ofPushMatrix();
        ofRotateZ( atan2( compass_values.y, compass_values.x));
        ofLine( 0, 0, 0, -ofGetWidth() / 8 * .6 );
        ofPopMatrix();
    ofPopMatrix();

    // draw accelerometer data
	ofPushMatrix();
	ofTranslate(ofGetWidth()/8, ofGetHeight() * 7 / 8 );
	ofLine( 0, 0, normAccel.x * ofGetWidth() / 8, 0);
	ofLine( 0, 0, 0, -normAccel.y * ofGetWidth() / 8);
	ofLine( 0, 0, normAccel.z * ofGetWidth() / 8 * .707, normAccel.z * ofGetWidth() / 8 * .707 );
	ofPopMatrix();

    ofPushMatrix();
    //center map to golden spike
    ofTranslate( ofMap( goldenSpike.x, minimum_world.x, maximum_world.x, minimum_screen.x, maximum_screen.x),
                 ofMap( goldenSpike.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y), 0 ); //golden spike as new (0,0)

    ofRotateZ( -45 );

   // ofCircle( 0, 0, 650 );
        ofLine( 0, 0, 650 * -sin( counter ), 650 * cos( counter ));
    //pentagon
    ofBeginShape();
    ofSetColor( 0, 130, 2);
    for ( int i = 0; i < pentagonPoint.size(); i ++ ) {
    	    ofVertex( centeredPentagonPoint[ i ].x, centeredPentagonPoint[ i ].y);
    }
    ofVertex( centeredPentagonPoint[ 0 ].x, centeredPentagonPoint[ 0 ].y );
    ofEndShape();



    //pentagon points
    ofFill();
    /*for ( int i = 0; i < pentagonPoint.size(); i ++ ) {
        ofCircle( centeredPentagonPoint[ i ].x, centeredPentagonPoint[ i ].y, 10 * (i + 1) );
    }*/

	//draw golden spike
	ofSetColor( 255, 255, 0 );
    ofCircle( 0, 0, 20 );
    ofLine( 0, 0, 0, -200 );

    //draw car based on gps coordinates
    ofSetColor( 0, 255, 255 );
    ofCircle( car_location.x, car_location.y, 20 );
	ofPopMatrix();



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

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){

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
