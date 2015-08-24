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

		void pause();
		void stop();
		void resume();
		void reloadTextures();
		void unloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

		void onLocationChanged(ofxLocation & location);

        //port sensor manager methods for calculating rotation matrix and orientation
		vector <float> mGravity, mGeomagnetic, orientation_fusion;
		bool getRotationMatrix( vector <float> R, vector <float> I, vector <float> gravity, vector <float> geomagnetic );
		vector <float> getOrientation( vector <float> R, vector <float> values );
		vector <float> lowPass( ofVec3f input, vector <float> output );

		vector <float> mR, mI;
		bool success;

		float averageAzimuths( float a );
		float azimuth;
		float avgAzimuth;
        float lastAvgAzm;
        float initialAvgAzm;
        int smoothing_val;
        vector <float> lastAzimuths;
        float totalAzimuth;


		ofVec2f centerToSpike( ofVec2f );
		ofVec2f centerToSpike( float x, float y );
		ofVec2f car_location;

		ofTrueTypeFont font;
		ofVec3f accel, normAccel, compass_values;
		//ofPoint orientation;

		float ALPHA;
		string messages[ 15 ]; //[ 8 ];

        ofxAndroidGPS gps_object;
        ofxLocation currentLocation;

        ofVec2f goldenSpike;
        vector <ofVec2f> pentagonPoint;
        vector <ofVec2f> centeredPentagonPoint;

        int margin, swoop_radius, city_width, city_height;
        //float counter;
        ofVec2f minimum_screen, minimum_world, maximum_screen, maximum_world, minimum_world_spike_centered, maximum_world_spike_centered;

        int fontHeight;

        ofImage radar;
        float radarSpeed;
        bool drawCar;

        ofImage grid;
        bool drawGrid;

        ofImage city;
        bool drawCity;

        int offSet;


};
