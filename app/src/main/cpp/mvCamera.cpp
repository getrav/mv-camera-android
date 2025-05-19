#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <CameraDefine.h>
#include <memory.h>
#include "CameraApi.h"

#include <android/log.h>
#define TAG                "mvCamera"

#define ENABLE_TIMER		0
#define ENABLE_SYSLOG		0

#if ENABLE_SYSLOG
#define LOGI(...)   __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...)   __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGI(...)
#define LOGD(...)
#endif

#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#if ENABLE_TIMER
#include <time.h>
class MvTimer
{
public:
    MvTimer() { Reset(); }
    static unsigned long GetTickCount () {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC,&ts);                      //此处可以判断一下返回值
        return  (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));
    }
    void Reset() { m_begin = GetTickCount(); }
    void PrintElapseTime(char const* text) { uint32_t Elapse = GetTickCount() - m_begin; LOGD("%s: UseTime:%u ms", text, Elapse); }
    uint32_t m_begin;
};
#else
class MvTimer
{
public:
    MvTimer() {}
    void Reset() {}
    void PrintElapseTime(char const* text) {}
};
#endif


tSdkCameraDevInfo   g_CameraEnumList[32];
int                 g_CameraCount;


static int SetupCameraParams(CameraHandle hCamera)
{
    //CameraSetTriggerMode(hCamera, 0);
    CameraSetTriggerCount(hCamera, 1);

    CameraSetAeState(hCamera, FALSE);
    //CameraSetExposureTime(hCamera, 10 * 1000);

    tSdkCameraCapbility tCapability;
    CameraGetCapability(hCamera, &tCapability);

    CameraSetMediaType(hCamera, 0);
    CameraSetFrameSpeed(hCamera, tCapability.iFrameSpeedDesc - 1);

    if (tCapability.sIspCapacity.bMonoSensor) {
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }
    else{
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_RGBA8);
    }

    CameraPlay(hCamera);
    return 0;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraEnumerateDeviceEx(JNIEnv *, jobject)
{
    system("su -c 'chmod -R 777 /dev/bus/usb'");

    g_CameraCount = sizeof(g_CameraEnumList) / sizeof(g_CameraEnumList[0]);

    MvTimer timer;
    CameraEnumerateDevice(g_CameraEnumList, &g_CameraCount);
    timer.PrintElapseTime("CameraEnumerateDevice");

    return g_CameraCount;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraInitEx(JNIEnv *, jobject,
                                                           jint iDeviceIndex, jint iParamLoadMode, jint emTeam)
{
    CameraHandle hCamera = 0;
    int status;
    if (iDeviceIndex >= 0 && iDeviceIndex < g_CameraCount) {
        MvTimer timer;
        status = CameraInit(&g_CameraEnumList[iDeviceIndex], iParamLoadMode, emTeam, &hCamera);
        timer.PrintElapseTime("CameraInit");
    }
    else {
        status = CAMERA_STATUS_NO_DEVICE_FOUND;
    }
    if (status == CAMERA_STATUS_SUCCESS) {
        MvTimer timer;
        SetupCameraParams(hCamera);
        timer.PrintElapseTime("SetupCameraParams");
    }
    else {
        LOGE("CameraInit Failed: %d", status);
    }
    return  status == CAMERA_STATUS_SUCCESS ? hCamera : 0;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraUninit(JNIEnv *env, jobject instance,
                                                           jint hCamera) {
    MvTimer timer;
    int ret = CameraUnInit(hCamera);
    timer.PrintElapseTime("CameraUnInit");
    return ret;
}

static uint16_t _RGB_2_565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((uint16_t(r) << 8) & 0xF800) |
                    ((uint16_t(g) << 3) & 0x7E0)  |
                    ((uint16_t(b) >> 3)));
}

static void _Mono8ToRGB565(BYTE const* pMono8, BYTE* pRGB565, int32_t StrideBytes, int w, int h)
{
    if (StrideBytes == w * 2) {
        BYTE const* pMono8End = pMono8 + w * h;
        uint16_t* dst = (uint16_t*)pRGB565;
        while (pMono8 < pMono8End)
        {
            BYTE gray = *pMono8++;
            *dst++ = _RGB_2_565(gray, gray, gray);
        }
    } else {
        for (int i = 0; i < h; ++i) {
            uint16_t* dst = (uint16_t*)(pRGB565 + StrideBytes * i);
            for (int j = 0; j < w; ++j) {
                BYTE gray = *pMono8++;
                *dst++ = _RGB_2_565(gray, gray, gray);
            }
        }
    }
}

