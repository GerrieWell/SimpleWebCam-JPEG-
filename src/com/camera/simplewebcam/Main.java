package com.camera.simplewebcam;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.Date;
import java.text.SimpleDateFormat;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.drawable.Drawable;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class Main extends Activity {
	
	CameraPreview cp;
	private Button mVideoStartBtn;
    private MediaRecorder mMediaRecorder;
    private SurfaceHolder mSurfaceHolder;
    private File mRecVedioPath;
    private File mRecAudioFile;
    private TextView timer;
    private int hour = 0;
    private int minute = 0;
    private int second = 0;
    private boolean bool;
    private int parentId;
    //protected Camera camera;
    protected boolean isPreview;
    private boolean isRecording = true; // true表示没有录像，点击开始；false表示正在录像，点击暂停
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
		mVideoStartBtn = (Button) findViewById(R.id.button_video);
		 // 设置缓存路径
        mRecVedioPath = new File(Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/hfdatabase/video/temp/");
        if (!mRecVedioPath.exists()) {
            mRecVedioPath.mkdirs();
        }
        
        SurfaceHolder holder = cp.getHolder();
/*/        
        holder.addCallback(new Callback() {

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (camera != null) {
                    if (isPreview) {
                        camera.stopPreview();
                        isPreview = false;
                    }
                    camera.release();
                    camera = null; // 记得释放
                }
                cp = null;
                mSurfaceHolder = null;
                mMediaRecorder = null;
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                try {
                    //camera = cp.//Camera.open();
                    Camera.Parameters parameters = camera.getParameters();
                    parameters.setPreviewFrameRate(5); // 每秒5帧
                    parameters.setPictureFormat(PixelFormat.JPEG);// 设置照片的输出格式
                    parameters.set("jpeg-quality", 85);// 照片质量
                    camera.setParameters(parameters);
                    camera.setPreviewDisplay(holder);
                    camera.startPreview();
                    isPreview = true;
                } catch (Exception e) {
                    e.printStackTrace();
                }
                mSurfaceHolder = holder;
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                    int width, int height) {
                mSurfaceHolder = holder;
            }
        });
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
//*/     
        mVideoStartBtn.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View v) {
                if (isRecording) {
                    /*
                     * 点击开始录像
                     */
                    if (isPreview) {
                        camera.stopPreview();
                        camera.release();
                        camera = null;
                    }
                    second = 0;
                    minute = 0;
                    hour = 0;
                    bool = true;
                    if (mMediaRecorder == null)
                        mMediaRecorder = new MediaRecorder();
                    else
                        mMediaRecorder.reset();
                    mMediaRecorder.setPreviewDisplay(mSurfaceHolder
                            .getSurface());
                    mMediaRecorder
                            .setVideoSource(MediaRecorder.VideoSource.DEFAULT);
                    mMediaRecorder
                            .setAudioSource(MediaRecorder.AudioSource.MIC);
                    mMediaRecorder
                            .setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
                    mMediaRecorder
                            .setVideoEncoder(MediaRecorder.VideoEncoder.H264);
                    mMediaRecorder
                            .setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
                    mMediaRecorder.setVideoSize(320, 240);
                    mMediaRecorder.setVideoFrameRate(15);
                    try {
                        mRecAudioFile = File.createTempFile("Vedio", ".3gp",
                                mRecVedioPath);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    mMediaRecorder.setOutputFile(mRecAudioFile
                            .getAbsolutePath());
                    try {
                        mMediaRecorder.prepare();
                        timer.setVisibility(View.VISIBLE);
                        handler.postDelayed(task, 1000);
                        mMediaRecorder.start();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    showMsg("开始录制");
                    isRecording = !isRecording;
                } else {
                    /*
                     * 点击停止
                     */
                    try {
                        bool = false;
                        mMediaRecorder.stop();
                        timer.setText(format(hour) + ":" + format(minute) + ":"
                                + format(second));
                        mMediaRecorder.release();
                        mMediaRecorder = null;
                        videoRename();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    isRecording = !isRecording;
                    showMsg("录制完成，已保存");

                    try {
                        camera = Camera.open();
                        Camera.Parameters parameters = camera.getParameters();
                        parameters.setPreviewFrameRate(5); // 每秒5帧
                        parameters.setPictureFormat(PixelFormat.JPEG);// 设置照片的输出格式
                        parameters.set("jpeg-quality", 85);// 照片质量
                        camera.setParameters(parameters);
                        camera.setPreviewDisplay(mSurfaceHolder);
                        camera.startPreview();
                        isPreview = true;
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        Button btnImgStart = (Button) findViewById(R.id.button1);
        btnImgStart.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (mMediaRecorder != null) {
                    try {
                        bool = false;
                        mMediaRecorder.stop();
                        timer.setText(format(hour) + ":" + format(minute) + ":"
                                + format(second));
                        mMediaRecorder.release();
                        mMediaRecorder = null;
                        videoRename();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    isRecording = !isRecording;
                    showMsg("录制完成，已保存");

                    try {
                        camera = Camera.open();
                        Camera.Parameters parameters = camera.getParameters();
                        parameters.setPreviewFrameRate(5); // 每秒5帧
                        parameters.setPictureFormat(PixelFormat.JPEG);// 设置照片的输出格式
                        parameters.set("jpeg-quality", 85);// 照片质量
                        camera.setParameters(parameters);
                        camera.setPreviewDisplay(mSurfaceHolder);
                        camera.startPreview();
                        isPreview = true;
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                if (camera != null) {
                    camera.autoFocus(null);
                    camera.takePicture(null, null, new PictureCallback() {
                        @Override
                        public void onPictureTaken(byte[] data, Camera camera) {
                            Bitmap bitmap = BitmapFactory.decodeByteArray(data, data.length, 0);
                            Matrix matrix = new Matrix();
                            // 设置缩放
                            matrix.postScale(5f, 4f);
                            bitmap = Bitmap.createBitmap(bitmap, 0, 0,
                                    bitmap.getWidth(), bitmap.getHeight(),
                                    matrix, true);

                            String path = Environment
                                    .getExternalStorageDirectory()
                                    .getAbsolutePath()
                                    + "/hfdatabase/img/"
                                    + String.valueOf(parentId) + "/";
                            long time=System.currentTimeMillis();//long now = android.os.SystemClock.uptimeMillis();  
                            String fileName = new SimpleDateFormat(
                                    "yyyyMMddHHmmss").format(new Date(time))
                                    + ".jpg";
                            File out = new File(path);
                            if (!out.exists()) {
                                out.mkdirs();
                            }
                            out = new File(path, fileName);
                            try {
                                FileOutputStream outStream = new FileOutputStream(
                                        out);
                                bitmap.compress(CompressFormat.JPEG, 100,
                                        outStream);
                                outStream.close();
                                camera.startPreview();
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                            showMsg("拍照成功");
                        }
                    }); // 拍照
                }
            }
        });
	}
	   /*
     * 消息提示
     */
    private Toast toast;

    public void showMsg(String arg) {
        if (toast == null) {
            toast = Toast.makeText(this, arg, Toast.LENGTH_SHORT);
        } else {
            toast.cancel();
            toast.setText(arg);
        }
        toast.show();
    }
	
	  /*
     * 生成video文件名字
     */
    protected void videoRename() {
        String path = Environment.getExternalStorageDirectory()
                .getAbsolutePath()
                + "/hfdatabase/video/"
                + String.valueOf(parentId) + "/";
        long time=System.currentTimeMillis();
        String fileName = new SimpleDateFormat("yyyyMMddHHmmss")
                .format(new Date(time)) + ".3gp";
        File out = new File(path);
        if (!out.exists()) {
            out.mkdirs();
        }
        out = new File(path, fileName);
        if (mRecAudioFile.exists())
            mRecAudioFile.renameTo(out);
    }

    /*
     * 定时器设置，实现计时
     */
    private Handler handler = new Handler();
    private Runnable task = new Runnable() {
        public void run() {
            if (bool) {
                handler.postDelayed(this, 1000);
                second++;
                if (second >= 60) {
                    minute++;
                    second = second % 60;
                }
                if (minute >= 60) {
                    hour++;
                    minute = minute % 60;
                }
                timer.setText(format(hour) + ":" + format(minute) + ":"
                        + format(second));
            }
        }
    };

    /*
     * 格式化时间
     */
    public String format(int i) {
        String s = i + "";
        if (s.length() == 1) {
            s = "0" + s;
        }
        return s;
    }

    /*
     * 覆写返回键监听
     */
    @Override
    public void onBackPressed() {
        if (mMediaRecorder != null) {
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            videoRename();
        }
        finish();
    }

    @Override
    protected void onPause() {
        super.onPause();
        onBackPressed();
    }
}

