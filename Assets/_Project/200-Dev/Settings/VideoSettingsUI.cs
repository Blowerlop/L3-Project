using System;
using System.Collections.Generic;
using System.Linq;
using _Project._200_Dev.UI_Framework.Runtime;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;

namespace _Project._200_Dev.Settings
{
    [Serializable]
    public class ResolutionSettingsUI
    {
        public enum ECustomFullScreenModeLabel
        {
            Fullscreen,
            Borderless,
            Windowed
        }
        
        [SerializeField] private TMP_Dropdown _resolutionDropdown;
        [SerializeField] private TMP_Dropdown _displayModeDropdown;
        [SerializeField] private Button _applyButton;

        [LabelText("Current Resolution"), ShowInInspector, ReadOnly] private string _currentResolutionName => VideoSettingsManager.resolution.currentResolutionName;
        [ShowInInspector, ReadOnly] private int _selectedResolutionIndex;
        
        [LabelText("Current Resolution"), ShowInInspector, ReadOnly] private string _currentDisplayModeName => VideoSettingsManager.resolution.currentFullScreenMode.ToString();
        [ShowInInspector, ReadOnly] private int _selectedDisplayModeIndex;
        
        public void Start()
        {
            FillResolutionDropDown();
            FillDisplayModeDropDown();
            
            DropdownSelectCurrentResolution();
            DropdownSelectCurrentDisplayMode();
        }

        public void OnEnable()
        {
            // DropdownSelectCurrentResolution();
            // DropdownSelectCurrentDisplayMode();
            
            _resolutionDropdown.onValueChanged.AddListener(OnResolutionDropdownValueChanged_CacheIndex);
            _displayModeDropdown.onValueChanged.AddListener(OnDisplayModeDropdownValueChanged_CacheIndex);
            
            _applyButton.onClick.AddListener(ApplyResolution);
        }
        
        public void OnDisable()
        {
            _resolutionDropdown.onValueChanged.RemoveListener(OnResolutionDropdownValueChanged_CacheIndex);
            _displayModeDropdown.onValueChanged.RemoveListener(OnDisplayModeDropdownValueChanged_CacheIndex);
            
            _applyButton.onClick.RemoveListener(ApplyResolution);
        }


        private void SetResolution(int width, int height, FullScreenMode fullScreenMode)
        {
            VideoSettingsManager.resolution.SetResolution(width, height, fullScreenMode);
        }

        private void FillResolutionDropDown()
        {
            _resolutionDropdown.ClearOptions();
            
            var resolutionsName = Screen.resolutions.Select(resolution => $"{resolution.width} x {resolution.height}").Distinct().Reverse().ToList();
            _resolutionDropdown.AddOptions(resolutionsName);
        }
        
        private void FillDisplayModeDropDown()
        {
            _displayModeDropdown.ClearOptions();
            
            List<string> fullScreenModeNames = Enum.GetNames(typeof(ECustomFullScreenModeLabel)).ToList();
            _displayModeDropdown.AddOptions(fullScreenModeNames); 
        }

        private void DropdownSelectCurrentResolution()
        {
            int index = _resolutionDropdown.options.FindIndex(optionData => optionData.text == _currentResolutionName);
            _resolutionDropdown.value = index;
        }
        
        private void DropdownSelectCurrentDisplayMode()
        {
            int index = _displayModeDropdown.options.FindIndex(optionData => optionData.text == _currentDisplayModeName);
            _displayModeDropdown.value = index;
        }

        private void OnResolutionDropdownValueChanged_CacheIndex(int index)
        {
            _selectedResolutionIndex = index;
        }
        
        private void OnDisplayModeDropdownValueChanged_CacheIndex(int index)
        {
            _selectedDisplayModeIndex = index;
        }

        private void ApplyResolution()
        {
            Resolution resolution = ExtractResolutionFromOption(_resolutionDropdown.options[_selectedResolutionIndex]);

            ECustomFullScreenModeLabel customFullScreenModeLabel = (ECustomFullScreenModeLabel)_selectedDisplayModeIndex;
            FullScreenMode fullScreenMode = ConvertCustomFullScreenModeToUnityOne(customFullScreenModeLabel);
            
            SetResolution(resolution.width, resolution.height, fullScreenMode);
        }

        private Resolution ExtractResolutionFromOption(TMP_Dropdown.OptionData optionData)
        {
            string[] split = optionData.text.Split('x', StringSplitOptions.RemoveEmptyEntries);
            return new Resolution{width = int.Parse(split[0]), height = int.Parse(split[1])};
        }

