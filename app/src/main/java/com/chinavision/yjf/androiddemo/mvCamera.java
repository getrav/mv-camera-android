package com.chinavision.yjf.androiddemo;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class mvCamera {

    class OpenedDev {
        private UsbDevice m_Device;
        private UsbDeviceConnection m_Connection;

        public OpenedDev(UsbDevice device) {
            m_Device = device;
        }

        public boolean Open() {
            UsbManager manager = (UsbManager) mContext.getSystemService(Context.USB_SERVICE);
            m_Connection = manager.openDevice(m_Device);
            return m_Connection != null;
        }

        public void Close() {
            if (m_Connection != null) {
                m_Connection.close();
                m_Connection = null;
            }
            m_Device = null;
        }

        public short GetVID() { return (short)m_Device.getVendorId(); }
        public short GetPID() { return (short)m_Device.getProductId(); }
        public int GetFd() {
            return m_Connection.getFileDescriptor();
        }
        public String GetPath() { return m_Device.getDeviceName(); }
    };

    private Context mContext;
    private mvCameraCallback mEnumerateCallback;
    private List<UsbDevice> mDeviceList;
    private List<OpenedDev> mOpenedDevList;
    private final String ACTION_USB_PERMISSION = "com.chinavision.yjf.USB_PERMISSION";

    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    boolean ok = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false);

                    if (mDeviceList.contains(device)) {
                        mDeviceList.remove(device);

                        if (ok) {
                            OpenedDev dev = new OpenedDev(device);
                            if (dev.Open()) {
                                mOpenedDevList.add(dev);
                            }
                        }

                        if (!mDeviceList.isEmpty()) {
                            RequestUsbPermission(mDeviceList.get(0));
                        } else {
                            EnumerateDeviceDone();
                        }
                    }
                }
            }
        }
    };

    public mvCamera(Context context)
    {
        mContext = context;
    }

    // 无需root
    public boolean CameraEnumerateDeviceEx2(mvCameraCallback EnumerateCallback)
    {
        if (mEnumerateCallback != null)
            return false;
        mEnumerateCallback = EnumerateCallback;

        UsbManager manager = (UsbManager) mContext.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
        Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();

        assert (mOpenedDevList == null);
        mOpenedDevList = new ArrayList<OpenedDev>();
        mDeviceList = new ArrayList<UsbDevice>();
        while(deviceIterator.hasNext()){
            UsbDevice device = deviceIterator.next();

            int vid = device.getVendorId();
            int pid = device.getProductId();

            if (vid == 0xf622 || vid == 0x080B) {
                if (!manager.hasPermission(device) ) {
                    mDeviceList.add(device);
                }
                else {
                    OpenedDev dev = new OpenedDev(device);
                    if (dev.Open() )
                        mOpenedDevList.add(dev);
                }
            }
        }

        if (!mDeviceList.isEmpty()) {
            RequestUsbPermission(mDeviceList.get(0));
        } else {
            EnumerateDeviceDone();
        }

        return true;
    }

    private void EnumerateDeviceDone()
    {
        assert (mDeviceList.isEmpty());
        assert (mEnumerateCallback != null);

        int nDev = mOpenedDevList.size();
        if (nDev > 0) {
            int[] fds = new int[nDev];
            int[] pids = new int[nDev];
            String[] paths = new String[nDev];

            int i = 0;
            for (OpenedDev dev: mOpenedDevList) {
                fds[i] = dev.GetFd();
                pids[i] = (dev.GetVID() << 16) | dev.GetPID();
                paths[i] = dev.GetPath();
                ++i;
            }

            nDev = CameraEnumerateDeviceFromOpenedDevList(nDev, fds, pids, paths);
        }

        for (OpenedDev dev : mOpenedDevList) {
            dev.Close();
        }
        mOpenedDevList = null;

        mEnumerateCallback.onEnumerateDeviceCompleted(nDev);
        mEnumerateCallback = null;
    }

    private boolean RequestUsbPermission(UsbDevice device) {
        UsbManager manager = (UsbManager) mContext.getSystemService(Context.USB_SERVICE);
        PendingIntent mPermissionIntent = PendingIntent.getBroadcast(mContext, 0,
                new Intent(ACTION_USB_PERMISSION), 0);
        mContext.registerReceiver(mUsbReceiver, new IntentFilter(ACTION_USB_PERMISSION));
        manager.requestPermission(device, mPermissionIntent);
        return true;
    }

    // android4 root
    // android5+ 有usb访问权限
    public native int CameraEnumerateDeviceEx();

    private native int CameraEnumerateDeviceFromOpenedDevList(int DevNum, int[] fds, int[] pids, String[] paths);

    public native int CameraInitEx(int iDeviceIndex, int iParamLoadMode, int emTeam);
    public native int CameraUninit(int hCamera);

    public native Bitmap CameraCaptureImage(int hCamera);
    public native int[] CameraCapture(int hCamera, Object surface);

    public native int[] CameraGetResolutionRange(int hCamera);
    public native String[] CameraGetPresetResolutions(int hCamera);
    public native int CameraSetImageResolution(int hCamera, int index, int hoff, int voff, int width, int height);
    public native int[] CameraGetImageResolution(int hCamera);

    public native double CameraGetExposureLineTime(int hCamera);
    public native int[] CameraGetExposureTimeRange(int hCamera);
    public native int CameraSetExposureTime(int hCamera, double time);
    public native double CameraGetExposureTime(int hCamera);

    public native float CameraGetAnalogGainStep(int hCamera);
    public native int[] CameraGetAnalogGainRange(int hCamera);
    public native int CameraSetAnalogGain(int hCamera, int gain);
    public native int CameraGetAnalogGain(int hCamera);

    public native int CameraSetTriggerMode(int hCamera, int mode);
    public native int CameraGetTriggerMode(int hCamera);
    public native int CameraSoftTrigger(int hCamera);

    public native int[] CameraGetFrameStatistic(int hCamera);
    public native int CameraSetOnceWB(int hCamera);
    public native int CameraSaveParameter(int hCamera);
    public native int CameraSaveParameterToFile(int hCamera, String fpath);
    public native int CameraReadParameterFromFile(int hCamera, String fpath);

    public native int CameraCommonCall(int hCamera, String call);

    static {
        System.loadLibrary("mvCamera");
    }
}
