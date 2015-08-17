#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxAccelerometer.h"
#include "ofxCompass.h"


class ofApp : public ofxAndroidApp{
	
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();
		void unloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

		void onLocationChanged(ofxLocation & location);

		vector <float> lowPass( vector <float> input, vector <float> output );
		float averageAzimuths( float a );


		ofVec2f centerToSpike( ofVec2f );
		ofVec2f centerToSpike( float x, float y );
		ofVec2f car_location;





		bool fullscreen;

		ofTrueTypeFont font;
		ofVec3f accel, normAccel, compass_values, normal_compass;
		ofVec2f normal_compass_2d;
		ofPoint orientation;
		float azimuth;
		//vector <float> mGravity, mGeomagnetic, orientation;
		float ALPHA;
		string messages[ 15 ]; //[ 8 ];

        ofxAndroidGPS gps_object;
        ofxLocation currentLocation;

        ofVec2f goldenSpike;
        vector <ofVec2f> pentagonPoint;
        vector <ofVec2f> centeredPentagonPoint;

        int margin, swoop_radius;
        float counter;
        ofVec2f minimum_screen, minimum_world, maximum_screen, maximum_world, minimum_world_spike_centered, maximum_world_spike_centered;

        ofShader shader;
        int fontHeight;

        ofImage radar;
        float radarSpeed;

        bool drawCar;

        //double altitude;
        //double latitude;
        //double longitude;
        //float speed;
        //float bearing;

};
