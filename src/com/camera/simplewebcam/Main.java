package com.camera.simplewebcam;

import java.io.IOException;

import android.app.Activity;
import android.os.Bundle;

public class Main extends Activity {
	
	CameraPreview cp;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		try {
			Runtime.getRuntime().exec("su");
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		cp = (CameraPreview) findViewById(R.id.cp); 
	}
	
}
