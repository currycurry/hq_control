#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// initialize the accelerometer
	ofxAccelerometer.setup();
	// initialize the compass
	ofxCompass.setup();
	//create GPS listener
	ofAddListener(ofxAndroidGPS::locationChangedE, this, &ofApp::onLocationChanged);
	//start gps
	gps_object.startGPS();

	//load assets - font and radar image
	font.loadFont( "2.ttf", 25, true, false, true, 0.3, 72);
    radar.loadImage( "radarSwoop1.png");
    city.loadImage("city.png");
    grid.loadImage("grid.png");

    drawCity = true;
    drawGrid = true;

    ofSetCircleResolution( 50 );
    ofSetLineWidth( 5 );
    ofBackground(0, 0, 0);

	//variables for orientation calculations
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

    margin = 100;
    swoop_radius = ofGetHeight() / 2 - ( margin / 2 );
    city_width = ofGetHeight() * 5 / 8;
    city_height = city_width * city.getHeight() / city.getWidth();
    radarSpeed = .8;
    fontHeight = 25;
    offSet = 25;

    minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
    maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );

    goldenSpike.set( -119.2065, 40.7864 );

    minimum_world.set( -119.2357, 40.8063 );
    maximum_world.set( -119.1802, 40.7644 );

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

    accel = ofxAccelerometer.getForce();

    compass_values = ofxCompass.getCompassValues();

    mGravity = lowPass( accel, mGravity );
    mGeomagnetic = lowPass( compass_values, mGeomagnetic );

    if ( !mGravity.empty() && !mGeomagnetic.empty() ) {
        mR.assign( 9, 0.0f );
        mI.assign( 9, 0.0f );
        bool success = getRotationMatrix( mR, mI, mGravity, mGeomagnetic );
        if ( success ) {
            orientation_fusion = getOrientation( mR, orientation_fusion );
            azimuth = orientation_fusion[ 0 ] * (-180 / 3.14159f);
            if ( azimuth < 0 ) {
                azimuth += 360;
            }
            avgAzimuth = averageAzimuths( azimuth );
        }
    }

	messages[0] = "G(X) = " + ofToString(mGravity[ 0 ],2);
	messages[1] = "G(Y) = " + ofToString(mGravity[ 1 ],2);
	messages[2] = "G(Z) = " + ofToString(mGravity[ 2 ],2);
    messages[3] = "C(X) = " + ofToString(mGeomagnetic[ 0 ]);
    messages[4] = "C(Y) = " + ofToString(mGeomagnetic[ 1 ]);
    messages[5] = "C(Z) = " + ofToString(mGeomagnetic[ 2 ]);
    messages[6] = "O(0) = " + ofToString(orientation_fusion[ 0 ]);
    messages[7] = "O(1) = " + ofToString(orientation_fusion[ 1 ]);
    messages[8] = "O(2) = " + ofToString(orientation_fusion[ 2 ]);
	messages[9] = "LATITUDE = " + ofToString(currentLocation.latitude);
	messages[10] = "LONGITUDE = " + ofToString(currentLocation.longitude);
	messages[11] = "ALTITUDE = " + ofToString(currentLocation.altitude);
	messages[12] = "SPEED (GPS) = " + ofToString(currentLocation.speed);
	messages[13] = "BEARING (GPS) = " + ofToString(currentLocation.bearing);
    messages[14] = "AZIMUTH = " + ofToString(avgAzimuth);

    //normalize accelerometer data to draw
	normAccel = accel.getNormalized();

    //calculate car location for drawing
	car_location = centerToSpike( currentLocation.longitude - 45.267, currentLocation.latitude + .0749 ); //centered at HQ
	//car_location = centerToSpike( currentLocation.longitude, currentLocation.latitude ); //centered at burning man

    //blink the car dot
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

    //text readout and panel
    ofPushMatrix();
        ofTranslate( 0, 0 );
	    ofSetColor( 0, 255, 4 );
	    ofNoFill();
	    ofRect( 0, 0, ofGetWidth() / 4, ofGetHeight() );
	    ofRect( 0, 0, ofGetWidth(), ofGetHeight());
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
    // draw compass
    ofPushMatrix();
        ofTranslate( ofGetWidth() / 8, ofGetHeight() / 2 + 150 );
        ofCircle( 0, 0, ofGetWidth() / 8 * .55 );

        ofPushMatrix();
            ofRotateZ( -azimuth );
            ofLine( 0, 0, 0, -ofGetWidth() / 8 * .55);
        ofPopMatrix();

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
            ofRotate( ofGetFrameNum() * radarSpeed , 0, 0, 1);//rotate from center
            ofEnableAlphaBlending();
            radar.draw( -swoop_radius, -swoop_radius, swoop_radius * 2, swoop_radius * 2 );
        ofPopMatrix();

        if ( drawCity ) {
            city.draw( -city_width / 2, -city_height / 3 + offSet, city_width, city_height );
        }

        ofRotateZ( -45 ); //rotate the whole thing so the pentagon and city are vertical on the map, north is 45 degrees left of vertical

        //pentagon
        ofBeginShape();
        for ( int i = 0; i < pentagonPoint.size(); i ++ ) {
    	    ofVertex( centeredPentagonPoint[ i ].x, centeredPentagonPoint[ i ].y);
        }
        ofVertex( centeredPentagonPoint[ 0 ].x, centeredPentagonPoint[ 0 ].y );
        ofEndShape();

        //draw car based on gps coordinates
        ofFill();
        if ( drawCar ){
            ofCircle( car_location.x, car_location.y, 15 );
        }

	ofPopMatrix();

    //draw north indicator in the lower right hand corner, north is 45 degrees left of vertical
	ofPushMatrix();
	    ofTranslate( ofGetWidth() - margin / 2, ofGetHeight() - margin / 2, 0 );
	    ofRotateZ( -45 );
	    ofLine( 0, 0, 0, -200 );
	    ofLine( 0, -200, 30, -160 );
	    ofLine( 0, -200, -30, -160 );
	ofPopMatrix();

	if ( drawGrid ) {
        grid.draw( ofGetWidth() / 4, 0, 3 * ofGetWidth() / 4, ofGetHeight() );
    }

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

    ofSetCircleResolution( 50 );

    minimum_screen.set( ofGetWidth() - ofGetHeight() + margin, margin );
    maximum_screen.set( ofGetWidth() - margin, ofGetHeight() - margin );


}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){

    if ( x < 100 && y < 100 ) {
        drawCity = !drawCity;
    }

    if ( x > ofGetWidth() - 100 && y < 100 ) {
        drawGrid = !drawGrid;
    }

    /*if ( x < 100 && y > ofGetHeight() - 100 ) {
        offSet +=5;
    }

    if ( x > ofGetWidth() - 100 && y > ofGetHeight() - 100 ) {
        offSet -=5;
    }*/

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){

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

    return avgAzm;

}