static void _Mono8ToARGB32(BYTE const* pMono8, BYTE* pARGB32, int32_t StrideBytes, int w, int h)
{
    if (StrideBytes == w * 4) {
        BYTE const* pMono8End = pMono8 + w * h;
        while (pMono8 < pMono8End)
        {
            BYTE gray = *pMono8++;
            *pARGB32++ = gray;
            *pARGB32++ = gray;
            *pARGB32++ = gray;
            *pARGB32++ = 0xFF;
        }
    } else {
        for (int i = 0; i < h; ++i) {
            BYTE *dst = pARGB32 + StrideBytes * i;
            for (int j = 0; j < w; ++j) {
                BYTE gray = *pMono8++;
                *dst++ = gray;
                *dst++ = gray;
                *dst++ = gray;
                *dst++ = 0xFF;
            }
        }
    }
}

static void _BGR24ToARGB32(BYTE const* pBGR24, BYTE* pARGB32, int w, int h)
{
    for (int i = 0; i < w * h; ++i)
    {
        *pARGB32++ = pBGR24[0];
        *pARGB32++ = pBGR24[1];
        *pARGB32++ = pBGR24[2];
        *pARGB32++ = 0xFF;
        pBGR24 += 3;
    }
}

static uint32_t CopyFrameToRGB565Buffer(uint8_t* pRGB565, int32_t StrideBytes, uint8_t* pImageData, tSdkFrameHead const* pHead)
{
    if (pHead->uiMediaType == CAMERA_MEDIA_TYPE_MONO8) {
        _Mono8ToRGB565(pImageData, pRGB565, StrideBytes, pHead->iWidth, pHead->iHeight);
    }
    else {
        int ImageLineBytes = pHead->iWidth * 4;
        for (int i = 0; i < pHead->iHeight; ++i)
        {
            uint16_t* dst = (uint16_t*)(pRGB565 + i * StrideBytes);
            uint8_t* src = pImageData + i * ImageLineBytes;
            for (int j = 0; j < pHead->iWidth; ++j)
            {
                *dst = _RGB_2_565(src[0], src[1], src[2]);
                dst++;
                src += 4;
            }
        }
    }
    return 0;
}

static uint32_t CopyFrameToRGBABuffer(uint8_t* pRGBA, int32_t StrideBytes, uint8_t* pImageData, tSdkFrameHead const* pHead)
{
    if (pHead->uiMediaType == CAMERA_MEDIA_TYPE_MONO8) {
        _Mono8ToARGB32(pImageData, pRGBA, StrideBytes, pHead->iWidth, pHead->iHeight);
    }
    else {
        int ImageLineBytes = pHead->iWidth * 4;
        if (ImageLineBytes == StrideBytes) {
            memcpy(pRGBA, pImageData, ImageLineBytes * pHead->iHeight);
        } else {
            for (int i = 0; i < pHead->iHeight; ++i) {
                memcpy(pRGBA + i * StrideBytes,
                       pImageData + i * ImageLineBytes,
                       ImageLineBytes);
            }
        }
    }
    return 0;
}

static uint32_t DrawImageToSurface(ANativeWindow* nativeWindow, uint8_t* pImageData, tSdkFrameHead const* pHead)
{
    uint32_t err = ANativeWindow_setBuffersGeometry(nativeWindow, pHead->iWidth, pHead->iHeight, WINDOW_FORMAT_RGBA_8888);
    if (err != 0) {
        LOGE("ANativeWindow_setBuffersGeometry error: %d", err);
        return err;
    }

    // lock native window buffer
    ANativeWindow_Buffer windowBuffer;
    err = ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
    if (err == 0) {
        CopyFrameToRGBABuffer((BYTE *) windowBuffer.bits, windowBuffer.stride * 4, pImageData, pHead);
        ANativeWindow_unlockAndPost(nativeWindow);
    }
    else {
        LOGE("ANativeWindow_lock error: %d", err);
    }
    return err;
}

enum BitmapFormat
{
    BMP_ARGB_8888,
    BMP_RGB_565,
};

