#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// initialize the accelerometer
	ofxAccelerometer.setup();
	ofxCompass.setup();

	ofAddListener(ofxAndroidGPS::locationChangedE, this, &ofApp::onLocationChanged);

	//start gps
	gps_object.startGPS();
	mGravity.assign( 3, 0.0f );
	mGeomagnetic.assign( 3, 0.0f );
	orientation_fusion.assign( 3, 0.0f );
	mR.assign( 9, 0.0f );
	mI.assign( 9, 0.0f );



    smoothing_val = 10;
    lastAzimuths.assign( smoothing_val, 0.0f );
    azimuth = 0.0;
    avgAzimuth = 0;
    lastAvgAzm = 0.0;
    totalAzimuth = 0.0;


	ALPHA = .25; //for low pass filter


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

    mGravity = lowPass( accel, mGravity );
    mGeomagnetic = lowPass( compass_values, mGeomagnetic );

    //orientation = ofxAccelerometer.getOrientation();
    //azimuth = fmod( orientation.x * 180 / 3.14159f + 180, 360 );

    if ( !mGravity.empty() && !mGeomagnetic.empty() ) {
        //vector <float> mR, mI;
        mR.assign( 9, 0.0f );
        mI.assign( 9, 0.0f );
        bool success = getRotationMatrix( mR, mI, mGravity, mGeomagnetic );
        if ( success ) {
            //orientation_fusion.assign( 3, 0.0f );
            getOrientation( mR, orientation_fusion );
            azimuth = orientation_fusion[ 0 ] * 180 / 3.14159f + 180;
            avgAzimuth = averageAzimuths( azimuth );
        }
    }

    //avgAzimuth = averageAzimuths( azimuth );

	accel = ofxAccelerometer.getForce();

	compass_values = ofxCompass.getCompassValues();
	normal_compass.x = compass_values.x / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );
    normal_compass.y = compass_values.y / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );
    normal_compass.z = compass_values.z / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y + compass_values.z * compass_values.z );

    normal_compass_2d.x = compass_values.x / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y );
    normal_compass_2d.y = compass_values.y / sqrt( compass_values.x * compass_values.x + compass_values.y * compass_values.y );

	messages[0] = "G(X) = " + ofToString(mGravity[ 0 ],2);
	messages[1] = "G(Y) = " + ofToString(mGravity[ 1 ],2);
	messages[2] = "G(Z) = " + ofToString(mGravity[ 2 ],2);
    messages[3] = "C(X) = " + ofToString(mGeomagnetic[ 0 ]);
    messages[4] = "C(Y) = " + ofToString(mGeomagnetic[ 1 ]);
    messages[5] = "C(Z) = " + ofToString(mGeomagnetic[ 2 ]);
    messages[6] = "O(X) = " + ofToString(orientation_fusion[ 0 ]);
    messages[7] = "O(Y) = " + ofToString(orientation_fusion[ 1 ]);
    messages[8] = "O(Z) = " + ofToString(orientation_fusion[ 2 ]);
	messages[9] = "LATITUDE = " + ofToString(currentLocation.latitude);
	messages[10] = "LONGITUDE = " + ofToString(currentLocation.longitude);
	messages[11] = "ALTITUDE = " + ofToString(currentLocation.altitude);
	messages[12] = "SPEED = " + ofToString(currentLocation.speed);
	messages[13] = "BEARING = " + ofToString(currentLocation.bearing);
    messages[14] = "AZIMUTH = " + ofToString(avgAzimuth);

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
                 ofMap( goldenSpike.y, minimum_world.y, maximum_world.y, minimum_screen.y, maximum_screen.y) + 40, 0 ); //golden spike as new (0,0)

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
vector <float> ofApp::lowPass( ofVec3f input, vector <float> output ) {

    if ( output.empty() ) {
        output[ 0 ] = input.x;
        output[ 1 ] = input.y;
        output[ 2 ] = input.z;
    }
    output[ 0 ] = output[ 0 ] + ALPHA * (input.x - output[ 0 ]);
    output[ 1 ] = output[ 1 ] + ALPHA * (input.y - output[ 1 ]);
    output[ 2 ] = output[ 2 ] + ALPHA * (input.z - output[ 2 ]);

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

	mGravity.assign( 3, 0.0f );
    mGeomagnetic.assign( 3, 0.0f );
    orientation_fusion.assign( 3, 0.0f );
    mR.assign( 9, 0.0f );
    mI.assign( 9, 0.0f );



    smoothing_val = 10;
    lastAzimuths.assign( smoothing_val, 0.0f );
    azimuth = 0.0;
    avgAzimuth = 0;
    lastAvgAzm = 0.0;
    totalAzimuth = 0.0;


    ALPHA = .25; //for low pass filter


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
//--------------------------------------------------------------
bool ofApp::getRotationMatrix( vector <float> R, vector <float> I, vector <float> gravity, vector <float> geomagnetic ) {

    float Ax = gravity[0];
    float Ay = gravity[1];
    float Az = gravity[2];
    float Ex = geomagnetic[0];
    float Ey = geomagnetic[1];
    float Ez = geomagnetic[2];
    float Hx = Ey*Az - Ez*Ay;
    float Hy = Ez*Ax - Ex*Az;
    float Hz = Ex*Ay - Ey*Ax;
    float normH = (float)sqrt(Hx*Hx + Hy*Hy + Hz*Hz);
    if (normH < 0.1f) {
            // device is close to free fall (or in space?), or close to
            // magnetic north pole. Typical values are  > 100.
            return false;
        }
        float invH = 1.0f / normH;
        Hx *= invH;
        Hy *= invH;
        Hz *= invH;
        float invA = 1.0f / (float)sqrt(Ax*Ax + Ay*Ay + Az*Az);
        Ax *= invA;
        Ay *= invA;
        Az *= invA;
        float Mx = Ay*Hz - Az*Hy;
        float My = Az*Hx - Ax*Hz;
        float Mz = Ax*Hy - Ay*Hx;
        if ( !R.empty() ) {
            if (R.size() == 9) {
                R[0] = Hx;     R[1] = Hy;     R[2] = Hz;
                R[3] = Mx;     R[4] = My;     R[5] = Mz;
                R[6] = Ax;     R[7] = Ay;     R[8] = Az;
            } else if (R.size() == 16) {
                R[0]  = Hx;    R[1]  = Hy;    R[2]  = Hz;   R[3]  = 0;
                R[4]  = Mx;    R[5]  = My;    R[6]  = Mz;   R[7]  = 0;
                R[8]  = Ax;    R[9]  = Ay;    R[10] = Az;   R[11] = 0;
                R[12] = 0;     R[13] = 0;     R[14] = 0;    R[15] = 1;
            }
            mR = R;

        }
        if ( !I.empty() ) {
            // compute the inclination matrix by projecting the geomagnetic
            // vector onto the Z (gravity) and X (horizontal component
            // of geomagnetic vector) axes.
            float invE = 1.0f / (float)sqrt(Ex*Ex + Ey*Ey + Ez*Ez);
            float c = (Ex*Mx + Ey*My + Ez*Mz) * invE;
            float s = (Ex*Ax + Ey*Ay + Ez*Az) * invE;
            if (I.size() == 9) {
                I[0] = 1;     I[1] = 0;     I[2] = 0;
                I[3] = 0;     I[4] = c;     I[5] = s;
                I[6] = 0;     I[7] =-s;     I[8] = c;
            } else if (I.size() == 16) {
                I[0] = 1;     I[1] = 0;     I[2] = 0;
                I[4] = 0;     I[5] = c;     I[6] = s;
                I[8] = 0;     I[9] =-s;     I[10]= c;
                I[3] = I[7] = I[11] = I[12] = I[13] = I[14] = 0;
                I[15] = 1;
            }
            mI = I;
        }
        return true;

}

//--------------------------------------------------------------
vector <float> ofApp::getOrientation( vector <float> R, vector <float> values ) {

    if (R.size() == 9) {
        values[0] = (float)atan2( R[1], R[4] );
        values[1] = (float)asin( -R[7] );
        values[2] = (float)atan2( -R[6], R[8] );
    } else {
        values[0] = (float)atan2( R[1], R[5] );
        values[1] = (float)asin( -R[9] );
        values[2] = (float)atan2( -R[8], R[10] );
    }


    orientation_fusion = values;

    return values;

}


//--------------------------------------------------------------
float ofApp::averageAzimuths( float a ) {
    float avgAzm;
    for ( int i = 0; i < smoothing_val - 1; i ++ ) {
        lastAzimuths[ i + 1 ] = lastAzimuths[ i ];
    }
    lastAzimuths[ 0 ] = azimuth;

    for ( int i = 0; i < smoothing_val; i ++ ) {
        totalAzimuth += lastAzimuths[ i ];
    }
    avgAzm = totalAzimuth / smoothing_val;
    totalAzimuth = 0;

    //lastAvgAzm = avgAzm;
    return avgAzm;

}
