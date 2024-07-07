using _Project._200_Dev.Console;
using _Project._200_Dev.Managers;
using UnityEngine;

namespace _Project._200_Dev.Settings
{
    public static class AudioSettingsManager
    {
        public const string KEY_MASTER = "MasterBus";
        public const string KEY_MUSIC = "MasterBus/Music";
        public const string KEY_AMBIENCE = "MasterBus/Ambience";
        public const string KEY_SFX = "MasterBus/SFX";
        
        private static readonly string _logPrefix = "[AudioManager]";


        public static float Get(string exposedVolumeName) => PlayerPrefs.GetFloat(exposedVolumeName, 0.5f);

        public static void Set(string exposedVolumeName, float value01)
        {
            SetWithoutNotify(exposedVolumeName, value01);
            Save(exposedVolumeName, value01);
        }

        private static void SetWithoutNotify(string exposedVolumeName, float value01)
        {
            SoundManager.instance.SetBusVolume(value01, exposedVolumeName);
        }

        private static void Save(string exposedVolumeName, float volume)
        {
            PlayerPrefs.SetFloat(exposedVolumeName, volume);
            PlayerPrefs.Save();
            
            Debug.Log($"{_logPrefix} Saved : {exposedVolumeName} ({volume})");
        }
        
        public static void Load()
        {
            float master = Get(KEY_MASTER);
            float music = Get(KEY_MUSIC);
            float ambience = Get(KEY_AMBIENCE);
            float sfx = Get(KEY_SFX);
            
            SetWithoutNotify(KEY_MASTER, master);
            SetWithoutNotify(KEY_MUSIC,music);
            SetWithoutNotify(KEY_AMBIENCE,ambience);
            SetWithoutNotify(KEY_SFX, sfx);
            
            Debug.Log($"{_logPrefix} Volumes loaded : \n " +
                      $"- {KEY_MASTER} {master} \n " +
                      $"- {KEY_MUSIC} {music} \n " +
                      $"- {KEY_AMBIENCE} {ambience} \n " +
                      $"- {KEY_SFX} {sfx} ");
        }
        
        
        #region Commands

        [ConsoleCommand("volume", "Set the master volume")]
        private static void SetMasterVolume(float value01) => Set(KEY_MASTER, value01);
        
        [ConsoleCommand("volume_m", "Set the music volume")]
        private static void SetMusicVolume(float value01) => Set(KEY_MUSIC, value01);
        
        [ConsoleCommand("volume_a", "Set the ambience volume")]
        private static void SetAmbienceVolume(float value01) => Set(KEY_AMBIENCE, value01);
        
        [ConsoleCommand("volume_fx", "Set the game sounds volume")]
        private static void SetGameSoundsVolume(float value01) => Set(KEY_MASTER, value01);
        #endregion
    }
}