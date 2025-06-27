package com.chinavision.yjf.androiddemo;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import androidx.appcompat.app.AppCompatActivity; // Changed to AndroidX
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity implements mvCameraCallback, SurfaceHolder.Callback {

    // 相机状态
    enum CamState { OPENED, OPENING, CLOSED };

    // ui
    private TextView m_textView;
    private SurfaceHolder m_PreviewHolder;
    private EditText m_etHOff;
    private EditText m_etVOff;
    private EditText m_etWidth;
    private EditText m_etHeight;
    private Spinner m_ResolutionSpinner;
    private TextView m_ExptimeLabel;
    private SeekBar m_ExptimeSeekBar;
    private TextView m_GainLabel;
    private SeekBar m_GainSeekBar;
    private Spinner m_TriggerSpinner;

    // camera
    private mvCamera m_cam = new mvCamera(this);
    private int m_hCamera = 0;
    private CamState m_CamState = CamState.CLOSED;
    private Thread m_OpenThread;
    private Thread m_CapThread;
    private boolean m_QuitCapThread;
    private Timer m_timer;

    // control
    private boolean m_bSnap;
    private int m_SnapIndex;
    private int m_ExptimeMax;
    private int m_ExptimeMin;
    private double m_ExpLineTime;
    private int m_AnalogGainMax;
    private int m_AnalogGainMin;
    private float m_AnalogGainStep;

    // stat
    private long m_lastTime = 0;
    private int m_Disp = 0;
    private int m_lastDisp = 0;
    private int m_lastCap = 0;
    private int m_lastWidth = 0;
    private int m_lastHeight = 0;

    private Handler m_handler = new Handler();
    private final String TAG = "mvCamera";

    private int ExpTime2ExpLines(double exptime) {
        if (m_ExpLineTime > 0) {
             int nLines = (int)((exptime + m_ExpLineTime / 2) / m_ExpLineTime);
             if (nLines < m_ExptimeMin) {
                 nLines = m_ExptimeMin;
             }
             else if (nLines > m_ExptimeMax) {
                 nLines = m_ExptimeMax;
             }
             return nLines;
        } else {
            return m_ExptimeMin;
        }
    }

    private void UpdateExptimeUI(boolean changeSeekBar) {
        double exptime = m_cam.CameraGetExposureTime(m_hCamera);
        if (changeSeekBar) {
            m_ExpLineTime = m_cam.CameraGetExposureLineTime(m_hCamera);
            m_ExptimeSeekBar.setProgress(ExpTime2ExpLines(exptime) - m_ExptimeMin);
        }
        m_ExptimeLabel.setText(String.format("Exptime %.3fms", exptime / 1000.0));
    }

    private SeekBar.OnSeekBarChangeListener m_OnExptimeSeekBarChange = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser && m_hCamera > 0) {
                double exptime = (progress + m_ExptimeMin) * m_ExpLineTime;
                int status = m_cam.CameraSetExposureTime(m_hCamera, exptime);
                Log.i(TAG, "CameraSetExposureTime: " + status);
                UpdateExptimeUI(false);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {}

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {}
    };

    private void UpdateAnalogGainUI(boolean changeSeekBar) {
        int gain = m_cam.CameraGetAnalogGain(m_hCamera);
        if (changeSeekBar) {
            m_GainSeekBar.setProgress(gain - m_AnalogGainMin);
        }
        m_GainLabel.setText(String.format("AnalogGain %.2f", gain * m_AnalogGainStep));
    }

    private SeekBar.OnSeekBarChangeListener m_OnGainSeekBarChange = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser && m_hCamera > 0) {
                int gain = progress + m_AnalogGainMin;
                int status = m_cam.CameraSetAnalogGain(m_hCamera, gain);
                Log.i(TAG, "CameraSetAnalogGain: " + status);
                UpdateAnalogGainUI(false);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {}

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {}
    };

    private void UpdateResolutionUI() {
        int[] res = m_cam.CameraGetImageResolution(m_hCamera);
        int index = res[0];
        int hoff = res[1];
        int voff = res[2];
        int width = res[3];
        int height = res[4];

        if (index != 255) {
            m_ResolutionSpinner.setSelection(index + 1);
        } else {
            m_ResolutionSpinner.setSelection(0);
        }

        m_etHOff.setText(String.format("%d", hoff));
        m_etVOff.setText(String.format("%d", voff));
        m_etWidth.setText(String.format("%d", width));
        m_etHeight.setText(String.format("%d", height));

        UpdateExptimeUI(true);
    }

    private AdapterView.OnItemSelectedListener m_OnResolutionSpinnerSelected = new AdapterView.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int position, long id) {
            if (m_hCamera <= 0)
                return;
            if (position > 0) {
                int status = m_cam.CameraSetImageResolution(m_hCamera, position - 1, 0, 0, 0, 0);
                Log.i(TAG, "CameraSetImageResolution: " + status);
                UpdateResolutionUI();
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {}
    };

    private void UpdateTriggerUI() {
        int mode = m_cam.CameraGetTriggerMode(m_hCamera);
        if (mode >= 0 && mode <= 2) {
            m_TriggerSpinner.setSelection(mode + 1);
        } else {
            m_TriggerSpinner.setSelection(0);
        }
    }

    private AdapterView.OnItemSelectedListener m_OnTriggerSpinnerSelected = new AdapterView.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int position, long id) {
            if (m_hCamera <= 0)
                return;
            if (position > 0) {
                int mode = position - 1;
                int status = m_cam.CameraSetTriggerMode(m_hCamera, mode);
                Log.i(TAG, "CameraSetTriggerMode(" + mode + ") = " + status);
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {}
    };

    private void SetupCameraParamsUI() {
        int[] range = m_cam.CameraGetExposureTimeRange(m_hCamera);
        m_ExptimeMin = range[0];
        m_ExptimeMax = range[1];
        m_ExptimeSeekBar.setMax(m_ExptimeMax - m_ExptimeMin);
        UpdateExptimeUI(true);

        range = m_cam.CameraGetAnalogGainRange(m_hCamera);
        m_AnalogGainMin = range[0];
        m_AnalogGainMax = range[1];
        m_AnalogGainStep = m_cam.CameraGetAnalogGainStep(m_hCamera);
        m_GainSeekBar.setMax(m_AnalogGainMax - m_AnalogGainMin);
        UpdateAnalogGainUI(true);

        String[] Resolutions = m_cam.CameraGetPresetResolutions(m_hCamera);
        List<String> ResolutionList = new ArrayList(Arrays.asList(Resolutions));
        ResolutionList.add(0, "Custom");
        m_ResolutionSpinner.setAdapter(new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                ResolutionList)
        );
        UpdateResolutionUI();

        List<String> TriggerModeList = new ArrayList(Arrays.asList(new String[]
                { "Error", "Continuous", "SoftTrigger", "HardwareTrigger" }));
        m_TriggerSpinner.setAdapter(new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                TriggerModeList));
        UpdateTriggerUI();
    }

    private boolean SaveBitmap(Bitmap bmp, String fileName) {
        File f = new File("/mnt/sdcard/", fileName);
        if (f.exists()) {
            f.delete();
        }

        Bitmap.CompressFormat format;
        if (fileName.endsWith("png"))
            format = Bitmap.CompressFormat.PNG;
        else
            format = Bitmap.CompressFormat.JPEG;

        try {
            FileOutputStream out = new FileOutputStream(f);
            bmp.compress(format, 80, out);
            out.flush();
            out.close();
            Log.i(TAG, "save complete");
            return true;
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return false;
    }

    private void WaitOpenThreadQuit() {
        if (m_OpenThread == null)
            return;
        try {
            m_OpenThread.join();
        } catch (Exception e) { }
        m_OpenThread = null;
    }

    private Runnable m_UpdateStat = new Runnable() {
        @Override
        public void run() {
            if (m_hCamera <= 0)
                return;

            int[] stat = m_cam.CameraGetFrameStatistic(m_hCamera);
            long curTime = System.currentTimeMillis();
            if (m_lastTime > 0)
            {
                float DispFps = (float)(m_Disp - m_lastDisp) * 1000.0f / (float)(curTime - m_lastTime);
                float CapFps = (float)(stat[1] - m_lastCap) * 1000.0f / (float)(curTime - m_lastTime);
                m_textView.setText(String.format("size:%d*%d Disp:%.2f Cap:%.2f\nC:%d L:%d E:%d",
                        m_lastWidth, m_lastHeight, DispFps, CapFps, stat[1], stat[2], stat[0] - (stat[1] + stat[2])));
            }
            m_lastDisp = m_Disp;
            m_lastCap = stat[1];
            m_lastTime = curTime;
        }
    };

    private Runnable m_OnOpenCameraCompleted = new Runnable() {
        @Override
        public void run() {
            if (m_hCamera > 0) {
                m_lastTime = 0;
                m_Disp = 0;
                m_lastDisp = 0;
                m_lastCap = 0;
                m_lastWidth = 0;
                m_lastHeight = 0;

                SetupCameraParamsUI();

                m_QuitCapThread = false;
                m_CapThread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while (!m_QuitCapThread) {
                            if (m_bSnap) {
                                Bitmap bmp = m_cam.CameraCaptureImage(m_hCamera);
                                if (bmp != null) {
                                    SaveBitmap(bmp, String.format("cam_snap%03d.png", ++m_SnapIndex) );
                                    m_bSnap = false;
                                }
                            }
                            else {
                                int[] result = m_cam.CameraCapture(m_hCamera, m_PreviewHolder.getSurface());
                                if (result[0] == 0) {
                                    m_lastWidth = result[1];
                                    m_lastHeight = result[2];
                                    ++m_Disp;
                                }
                            }
                        }
                    }
                });
                m_CapThread.start();

                TimerTask task = new TimerTask() {
                    public void run() {
                        m_handler.post(m_UpdateStat);
                    }
                };
                m_timer = new Timer();
                m_timer.schedule(task, 1000, 1000);

                m_CamState = CamState.OPENED;
                m_textView.setText("Camera open succeed");
            } else {
                m_CamState = CamState.CLOSED;
                m_textView.setText("Camera init failed");
            }
        }
    };

    public void onEnumerateDeviceCompleted(int nDev) {
        if (m_CamState != CamState.OPENING) {
            return;
        }

        if (nDev > 0) {
            WaitOpenThreadQuit();

            m_textView.setText("Connect Camera ...");

            m_OpenThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    m_hCamera = m_cam.CameraInitEx(0, -1, -1);
                    m_handler.post(m_OnOpenCameraCompleted);
                }
            });
            m_OpenThread.start();
        }
        else {
            m_CamState = CamState.CLOSED;
            m_textView.setText("Camera not found");
        }
    }

    protected  void openCamera()
    {
        if (m_PreviewHolder.getSurface() == null)
            return;

        if (m_CamState != CamState.CLOSED)
            return;

        m_CamState = CamState.OPENING;
        m_textView.setText("Enum Camera ...");

        if (true) {
            // Only supports USB Camera
            // No usb permission
            m_cam.CameraEnumerateDeviceEx2(this);
        }
        else {
            // GIGE Camera
            // OR
            // USB Camera
            // android 4 root
            // android 5+ Have usb permission
            int nCam = m_cam.CameraEnumerateDeviceEx();
            onEnumerateDeviceCompleted(nCam);
        }
    }

    protected  void closeCamera()
    {
        WaitOpenThreadQuit();

        if (m_timer != null) {
            m_timer.cancel();
            m_timer = null;
        }

        if (m_CapThread != null) {
            m_QuitCapThread = true;
            try {
                m_CapThread.join();
            }catch (Exception e) { }
            m_CapThread = null;
        }

        if (m_hCamera > 0)
        {
            m_cam.CameraUninit(m_hCamera);
            m_hCamera = 0;
        }

        m_CamState = CamState.CLOSED;
        m_textView.setText("Camera closed");
    }

    public void onClick_SetImageSize(View view){
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        if(imm != null) {
            imm.hideSoftInputFromWindow(getWindow().getDecorView().getWindowToken(), 0);
        }

        if (m_hCamera == 0)
            return;

        int hoff = 0, voff = 0, width = 0, height = 0;
        try {
            hoff = Integer.parseInt(m_etHOff.getText().toString());
            voff = Integer.parseInt(m_etVOff.getText().toString());
            width = Integer.parseInt(m_etWidth.getText().toString());
            height = Integer.parseInt(m_etHeight.getText().toString());
        } catch (NumberFormatException e) {
            return;
        }

        int status = m_cam.CameraSetImageResolution(m_hCamera, 255, hoff, voff, width, height);
        Log.i(TAG, "CameraSetImageResolution: " + status);
        UpdateResolutionUI();
    }

    public void onClick_WBOnce(View view){
        if (m_hCamera > 0) {
            int status = m_cam.CameraSetOnceWB(m_hCamera);
            Log.i(TAG, "CameraSetOnceWB: " + status);
        }
    }

    public void onClick_Snap(View view){
        m_bSnap = true;
    }

    public void onClick_SaveParam(View view) {
        if (m_hCamera > 0) {
            int status = m_cam.CameraSaveParameter(m_hCamera);
            Log.i(TAG, "CameraSaveParameter: " + status);
        }
    }

    public void onClick_SoftTrigger(View view) {
        if (m_hCamera > 0) {
            int status = m_cam.CameraSoftTrigger(m_hCamera);
            Log.i(TAG, "CameraSoftTrigger: " + status);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView Preview = (SurfaceView)findViewById(R.id.Preview);
        m_PreviewHolder = Preview.getHolder();
        m_PreviewHolder.addCallback(this);

        m_textView = (TextView)findViewById(R.id.TV);
        m_etHOff = (EditText)findViewById(R.id.editTextHOff);
        m_etVOff = (EditText)findViewById(R.id.editTextVOff);
        m_etWidth = (EditText)findViewById(R.id.editTextWidth);
        m_etHeight = (EditText)findViewById(R.id.editTextHeight);
        m_ResolutionSpinner = (Spinner)findViewById(R.id.spinnerResolution);
        m_ExptimeLabel = (TextView)findViewById(R.id.ExptimeLabel);
        m_ExptimeSeekBar = (SeekBar)findViewById(R.id.ExptimeSeekBar);
        m_GainLabel = (TextView)findViewById(R.id.GainLabel);
        m_GainSeekBar = (SeekBar)findViewById(R.id.GainSeekBar);
        m_TriggerSpinner = (Spinner)findViewById(R.id.spinnerTriggerMode);

        m_ResolutionSpinner.setOnItemSelectedListener(m_OnResolutionSpinnerSelected);
        m_ExptimeSeekBar.setOnSeekBarChangeListener(m_OnExptimeSeekBarChange);
        m_GainSeekBar.setOnSeekBarChangeListener(m_OnGainSeekBarChange);
        m_TriggerSpinner.setOnItemSelectedListener(m_OnTriggerSpinnerSelected);

        LogToFile.init(this);
    }

    @Override
    protected void onStart() {
        Log.i(TAG, "onStart");
        openCamera();
        super.onStart();
    }

    @Override
    protected void onStop() {
        Log.i(TAG, "onStop");
        closeCamera();
        super.onStop();
    }

    @Override
    protected void onRestart() {
        Log.i(TAG, "onRestart");
        //openCamera();
        super.onRestart();
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume");
        //openCamera();
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.i(TAG, "onPause");
        //closeCamera();
        super.onPause();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
        openCamera();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG, "surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
        closeCamera();
    }
}
