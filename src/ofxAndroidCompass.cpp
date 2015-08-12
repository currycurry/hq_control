/*
 * ofxAndroidCompass.cpp
 *
 *  Created on: 28/11/2010
 *      Author: arturo
 */

#include <jni.h>
#include "ofxCompass.h"
#include "ofxAndroidUtils.h"
#include "ofLog.h"

extern "C"{
void
Java_cc_openframeworks_OFAndroidCompass_updateCompass( JNIEnv*  env, jobject  thiz, jfloat x, jfloat y, jfloat z ){
	// android reports these in nT
	ofxCompass.update( x, y, z );
}
}

void ofxCompassHandler::setup(){
	if(!ofGetJavaVMPtr()){
		ofLogError("ofxAndroidCompass") << "setup(): couldn't find java virtual machine";
		return;
	}
	JNIEnv *env;
	if (ofGetJavaVMPtr()->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		ofLogError("ofxAndroidCompass") << "setup(): failed to get environment using GetEnv()";
		return;
	}
	jclass javaClass = env->FindClass("cc/openframeworks/OFAndroid");

	if(javaClass==0){
		ofLogError("ofxAndroidCompass") << "setup(): couldn't find OFAndroid java class";
		return;
	}

	jmethodID setupCompass = env->GetStaticMethodID(javaClass,"setupCompass","()V");
	if(!setupCompass){
		ofLogError("ofxAndroidCompass") << "setup(): couldn't find OFAndroid.setupCompass method";
		return;
	}
	env->CallStaticVoidMethod(javaClass,setupCompass);
}

void ofxCompassHandler::exit(){

}
