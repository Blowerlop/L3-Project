using Project.Extensions;
using UnityEngine;
using UnityEngine.Rendering;

namespace Project._Project.Scripts.UI.Settings
{
    public class ResolutionSettingsManager
    {
        private static ResolutionSettingsManager _instance;
        public static ResolutionSettingsManager instance => _instance ??= new ResolutionSettingsManager();

        public Resolution currentResolution => new Resolution {width = Screen.width, height = Screen.height};
        public string currentResolutionName => $"{currentResolution.width} x {currentResolution.height}";
        public FullScreenMode currentFullScreenMode => Screen.fullScreenMode;
        
        private readonly string _logPrefix = "[VideoManager/Resolution]";

        
        private ResolutionSettingsManager()
        {
        }

        
        public void SetResolution(int width, int height, FullScreenMode fullScreenMode)
        {
            Debug.Log($"{_logPrefix} Setting resolution to {width} x {height} in {fullScreenMode}...");
            Screen.SetResolution(width, height, fullScreenMode);
            Debug.Log($"{_logPrefix} Setting resolution is now {width} x {height} in {fullScreenMode} !");
        }

        public void Load()
        {
            Debug.Log($"{_logPrefix} Loaded : {currentResolutionName} in {currentFullScreenMode}");
        }
    }
    
    public class QualitySettingsManager
    {
        private static QualitySettingsManager _instance;
        public static QualitySettingsManager instance => _instance ??= new QualitySettingsManager();

        public int currentQualityIndex => QualitySettings.GetQualityLevel();
        public string currentQualityName => QualitySettings.names[QualitySettings.GetQualityLevel()];
        private const int _LOW_QUALITY_INDEX = 0;
        private const int _MEDIUM_QUALITY_INDEX = 1;
        private const int _HIGH_QUALITY_INDEX = 2;
        private const int _CUSTOM_QUALITY_INDEX = 3;

        private readonly string _logPrefix = "[VideoManager/Quality]";

        
        private QualitySettingsManager()
        {
        }
        
        
        public void SetLowQuality()
        {
            Debug.Log($"{_logPrefix} Setting Quality Level To Low...");
            QualitySettings.SetQualityLevel(_LOW_QUALITY_INDEX, true);
            Debug.Log($"{_logPrefix} Quality setting is now low !");
        }

        public void SetMediumQuality()
        {
            Debug.Log($"{_logPrefix} Setting Quality Level To Medium...");
            QualitySettings.SetQualityLevel(_MEDIUM_QUALITY_INDEX, true);
            Debug.Log($"{_logPrefix} Quality setting is now Medium !");
        }

        public void SetHighQuality()
        {
            Debug.Log($"{_logPrefix} Setting Quality Level To High...");
            QualitySettings.SetQualityLevel(_HIGH_QUALITY_INDEX, true);
            Debug.Log($"{_logPrefix} Quality setting is now High !");
        }

        public void SetCustomQuality()
        {
            Debug.Log($"{_logPrefix} Setting Quality Level To Custom...");
            // Getting the RenderPipelineAsset of the current quality settings before switching quality to override this renderPipeline
            RenderPipelineAsset currentRenderPipelineAsset = QualitySettings.GetRenderPipelineAssetAt(currentQualityIndex);
            QualitySettings.SetQualityLevel(_CUSTOM_QUALITY_INDEX, true);
            QualitySettings.renderPipeline = currentRenderPipelineAsset;
            Debug.Log($"{_logPrefix} Quality setting is now Custom with the {currentRenderPipelineAsset.name} render pipeline");
        }
        
        public void Load()
        {
            Debug.Log($"{_logPrefix} Loaded : {currentQualityName}");
        }
    }

    public class FrameRateSettingsManager
    {
        private static FrameRateSettingsManager _instance;
        public static FrameRateSettingsManager instance => _instance ??= new FrameRateSettingsManager();

        private const string _KEY_FRAMERATE = "FrameRate";
        private const string _KEY_VSYNC = "Vsync";
        
        private readonly string _logPrefix = "[VideoManager/FrameRate]";
        

        private FrameRateSettingsManager()
        {
        }
        

        public int GetFrameRate() => PlayerPrefs.GetInt(_KEY_FRAMERATE, (int)Screen.currentResolution.refreshRateRatio.value);
        public bool GetVSync() => PlayerPrefs.GetInt(_KEY_VSYNC, false.ToInt()).ToBool();

        public void SetFrameRate(int targetFrameRate)
        {
            SetFrameRateWithoutNotify(targetFrameRate);
            OnFrameRateSet();
        }

        private void SetFrameRateWithoutNotify(int targetFrameRate)
        {
            Application.targetFrameRate = targetFrameRate;
        }

        private void OnFrameRateSet()
        {
            SaveFrameRate();
        }
        
        public void SaveFrameRate()
        {
            PlayerPrefs.SetInt(_KEY_FRAMERATE, Application.targetFrameRate);
            PlayerPrefs.Save();
            Debug.Log($"{_logPrefix} Saved : {Application.targetFrameRate}");
        }
        
        public void SetSync(bool state)
        {
            SetVsyncWithoutNotify(state);
            OnVsyncSet();
        }

        private void SetVsyncWithoutNotify(bool state)
        {
            QualitySettings.vSyncCount = state.ToInt();
        }

        private void OnVsyncSet()
        {
            SaveVSync();
        }
            
        public void SaveVSync()
        {
            PlayerPrefs.SetInt(_KEY_VSYNC, QualitySettings.vSyncCount);
            PlayerPrefs.Save();
            Debug.Log($"{_logPrefix} Saved : vSync {GetVSync()}");
        }

        public void Load()
        {
            SetFrameRateWithoutNotify(GetFrameRate());
            SetVsyncWithoutNotify(GetVSync());
            Debug.Log($"{_logPrefix} Loaded : {Application.targetFrameRate}");
        }

        public override string ToString()
        {
            return GetFrameRate().ToString();
        }
    }
    
    
    public static class VideoSettingsManager
    {
        public static ResolutionSettingsManager resolution => ResolutionSettingsManager.instance;
        public static QualitySettingsManager quality => QualitySettingsManager.instance;
        public static FrameRateSettingsManager frameRate => FrameRateSettingsManager.instance;

        
        public static void Load()
        {
            resolution.Load();
            quality.Load();
            frameRate.Load();
        }
    }
}
