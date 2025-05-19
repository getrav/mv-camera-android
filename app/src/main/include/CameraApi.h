#ifndef _MVCAMAPI_H_
#define _MVCAMAPI_H_

#define MVSDK_API

#ifdef __cplusplus
extern "C" {
#endif


#include "CameraDefine.h"
#include "CameraStatus.h"



MVSDK_API CameraSdkStatus     CameraSdkInit(int     iLanguageSel);

/******************************************************/
// 函数名   : CameraSetDataDirectory
// 功能描述 : 设置相机数据文件的存储目录（.config .mvdat等）
//            需要在CameraInit打开相机前设置好
//            默认目录为/mnt/sdcard
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus CameraSetDataDirectory(char const* dirname);


MVSDK_API CameraSdkStatus  CameraEnumerateDevice(
    tSdkCameraDevInfo* pCameraList,
    INT*               piNums
);

MVSDK_API CameraSdkStatus  CameraEnumerateDeviceFromOpenedDevList(
        tSdkCameraDevInfo* pCameraList,
        INT*               piNums,
        INT                nDev,
        INT*               fd_list,
        INT*               pid_list,
        char const**      path_list
);

MVSDK_API CameraSdkStatus  CameraIdleStateDevice(
    tSdkCameraDevInfo* pCameraList,
    INT*               piNums
);


MVSDK_API INT  CameraEnumerateDeviceEx(
);


MVSDK_API CameraSdkStatus  CameraIsOpened(
  tSdkCameraDevInfo*  pCameraList,
  BOOL*               pOpened
);


MVSDK_API CameraSdkStatus  CameraInit(
    tSdkCameraDevInfo*  pCameraInfo,
    int                 emParamLoadMode,
    int                 emTeam,
    CameraHandle*       pCameraHandle
);

MVSDK_API CameraSdkStatus  CameraInitEx(
    int             iDeviceIndex,
    int             iParamLoadMode,
    int             emTeam,
    CameraHandle*   pCameraHandle
);

MVSDK_API CameraSdkStatus  CameraSetCallbackFunction(
    CameraHandle        hCamera,
    CAMERA_SNAP_PROC    pCallBack,
    PVOID               pContext,
    CAMERA_SNAP_PROC*   pCallbackOld
);

MVSDK_API CameraSdkStatus  CameraUnInit(
    CameraHandle hCamera
);

MVSDK_API CameraSdkStatus  CameraGetInformation(
    CameraHandle    hCamera,
    char**          pbuffer
);

MVSDK_API CameraSdkStatus  CameraImageProcess(
    CameraHandle        hCamera,
    BYTE*               pbyIn,
    BYTE*               pbyOut,
    tSdkFrameHead*      pFrInfo
);

MVSDK_API CameraSdkStatus CameraImageProcessEx(
    CameraHandle hCamera,
    BYTE *pbyIn,
    BYTE *pbyOut,
    tSdkFrameHead *pFrInfo,
    UINT uOutFormat,
    UINT uReserved
);

MVSDK_API CameraSdkStatus  CameraDisplayInit(
    CameraHandle    hCamera,
    HWND            hWndDisplay
);

MVSDK_API CameraSdkStatus  CameraDisplayRGB24(
    CameraHandle        hCamera,
    BYTE*               pbyRGB24,
    tSdkFrameHead*      pFrInfo
);

MVSDK_API CameraSdkStatus  CameraSetDisplayMode(
    CameraHandle    hCamera,
    INT             iMode
);

MVSDK_API CameraSdkStatus  CameraSetDisplayOffset(
    CameraHandle    hCamera,
    int             iOffsetX,
    int             iOffsetY
);

MVSDK_API CameraSdkStatus  CameraSetDisplaySize(
    CameraHandle    hCamera,
    INT             iWidth,
    INT             iHeight
);

MVSDK_API CameraSdkStatus  CameraGetImageBuffer(
    CameraHandle        hCamera,
    tSdkFrameHead*      pFrameInfo,
    BYTE**              pbyBuffer,
    UINT                wTimes
);

MVSDK_API unsigned char*  CameraGetImageBufferEx(
    CameraHandle        hCamera,
    INT*                piWidth,
    INT*                piHeight,
    UINT                wTimes
);


MVSDK_API CameraSdkStatus  CameraSnapToBuffer(
    CameraHandle        hCamera,
    tSdkFrameHead*      pFrameInfo,
    BYTE**              pbyBuffer,
    UINT                uWaitTimeMs
);

MVSDK_API CameraSdkStatus  CameraReleaseImageBuffer(
    CameraHandle    hCamera,
    BYTE*           pbyBuffer
);

MVSDK_API CameraSdkStatus  CameraPlay(
    CameraHandle hCamera
);

MVSDK_API CameraSdkStatus  CameraPause(
    CameraHandle hCamera
);

MVSDK_API CameraSdkStatus  CameraStop(
    CameraHandle hCamera
);

MVSDK_API CameraSdkStatus  CameraInitRecord(
    CameraHandle    hCamera,
    int             iFormat,
    char*           pcSavePath,
    BOOL            b2GLimit,
    DWORD           dwQuality,
    int             iFrameRate
);

MVSDK_API CameraSdkStatus  CameraStopRecord(
    CameraHandle    hCamera
);

MVSDK_API CameraSdkStatus  CameraPushFrame(
    CameraHandle    hCamera,
    BYTE*           pbyImageBuffer,
    tSdkFrameHead*  pFrInfo
);

MVSDK_API CameraSdkStatus  CameraSaveImage(
    CameraHandle    hCamera,
    char*           lpszFileName,
    BYTE*           pbyImageBuffer,
    tSdkFrameHead*  pFrInfo,
    BYTE            byFileType,
    BYTE            byQuality
);

MVSDK_API CameraSdkStatus  CameraGetImageResolution(
    CameraHandle            hCamera,
    tSdkImageResolution*    psCurVideoSize
);

MVSDK_API CameraSdkStatus  CameraSetImageResolution(
    CameraHandle            hCamera,
    tSdkImageResolution*    pImageResolution
);

MVSDK_API CameraSdkStatus  CameraGetMediaType(
    CameraHandle    hCamera,
    INT*            piMediaType
);

MVSDK_API CameraSdkStatus  CameraSetMediaType(
    CameraHandle    hCamera,
    INT             iMediaType
);

MVSDK_API CameraSdkStatus  CameraSetAeState(
    CameraHandle    hCamera,
    BOOL            bAeState
);

MVSDK_API CameraSdkStatus  CameraGetAeState(
    CameraHandle    hCamera,
    BOOL*           pAeState
);

MVSDK_API CameraSdkStatus  CameraSetSharpness(
    CameraHandle    hCamera,
    int             iSharpness
);

MVSDK_API CameraSdkStatus  CameraGetSharpness(
    CameraHandle    hCamera,
    int*            piSharpness
);

MVSDK_API CameraSdkStatus  CameraSetLutMode(
    CameraHandle    hCamera,
    int             emLutMode
);

MVSDK_API CameraSdkStatus  CameraGetLutMode(
    CameraHandle    hCamera,
    int*            pemLutMode
);

MVSDK_API CameraSdkStatus  CameraSelectLutPreset(
    CameraHandle    hCamera,
    int             iSel
);

MVSDK_API CameraSdkStatus  CameraGetLutPresetSel(
    CameraHandle    hCamera,
    int*            piSel
);

MVSDK_API CameraSdkStatus  CameraSetCustomLut(
    CameraHandle    hCamera,
    int       iChannel,
    USHORT*         pLut
);

MVSDK_API CameraSdkStatus  CameraGetCustomLut(
    CameraHandle    hCamera,
    int       iChannel,
    USHORT*         pLut
);

MVSDK_API CameraSdkStatus  CameraGetCurrentLut(
    CameraHandle    hCamera,
    int       iChannel,
    USHORT*         pLut
);

MVSDK_API CameraSdkStatus  CameraSetWbMode(
    CameraHandle    hCamera,
    BOOL            bAuto
);

MVSDK_API CameraSdkStatus  CameraGetWbMode(
    CameraHandle    hCamera,
    BOOL*           pbAuto
);

MVSDK_API CameraSdkStatus  CameraSetPresetClrTemp(
    CameraHandle    hCamera,
    int             iSel
);

MVSDK_API CameraSdkStatus  CameraGetPresetClrTemp(
    CameraHandle    hCamera,
    int*            piSel
);

MVSDK_API CameraSdkStatus  CameraSetUserClrTempGain(
  CameraHandle  hCamera,
  int       iRgain,
  int       iGgain,
  int       iBgain
);


MVSDK_API CameraSdkStatus  CameraGetUserClrTempGain(
  CameraHandle  hCamera,
  int*      piRgain,
  int*      piGgain,
  int*      piBgain
);

MVSDK_API CameraSdkStatus  CameraSetUserClrTempMatrix(
  CameraHandle  hCamera,
  float*      pMatrix
);


MVSDK_API CameraSdkStatus  CameraGetUserClrTempMatrix(
  CameraHandle  hCamera,
  float*      pMatrix
);

MVSDK_API CameraSdkStatus  CameraSetClrTempMode(
  CameraHandle  hCamera,
  int       iMode
);

MVSDK_API CameraSdkStatus  CameraGetClrTempMode(
  CameraHandle  hCamera,
  int*      pimode
);



MVSDK_API CameraSdkStatus  CameraSetOnceWB(
    CameraHandle    hCamera
);

MVSDK_API CameraSdkStatus  CameraSetOnceBB(
    CameraHandle    hCamera
);


MVSDK_API CameraSdkStatus  CameraSetAeTarget(
    CameraHandle    hCamera,
    int             iAeTarget
);

MVSDK_API CameraSdkStatus  CameraGetAeTarget(
    CameraHandle    hCamera,
    int*            piAeTarget
);

MVSDK_API CameraSdkStatus CameraSetAeExposureRange(
	CameraHandle    hCamera,
	double          fMinExposureTime,
	double			fMaxExposureTime
	);

MVSDK_API CameraSdkStatus CameraGetAeExposureRange(
	CameraHandle    hCamera,
	double*         fMinExposureTime,
	double*			fMaxExposureTime
	);

MVSDK_API CameraSdkStatus CameraSetAeAnalogGainRange(
	CameraHandle    hCamera,
	int				iMinAnalogGain,
	int				iMaxAnalogGain
	);

MVSDK_API CameraSdkStatus CameraGetAeAnalogGainRange(
	CameraHandle    hCamera,
	int*			iMinAnalogGain,
	int*			iMaxAnalogGain
	);

MVSDK_API CameraSdkStatus CameraSetAeThreshold(
	CameraHandle    hCamera, 
	int				iThreshold
	);

MVSDK_API CameraSdkStatus CameraGetAeThreshold(
	CameraHandle    hCamera, 
	int*			iThreshold
	);

MVSDK_API CameraSdkStatus  CameraSetExposureTime(
    CameraHandle    hCamera,
    double          fExposureTime
);


MVSDK_API CameraSdkStatus  CameraGetExposureLineTime(
    CameraHandle    hCamera,
    double*         pfLineTime
);

MVSDK_API CameraSdkStatus  CameraGetExposureTime(
    CameraHandle    hCamera,
    double*         pfExposureTime
);

MVSDK_API CameraSdkStatus  CameraSetAnalogGain(
    CameraHandle    hCamera,
    INT             iAnalogGain
);

MVSDK_API CameraSdkStatus  CameraGetAnalogGain(
    CameraHandle    hCamera,
    INT*            piAnalogGain
);

MVSDK_API CameraSdkStatus  CameraSetGain(
    CameraHandle    hCamera,
    int             iRGain,
    int             iGGain,
    int             iBGain
);


MVSDK_API CameraSdkStatus  CameraGetGain(
    CameraHandle    hCamera,
    int*            piRGain,
    int*            piGGain,
    int*            piBGain
);


MVSDK_API CameraSdkStatus  CameraSetGamma(
    CameraHandle    hCamera,
    int             iGamma
);

MVSDK_API CameraSdkStatus  CameraGetGamma(
    CameraHandle    hCamera,
    int*            piGamma
);

MVSDK_API CameraSdkStatus  CameraSetContrast(
    CameraHandle    hCamera,
    int             iContrast
);

MVSDK_API CameraSdkStatus  CameraGetContrast(
    CameraHandle    hCamera,
    int*            piContrast
);

MVSDK_API CameraSdkStatus  CameraSetSaturation(
    CameraHandle    hCamera,
    int             iSaturation
);

MVSDK_API CameraSdkStatus  CameraGetSaturation(
    CameraHandle    hCamera,
    int*            piSaturation
);

MVSDK_API CameraSdkStatus  CameraSetMonochrome(
    CameraHandle    hCamera,
    BOOL            bEnable
);

MVSDK_API CameraSdkStatus  CameraGetMonochrome(
    CameraHandle    hCamera,
    BOOL*           pbEnable
);

MVSDK_API CameraSdkStatus  CameraSetInverse(
    CameraHandle    hCamera,
    BOOL            bEnable
);

MVSDK_API CameraSdkStatus  CameraGetInverse(
    CameraHandle    hCamera,
    BOOL*           pbEnable
);

MVSDK_API CameraSdkStatus  CameraSetAntiFlick(
    CameraHandle    hCamera,
    BOOL            bEnable
);

MVSDK_API CameraSdkStatus  CameraGetAntiFlick(
    CameraHandle    hCamera,
    BOOL*           pbEnable
);

MVSDK_API CameraSdkStatus  CameraGetLightFrequency(
    CameraHandle    hCamera,
    int*            piFrequencySel
);

MVSDK_API CameraSdkStatus  CameraSetLightFrequency(
    CameraHandle    hCamera,
    int             iFrequencySel
);

MVSDK_API CameraSdkStatus  CameraSetFrameSpeed(
    CameraHandle    hCamera,
    int             iFrameSpeed
);

MVSDK_API CameraSdkStatus  CameraGetFrameSpeed(
    CameraHandle    hCamera,
    int*            piFrameSpeed
);


MVSDK_API CameraSdkStatus  CameraSetParameterMode(
    CameraHandle    hCamera,
    int             iMode
);

MVSDK_API CameraSdkStatus  CameraGetParameterMode(
    CameraHandle    hCamera,
    int*            piTarget
);

MVSDK_API CameraSdkStatus  CameraSetParameterMask(
    CameraHandle    hCamera,
    UINT            uMask
);

MVSDK_API CameraSdkStatus  CameraSaveParameter(
    CameraHandle    hCamera,
    int             iTeam
);


MVSDK_API CameraSdkStatus  CameraSaveParameterToFile(
  CameraHandle  hCamera,
  char*       sFileName
);


MVSDK_API CameraSdkStatus  CameraReadParameterFromFile(
    CameraHandle    hCamera,
    char*           sFileName
);

MVSDK_API CameraSdkStatus  CameraLoadParameter(
    CameraHandle    hCamera,
    int             iTeam
);

MVSDK_API CameraSdkStatus  CameraGetCurrentParameterGroup(
    CameraHandle    hCamera,
    int*            piTeam
);

MVSDK_API CameraSdkStatus  CameraSetTransPackLen(
    CameraHandle    hCamera,
    INT             iPackSel
);

MVSDK_API CameraSdkStatus  CameraGetTransPackLen(
    CameraHandle    hCamera,
    INT*            piPackSel
);

MVSDK_API CameraSdkStatus  CameraIsAeWinVisible(
    CameraHandle    hCamera,
    BOOL*           pbIsVisible
);

MVSDK_API CameraSdkStatus  CameraSetAeWinVisible(
    CameraHandle    hCamera,
    BOOL            bIsVisible
);

MVSDK_API CameraSdkStatus  CameraGetAeWindow(
    CameraHandle    hCamera,
    INT*            piHOff,
    INT*            piVOff,
    INT*            piWidth,
    INT*            piHeight
);

MVSDK_API CameraSdkStatus  CameraSetAeWindow(
    CameraHandle    hCamera,
    int             iHOff,
    int             iVOff,
    int             iWidth,
    int             iHeight
);

MVSDK_API CameraSdkStatus  CameraSetMirror(
    CameraHandle    hCamera,
    int             iDir,
    BOOL            bEnable
);

MVSDK_API CameraSdkStatus  CameraGetMirror(
    CameraHandle    hCamera,
    int             iDir,
    BOOL*           pbEnable
);

MVSDK_API CameraSdkStatus CameraSetRotate(
	CameraHandle    hCamera,
	int             iRot
	);

MVSDK_API CameraSdkStatus CameraGetRotate(
	CameraHandle    hCamera,
	int*            iRot
	);

MVSDK_API CameraSdkStatus  CameraGetWbWindow(
    CameraHandle    hCamera,
    INT*            PiHOff,
    INT*            PiVOff,
    INT*            PiWidth,
    INT*            PiHeight
);

MVSDK_API CameraSdkStatus  CameraSetWbWindow(
    CameraHandle    hCamera,
    INT             iHOff,
    INT             iVOff,
    INT             iWidth,
    INT             iHeight
);

MVSDK_API CameraSdkStatus  CameraIsWbWinVisible(
    CameraHandle    hCamera,
    BOOL*           pbShow
);

MVSDK_API CameraSdkStatus  CameraSetWbWinVisible(
    CameraHandle    hCamera,
    BOOL            bShow
);

MVSDK_API CameraSdkStatus  CameraImageOverlay(
    CameraHandle    hCamera,
    BYTE*           pRgbBuffer,
    tSdkFrameHead*  pFrInfo
);

MVSDK_API CameraSdkStatus  CameraSetCrossLine(
    CameraHandle    hCamera,
    int             iLine,
    INT             x,
    INT             y,
    UINT            uColor,
    BOOL            bVisible
);

MVSDK_API CameraSdkStatus  CameraGetCrossLine(
    CameraHandle    hCamera,
    INT             iLine,
    INT*            px,
    INT*            py,
    UINT*           pcolor,
    BOOL*           pbVisible
);

MVSDK_API CameraSdkStatus  CameraGetCapability(
    CameraHandle            hCamera,
    tSdkCameraCapbility*    pCameraInfo
);

MVSDK_API CameraSdkStatus  CameraGetCapabilityEx(
    char*                   sDeviceModel,
    tSdkCameraCapbility*    pCameraInfo,
    PVOID                   hCameraHandle
);


MVSDK_API CameraSdkStatus  CameraWriteSN(
    CameraHandle    hCamera,
    BYTE*           pbySN,
    INT             iLevel
);

MVSDK_API CameraSdkStatus  CameraReadSN(
    CameraHandle        hCamera,
    BYTE*               pbySN,
    INT                 iLevel
);
MVSDK_API CameraSdkStatus  CameraSetTriggerDelayTime(
    CameraHandle    hCamera,
    UINT            uDelayTimeUs
);

MVSDK_API CameraSdkStatus  CameraGetTriggerDelayTime(
    CameraHandle    hCamera,
    UINT*           puDelayTimeUs
);

MVSDK_API CameraSdkStatus  CameraSetTriggerCount(
    CameraHandle    hCamera,
    INT             iCount
);

MVSDK_API CameraSdkStatus  CameraGetTriggerCount(
    CameraHandle    hCamera,
    INT*            piCount
);

MVSDK_API CameraSdkStatus  CameraSoftTrigger(
    CameraHandle    hCamera
);

MVSDK_API CameraSdkStatus  CameraSetTriggerMode(
    CameraHandle    hCamera,
    int             iModeSel
);

MVSDK_API CameraSdkStatus  CameraGetTriggerMode(
    CameraHandle    hCamera,
    INT*            piModeSel
);

MVSDK_API CameraSdkStatus   CameraSetStrobeMode(
    CameraHandle    hCamera,
    INT             iMode
);

MVSDK_API CameraSdkStatus   CameraGetStrobeMode(
    CameraHandle    hCamera,
    INT*            piMode
);

MVSDK_API CameraSdkStatus   CameraSetStrobeDelayTime(
    CameraHandle    hCamera,
    UINT            uDelayTimeUs
);

MVSDK_API CameraSdkStatus   CameraGetStrobeDelayTime(
    CameraHandle    hCamera,
    UINT*           upDelayTimeUs
);

MVSDK_API CameraSdkStatus   CameraSetStrobePulseWidth(
    CameraHandle    hCamera,
    UINT            uTimeUs
);

MVSDK_API CameraSdkStatus   CameraGetStrobePulseWidth(
    CameraHandle    hCamera,
    UINT*           upTimeUs
);

MVSDK_API CameraSdkStatus   CameraSetStrobePolarity(
    CameraHandle    hCamera,
    INT             uPolarity
);

MVSDK_API CameraSdkStatus   CameraGetStrobePolarity(
    CameraHandle    hCamera,
    INT*            upPolarity
);


MVSDK_API CameraSdkStatus   CameraSetExtTrigSignalType(
    CameraHandle    hCamera,
    INT             iType
);

MVSDK_API CameraSdkStatus   CameraGetExtTrigSignalType(
    CameraHandle    hCamera,
    INT*            ipType
);

MVSDK_API CameraSdkStatus   CameraSetExtTrigShutterType(
    CameraHandle    hCamera,
    INT             iType
);

MVSDK_API CameraSdkStatus   CameraGetExtTrigShutterType(
    CameraHandle    hCamera,
    INT*            ipType
);

MVSDK_API CameraSdkStatus   CameraSetExtTrigDelayTime(
    CameraHandle    hCamera,
    UINT            uDelayTimeUs
);

MVSDK_API CameraSdkStatus   CameraGetExtTrigDelayTime(
    CameraHandle    hCamera,
    UINT*           upDelayTimeUs
);

MVSDK_API CameraSdkStatus   CameraSetExtTrigJitterTime(
    CameraHandle    hCamera,
    UINT            uTimeUs
);

MVSDK_API CameraSdkStatus   CameraGetExtTrigJitterTime(
    CameraHandle    hCamera,
    UINT*           upTimeUs
);

MVSDK_API CameraSdkStatus   CameraGetExtTrigCapability(
    CameraHandle    hCamera,
    UINT*           puCapabilityMask
);


MVSDK_API CameraSdkStatus  CameraGetResolutionForSnap(
    CameraHandle            hCamera,
    tSdkImageResolution*    pImageResolution
);

MVSDK_API CameraSdkStatus  CameraSetResolutionForSnap(
    CameraHandle            hCamera,
    tSdkImageResolution*    pImageResolution
);

MVSDK_API CameraSdkStatus  CameraCustomizeResolution(
    CameraHandle            hCamera,
    tSdkImageResolution*    pImageCustom
);

MVSDK_API CameraSdkStatus  CameraCustomizeReferWin(
    CameraHandle    hCamera,
    INT             iWinType,
    HWND            hParent,
    INT*            piHOff,
    INT*            piVOff,
    INT*            piWidth,
    INT*            piHeight
);

MVSDK_API CameraSdkStatus  CameraShowSettingPage(
    CameraHandle    hCamera,
    BOOL            bShow
);

MVSDK_API CameraSdkStatus  CameraCreateSettingPage(
    CameraHandle            hCamera,
    HWND                    hParent,
    char*                   pWinText,
    CAMERA_PAGE_MSG_PROC    pCallbackFunc,
    PVOID                   pCallbackCtx,
    UINT                    uReserved
);

MVSDK_API CameraSdkStatus  CameraCreateSettingPageEx(
    CameraHandle            hCamera
);


MVSDK_API CameraSdkStatus  CameraSetActiveSettingSubPage(
    CameraHandle    hCamera,
    INT             index
);

MVSDK_API CameraSdkStatus  CameraSpecialControl(
    CameraHandle    hCamera,
    DWORD           dwCtrlCode,
    DWORD           dwParam,
    LPVOID          lpData
);

MVSDK_API CameraSdkStatus  CameraGetFrameStatistic(
    CameraHandle            hCamera,
    tSdkFrameStatistic*     psFrameStatistic
);

MVSDK_API CameraSdkStatus  CameraSetNoiseFilter(
    CameraHandle    hCamera,
    BOOL            bEnable
);

MVSDK_API CameraSdkStatus  CameraGetNoiseFilterState(
    CameraHandle    hCamera,
    BOOL*           pEnable
);

MVSDK_API CameraSdkStatus  CameraRstTimeStamp(
    CameraHandle    hCamera
);

MVSDK_API CameraSdkStatus  CameraSaveUserData(
    CameraHandle    hCamera,
    UINT            uStartAddr,
    BYTE            *pbData,
    int             ilen
);

MVSDK_API CameraSdkStatus  CameraLoadUserData(
    CameraHandle    hCamera,
    UINT            uStartAddr,
    BYTE            *pbData,
    int             ilen
);

MVSDK_API CameraSdkStatus  CameraGetFriendlyName(
  CameraHandle  hCamera,
  char*     pName
);

MVSDK_API CameraSdkStatus  CameraSetFriendlyName(
  CameraHandle  hCamera,
  char*       pName
);

MVSDK_API CameraSdkStatus  CameraSdkGetVersionString(
  char*       pVersionString
);

MVSDK_API CameraSdkStatus  CameraCheckFwUpdate(
  CameraHandle  hCamera,
  BOOL*     pNeedUpdate
);

MVSDK_API CameraSdkStatus  CameraGetFirmwareVersion(
  CameraHandle  hCamera,
  char*     pVersion
);

MVSDK_API CameraSdkStatus  CameraGetEnumInfo(
  CameraHandle    hCamera,
  tSdkCameraDevInfo*  pCameraInfo
);

MVSDK_API CameraSdkStatus  CameraGetInerfaceVersion(
  CameraHandle    hCamera,
  char*       pVersion
);

MVSDK_API CameraSdkStatus  CameraSetIOState(
  CameraHandle    hCamera,
  INT         iOutputIOIndex,
  UINT        uState
);

MVSDK_API CameraSdkStatus  CameraGetIOState(
  CameraHandle    hCamera,
  INT         iInputIOIndex,
  UINT*         puState
);



MVSDK_API CameraSdkStatus  CameraSetInPutIOMode(
	CameraHandle    hCamera,
	INT         iInputIOIndex,
	INT			iMode
	);

MVSDK_API CameraSdkStatus  CameraSetOutPutIOMode(
	CameraHandle    hCamera,
	INT         iOutputIOIndex,
	INT			iMode
	);

MVSDK_API CameraSdkStatus  CameraSetOutPutPWM(
	CameraHandle    hCamera,
	INT         iOutputIOIndex,
	UINT		iCycle,
	UINT		uDuty
	);


MVSDK_API CameraSdkStatus  CameraSetAeAlgorithm(
    CameraHandle    hCamera,
    INT             iIspProcessor,
    INT             iAeAlgorithmSel
);

MVSDK_API CameraSdkStatus  CameraGetAeAlgorithm(
    CameraHandle    hCamera,
    INT             iIspProcessor,
    INT*            piAlgorithmSel
);

MVSDK_API CameraSdkStatus  CameraSetBayerDecAlgorithm(
    CameraHandle    hCamera,
    INT             iIspProcessor,
    INT             iAlgorithmSel
);

MVSDK_API CameraSdkStatus  CameraGetBayerDecAlgorithm(
    CameraHandle    hCamera,
    INT             iIspProcessor,
    INT*            piAlgorithmSel
);

MVSDK_API CameraSdkStatus  CameraSetIspProcessor(
    CameraHandle    hCamera,
    INT             iIspProcessor
);

MVSDK_API CameraSdkStatus  CameraGetIspProcessor(
    CameraHandle    hCamera,
    INT*            piIspProcessor
);

MVSDK_API CameraSdkStatus  CameraSetBlackLevel(
  CameraHandle    hCamera,
  INT         iBlackLevel
);

MVSDK_API CameraSdkStatus  CameraGetBlackLevel(
  CameraHandle    hCamera,
  INT*        piBlackLevel
);

MVSDK_API CameraSdkStatus  CameraSetWhiteLevel(
  CameraHandle    hCamera,
  INT         iWhiteLevel
);

MVSDK_API CameraSdkStatus  CameraGetWhiteLevel(
  CameraHandle    hCamera,
  INT*        piWhiteLevel
);

MVSDK_API CameraSdkStatus  CameraSetIspOutFormat(
    CameraHandle    hCamera,
    UINT            uFormat
);

MVSDK_API CameraSdkStatus  CameraGetIspOutFormat(
    CameraHandle    hCamera,
    UINT*           puFormat
);

MVSDK_API char*  CameraGetErrorString(
    CameraSdkStatus     iStatusCode
);


MVSDK_API CameraSdkStatus CameraReConnect(
    CameraHandle    hCamera
);


MVSDK_API CameraSdkStatus CameraConnectTest(
    CameraHandle    hCamera
);

MVSDK_API BYTE* CameraAlignMalloc(
        int             size,
        int             align
);

MVSDK_API void CameraAlignFree(
        BYTE*           membuffer
);

MVSDK_API CameraSdkStatus  CameraCommonCall(
	CameraHandle    hCamera,
	char const*		pszCall,
	char*			pszResult,
	UINT			uResultBufSize
);

MVSDK_API CameraSdkStatus CameraGetEyeCount(
    CameraHandle hCamera,
    int *eyecount
);

MVSDK_API CameraSdkStatus CameraFlipFrameBuffer(
	BYTE *pFrameBuffer,
	tSdkFrameHead* pFrameHead,
	int Flags
	);

MVSDK_API CameraSdkStatus CameraConvertFrameBufferFormat(
	CameraHandle hCamera,
	BYTE *pInFrameBuffer,
	BYTE *pOutFrameBuffer,
	int outWidth,
	int outHeight,
	UINT outMediaType,
	tSdkFrameHead* pFrameHead
	);

MVSDK_API CameraSdkStatus CameraGetFrameID(
	CameraHandle    hCamera,
	UINT*           id
	);

MVSDK_API CameraSdkStatus CameraGetFrameTimeStamp(
	CameraHandle    hCamera,
	UINT*           TimeStampL,
	UINT*			TimeStampH
	);

MVSDK_API CameraSdkStatus CameraSetConnectionStatusCallback(
	CameraHandle        hCamera,
	CAMERA_CONNECTION_STATUS_CALLBACK pCallBack,
	PVOID               pContext
	);

MVSDK_API CameraSdkStatus CameraSetSysOption(
	char const* optionName,
	char const* value
	);

MVSDK_API CameraSdkStatus CameraClearBuffer(
	CameraHandle hCamera
	);

MVSDK_API CameraSdkStatus CameraSetFrameResendCount(
	CameraHandle        hCamera,
	int					count
	);

#ifdef __cplusplus
}
#endif

#endif