        private FullScreenMode ConvertCustomFullScreenModeToUnityOne(
            ECustomFullScreenModeLabel customFullScreenModeLabel)
        {
            return customFullScreenModeLabel switch
            {
                ECustomFullScreenModeLabel.Fullscreen => FullScreenMode.ExclusiveFullScreen,
                ECustomFullScreenModeLabel.Borderless => FullScreenMode.FullScreenWindow,
                ECustomFullScreenModeLabel.Windowed => FullScreenMode.Windowed,
                _ => throw new ArgumentOutOfRangeException(nameof(customFullScreenModeLabel), customFullScreenModeLabel,
                    null)
            };
        }
    }
    
    [Serializable]
    public class QualitySettingsUI
    {
        [SerializeField] private TMP_Dropdown _dropdown;

        
        public void Start()
        {
            FillDropDown();
            DropdownSelectCurrentQuality();
        }
        
        public void OnEnable()
        {
            _dropdown.onValueChanged.AddListener(SetQuality);
        }
        
        public void OnDisable()
        {
            _dropdown.onValueChanged.RemoveListener(SetQuality);
        }
        

        private void FillDropDown()
        {
            _dropdown.ClearOptions();
            
            List<string> names = QualitySettings.names.ToList();
            _dropdown.AddOptions(names);
        }
        
        private void DropdownSelectCurrentQuality()
        {
            _dropdown.value = QualitySettingsManager.instance.currentQualityIndex;
        }
        
        public void SetQuality(int index)
        {
            switch (index)
            {
                case 0:
                    SetLowQuality();
                    break;
                
                case 1:
                    SetMediumQuality();
                    break;
                
                case 2:
                    SetHighQuality();
                    break;
                
                case 3:
                    SetCustomQuality();
                    break;
                
                default:
                    throw new IndexOutOfRangeException();
            }
        }
        
        public void SetLowQuality() => VideoSettingsManager.quality.SetLowQuality();
        public void SetMediumQuality() => VideoSettingsManager.quality.SetMediumQuality();
        public void SetHighQuality() => VideoSettingsManager.quality.SetHighQuality();
        public void SetCustomQuality() => VideoSettingsManager.quality.SetCustomQuality();
    }

    [Serializable]
    public class FrameRateSettingsUI
    {
        [SerializeField] private TMP_InputField _frameRateInputField;
        [SerializeField] private ToggleButton _vSyncButton;


        public void Start()
        {
            _frameRateInputField.SetTextWithoutNotify(VideoSettingsManager.frameRate.ToString());
            _vSyncButton.SetToggle(VideoSettingsManager.frameRate.GetVSync());
        }

        public void OnEnable()
        {
            _frameRateInputField.onEndEdit.AddListener(SetSetFrameRate);
            _vSyncButton._onToggle.AddListener(SetVsync);
        }
        
        public void OnDisable()
        {
            _frameRateInputField.onEndEdit.RemoveListener(SetSetFrameRate);
            _vSyncButton._onToggle.RemoveListener(SetVsync);
        }
        

        private void SetFrameRate(int value)
        {
            VideoSettingsManager.frameRate.SetFrameRate(value);
        }
        
        private void SetSetFrameRate(string value)
        {
            if (string.IsNullOrEmpty(value))
            {
                Debug.LogError("FrameRateSettings inputField text is null or empty. This should never happened");
                return;
            }

            if (int.TryParse(value, out int result))
            {
                SetFrameRate(result);
            }
            else
            {
                Debug.LogError("FrameRateSettings inputField text parse failed. This should never happened");
            }
        }

        private void SetVsync(bool state)
        {
            VideoSettingsManager.frameRate.SetSync(state);
        }
    }


    public class VideoSettingsUI : MonoBehaviour
    {
        [SerializeField] private ResolutionSettingsUI _resolutionSettingsUISettingsUI = new ResolutionSettingsUI();
        [SerializeField] private QualitySettingsUI _qualitySettingsUI = new QualitySettingsUI();
        [SerializeField] private FrameRateSettingsUI _frameRateSettingsUI = new FrameRateSettingsUI();
        
        
        private void Start()
        {
            _resolutionSettingsUISettingsUI.Start();
            _qualitySettingsUI.Start();
            _frameRateSettingsUI.Start();
        }
        
        private void OnEnable()
        {
            _resolutionSettingsUISettingsUI.OnEnable();
            _qualitySettingsUI.OnEnable();
            _frameRateSettingsUI.OnEnable();
        }
        
        private void OnDisable()
        {
            _resolutionSettingsUISettingsUI.OnDisable();
            _qualitySettingsUI.OnDisable();
            _frameRateSettingsUI.OnDisable();
        }
    }
}