static jobject CreateBitmapFromFrame(JNIEnv *env, uint8_t* pFrameData, tSdkFrameHead const* pHead, BitmapFormat bmpFormat)
{
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jstring configName;
    if (bmpFormat == BMP_RGB_565)
    {
        configName = env->NewStringUTF("RGB_565");
    }
    else
    {
        configName = env->NewStringUTF("ARGB_8888");
    }

    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass,
                                                                   "valueOf",
                                                                   "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction,
                                                       configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction,
                                                    pHead->iWidth, pHead->iHeight, bitmapConfig);

    if (newBitmap != NULL)
    {
        //
        // putting the pixels into the new bitmap:
        //
        int ret = 0;
        void *bitmapPixels = NULL;
        if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) >= 0) {
            if (bmpFormat == BMP_RGB_565)
            {
                CopyFrameToRGB565Buffer((uint8_t*)bitmapPixels, pHead->iWidth * 2, pFrameData, pHead);
            }
            else
            {
                CopyFrameToRGBABuffer((uint8_t*)bitmapPixels, pHead->iWidth * 4, pFrameData, pHead);
            }

            AndroidBitmap_unlockPixels(env, newBitmap);
        }
    }

    return newBitmap;
}

static BYTE* CaptureFrame(int hCamera, tSdkFrameHead* OutHead, uint32_t Timeout)
{
    BYTE* pRawData = NULL;
    BYTE* pFrameBuffer = NULL;
    int status;

    do
    {
        status = CameraGetImageBuffer(hCamera, OutHead, &pRawData, Timeout);
        if (status != CAMERA_STATUS_SUCCESS) {
            break;
        }

        pFrameBuffer = (BYTE*)CameraAlignMalloc(OutHead->iWidth * OutHead->iHeight * 4, 16);
        if (pFrameBuffer == NULL) {
            LOGE("alloc frame buffer failed!!!!");
            status = CAMERA_STATUS_NO_MEMORY;
            break;
        }

        MvTimer timer;
        status = CameraImageProcess(hCamera, pRawData, pFrameBuffer, OutHead);
        timer.PrintElapseTime("CameraImageProcess");
        if (status != CAMERA_STATUS_SUCCESS) {
            LOGE("CameraImageProcess error: %d", status);
            break;
        }

    } while (0);

    if (pRawData != NULL)
        CameraReleaseImageBuffer(hCamera, pRawData);

    if (status != CAMERA_STATUS_SUCCESS) {
        CameraAlignFree(pFrameBuffer);
        pFrameBuffer = NULL;
    }
    return pFrameBuffer;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraCapture(JNIEnv *env, jobject instance, jint hCamera,
                                                        jobject surface) {
    tSdkFrameHead FrameHead = { 0 };
    BYTE* pFrameBuffer = NULL;
    ANativeWindow* nativeWindow = NULL;
    int status = CAMERA_STATUS_SUCCESS;

    do
    {
        // 从相机取图
        MvTimer timer;
        pFrameBuffer = CaptureFrame(hCamera, &FrameHead, 200);
        if (pFrameBuffer == NULL) {
            status = CAMERA_STATUS_TIME_OUT;
            break;
        }
        timer.PrintElapseTime("CaptureFrame");

        // 显示到Surface
        timer.Reset();
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        if (nativeWindow == NULL) {
            status = CAMERA_STATUS_NO_MEMORY;
            break;
        }
        DrawImageToSurface(nativeWindow, pFrameBuffer, &FrameHead);
        timer.PrintElapseTime("DrawImageToSurface");

    } while (0);

    if (nativeWindow != NULL)
        ANativeWindow_release(nativeWindow);

    CameraAlignFree(pFrameBuffer);
    pFrameBuffer = NULL;

    jint tmpResult[3] = { status, FrameHead.iWidth, FrameHead.iHeight };
    jintArray result = env->NewIntArray(sizeof(tmpResult) / sizeof(tmpResult[0]));
    if (result != NULL)
    {
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), tmpResult);
    }
    return result;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jobject JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraCaptureImage(JNIEnv *env, jobject instance,
                                                                 jint hCamera) {
    tSdkFrameHead FrameHead;
    BYTE* pFrameBuffer = CaptureFrame(hCamera, &FrameHead, 200);
    if (pFrameBuffer == NULL)
        return NULL;

    MvTimer timer;
    jobject newBitmap = CreateBitmapFromFrame(env, pFrameBuffer, &FrameHead, BMP_RGB_565);
    timer.PrintElapseTime("CreateBitmapFromFrame");
    CameraAlignFree(pFrameBuffer);
    return newBitmap;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetFrameStatistic(JNIEnv *env, jobject instance,
                                                                      jint hCamera) {
    tSdkFrameStatistic stat = { 0 };
    CameraGetFrameStatistic(hCamera, &stat);

    jintArray result = env->NewIntArray(sizeof(stat) / sizeof(INT));
    if (result != NULL)
    {
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), (jint*)&stat);
    }
    return result;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetResolutionRange(JNIEnv *env, jobject instance,
                                                                       jint hCamera) {
    tSdkCameraCapbility tCapability = { 0 };
    if (CameraGetCapability(hCamera, &tCapability) != CAMERA_STATUS_SUCCESS)
        return NULL;

    jintArray result = env->NewIntArray(2);
    if (result != NULL)
    {
        jint range[2] = { tCapability.sResolutionRange.iWidthMax, tCapability.sResolutionRange.iHeightMax };
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), range);
    }
    return result;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSetImageResolution(JNIEnv *env, jobject instance,
                                                                       jint hCamera, jint index, jint hoff, jint voff, jint width, jint height) {
    tSdkImageResolution res = { 0 };
    res.iIndex = index;
    res.iHOffsetFOV = hoff;
    res.iVOffsetFOV = voff;
    res.iWidth = res.iWidthFOV = width;
    res.iHeight = res.iHeightFOV = height;
    return CameraSetImageResolution(hCamera, &res);
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraEnumerateDeviceFromOpenedDevList(JNIEnv *env, jobject instance,
                                                                     jint DevNum, jintArray fds_,
                                                                     jintArray pids_, jobjectArray paths_) {
    jint *fds = env->GetIntArrayElements(fds_, NULL);
    jint *pids = env->GetIntArrayElements(pids_, NULL);
    char const** paths = (char const**)malloc(DevNum * sizeof(char*));
    BOOL bOK = TRUE;

    memset(paths, 0, DevNum * sizeof(char*));
    for (int i = 0; i < DevNum; ++i)
    {
        jstring jstr = (jstring)env->GetObjectArrayElement(paths_, i);
        paths[i] = env->GetStringUTFChars(jstr, NULL);
        env->DeleteLocalRef(jstr);
        if (paths[i] == NULL)
        {
            bOK = FALSE;
            break;
        }
    }

    if (bOK)
    {
        g_CameraCount = sizeof(g_CameraEnumList) / sizeof(g_CameraEnumList[0]);
        CameraEnumerateDeviceFromOpenedDevList(g_CameraEnumList, &g_CameraCount, DevNum, fds, pids, paths);
    }
    else
    {
        g_CameraCount = 0;
    }

    env->ReleaseIntArrayElements(fds_, fds, 0);
    env->ReleaseIntArrayElements(pids_, pids, 0);
    for (int i = 0; i < DevNum; ++i)
    {
        if (paths[i] != NULL)
        {
            jstring jstr = (jstring)env->GetObjectArrayElement(paths_, i);
            env->ReleaseStringUTFChars(jstr, paths[i]);
            env->DeleteLocalRef(jstr);
        }
    }
    free(paths);
    return g_CameraCount;
}

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSetOnceWB(JNIEnv *env, jobject instance,
                                                          jint hCamera) {
    return CameraSetOnceWB(hCamera);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetPresetResolutions(JNIEnv *env,
                                                                         jobject instance,
                                                                         jint hCamera) {
    tSdkCameraCapbility cap = { 0 };
    int status = CameraGetCapability(hCamera, &cap);
    if (status != CAMERA_STATUS_SUCCESS)
        return NULL;

    int PresetResolutionNum = cap.iImageSizeDesc;
    if (PresetResolutionNum < 1)
        return NULL;

    if (PresetResolutionNum > 32)
        PresetResolutionNum = 32;

    jobjectArray ret = (jobjectArray)env->NewObjectArray(PresetResolutionNum,
                        env->FindClass("java/lang/String"),
                        env->NewStringUTF(""));

    for (int i = 0; i < PresetResolutionNum; i++) {
        env->SetObjectArrayElement(
                ret, i, env->NewStringUTF(cap.pImageSizeDesc[i].acDescription));
    }
    return ret;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetExposureLineTime(JNIEnv *env,
                                                                        jobject instance,
                                                                        jint hCamera) {
    double LineTime = 0;
    CameraGetExposureLineTime(hCamera, &LineTime);
    return LineTime;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetExposureTimeRange(JNIEnv *env,
                                                                         jobject instance,
                                                                         jint hCamera) {
    tSdkCameraCapbility cap = { 0 };
    int status = CameraGetCapability(hCamera, &cap);
    if (status != CAMERA_STATUS_SUCCESS)
        return NULL;

    jintArray result = env->NewIntArray(2);
    if (result != NULL)
    {
        jint range[2] = { (int)cap.sExposeDesc.uiExposeTimeMin, (int)cap.sExposeDesc.uiExposeTimeMax };
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), range);
    }
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSetExposureTime(JNIEnv *env, jobject instance,
                                                                    jint hCamera, jdouble time) {
    return CameraSetExposureTime(hCamera, time);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetExposureTime(JNIEnv *env, jobject instance,
                                                                    jint hCamera) {
    double time = 0;
    CameraGetExposureTime(hCamera, &time);
    return time;
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetAnalogGainStep(JNIEnv *env, jobject instance,
                                                                      jint hCamera) {
    float step = 0;
    tSdkCameraCapbility cap = { 0 };
    int status = CameraGetCapability(hCamera, &cap);
    if (status == CAMERA_STATUS_SUCCESS) {
        step = cap.sExposeDesc.fAnalogGainStep;
    }
    return step;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetAnalogGainRange(JNIEnv *env,
                                                                       jobject instance,
                                                                       jint hCamera) {
    tSdkCameraCapbility cap = { 0 };
    int status = CameraGetCapability(hCamera, &cap);
    if (status != CAMERA_STATUS_SUCCESS)
        return NULL;

    jintArray result = env->NewIntArray(2);
    if (result != NULL)
    {
        jint range[2] = { (int)cap.sExposeDesc.uiAnalogGainMin, (int)cap.sExposeDesc.uiAnalogGainMax };
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), range);
    }
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSetAnalogGain(JNIEnv *env, jobject instance,
                                                                  jint hCamera, jint gain) {
    return CameraSetAnalogGain(hCamera, gain);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetAnalogGain(JNIEnv *env, jobject instance,
                                                                  jint hCamera) {
    int gain = 0;
    CameraGetAnalogGain(hCamera, &gain);
    return gain;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSaveParameter(JNIEnv *env, jobject instance,
                                                                  jint hCamera) {
    return CameraSaveParameter(hCamera, 0);
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetImageResolution(JNIEnv *env,
                                                                       jobject instance,
                                                                       jint hCamera) {
    tSdkImageResolution res = { 0 };
    int status = CameraGetImageResolution(hCamera, &res);
    if (status != CAMERA_STATUS_SUCCESS)
        return NULL;

    jintArray result = env->NewIntArray(5);
    if (result != NULL)
    {
        jint values[5] = { res.iIndex, res.iHOffsetFOV, res.iVOffsetFOV, res.iWidth, res.iHeight };
        env->SetIntArrayRegion(result, 0, env->GetArrayLength(result), values);
    }
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSetTriggerMode(JNIEnv *env, jobject instance,
                                                                   jint hCamera, jint mode) {
    return CameraSetTriggerMode(hCamera, mode);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSoftTrigger(JNIEnv *env, jobject instance,
                                                                jint hCamera) {
    return CameraSoftTrigger(hCamera);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraGetTriggerMode(JNIEnv *env, jobject instance,
                                                                   jint hCamera) {
    int mode = 0;
    int status = CameraGetTriggerMode(hCamera, &mode);
    if (status == CAMERA_STATUS_SUCCESS)
        return  mode;
    else
        return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraCommonCall(JNIEnv *env, jobject instance,
                                                               jint hCamera, jstring call_) {
    const char *call = env->GetStringUTFChars(call_, 0);
    int status = CameraCommonCall(hCamera, call, NULL, 0);
    env->ReleaseStringUTFChars(call_, call);
    return status;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraSaveParameterToFile(JNIEnv *env,
                                                                        jobject instance,
                                                                        jint hCamera,
                                                                        jstring fpath_) {
    const char *fpath = env->GetStringUTFChars(fpath_, 0);
    int status = CameraSaveParameterToFile(hCamera, (char*)fpath);
    env->ReleaseStringUTFChars(fpath_, fpath);
    return status;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_chinavision_yjf_androiddemo_mvCamera_CameraReadParameterFromFile(JNIEnv *env,
                                                                          jobject instance,
                                                                          jint hCamera,
                                                                          jstring fpath_) {
    const char *fpath = env->GetStringUTFChars(fpath_, 0);
    int status = CameraReadParameterFromFile(hCamera, (char*)fpath);
    env->ReleaseStringUTFChars(fpath_, fpath);
    return status;
}
