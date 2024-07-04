using UnityEngine;

namespace Project._Project.Scripts.UI.Settings
{
    public class AudioSettingsUI : MonoBehaviour
    {
        [SerializeField] private SliderExtended _masterSlider;
        [SerializeField] private SliderExtended _musicSlider;
        [SerializeField] private SliderExtended _ambienceSlider;
        [SerializeField] private SliderExtended _sfxSlider;


        private void Start()
        {
            RefreshSliders();
        }

        private void OnEnable()
        {
            RefreshSliders();
                
            _masterSlider.onPointerUp.AddListener(OnPointerUp_SetMaster);
            _musicSlider.onPointerUp.AddListener(OnPointerUp_SetMusic);
            _ambienceSlider.onPointerUp.AddListener(OnPointerUp_SetAmbience);
            _sfxSlider.onPointerUp.AddListener(OnPointerUp_SetSfx);
        }

        private void OnDisable()
        {
            _masterSlider.onPointerUp.RemoveListener(OnPointerUp_SetMaster);
            _musicSlider.onPointerUp.RemoveListener(OnPointerUp_SetMusic);
            _ambienceSlider.onPointerUp.RemoveListener(OnPointerUp_SetAmbience);
            _sfxSlider.onPointerUp.RemoveListener(OnPointerUp_SetSfx);
        }


        private void RefreshSliders()
        {
            _masterSlider.SetValueWithoutNotify(Get(AudioSettingsManager.KEY_MASTER));
            _musicSlider.SetValueWithoutNotify(Get(AudioSettingsManager.KEY_MUSIC));
            _ambienceSlider.SetValueWithoutNotify(Get(AudioSettingsManager.KEY_AMBIENCE));
            _sfxSlider.SetValueWithoutNotify(Get(AudioSettingsManager.KEY_SFX));
        }


        private float Get(string exposedVolumeName)
        {
            return AudioSettingsManager.Get(exposedVolumeName) * 100.0f;
        }

        private void Set(string exposedVolumeName, float value)
        {
            float value01 = value / 100.0f;
            
            AudioSettingsManager.Set(exposedVolumeName, value01);
        }

        private void OnPointerUp_SetMaster(float value)
        {
            Set(AudioSettingsManager.KEY_MASTER, value);
        }
        
        private void OnPointerUp_SetMusic(float value)
        {
            Set(AudioSettingsManager.KEY_MUSIC, value);
        }
        
        private void OnPointerUp_SetAmbience(float value)
        {
            Set(AudioSettingsManager.KEY_AMBIENCE, value);
        }
        
        private void OnPointerUp_SetSfx(float value)
        {
            Set(AudioSettingsManager.KEY_SFX, value);
        }
    }
}