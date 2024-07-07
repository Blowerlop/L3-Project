using UnityEngine;

namespace _Project._200_Dev.Settings
{
    public static class SettingsManager
    {
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
        private static void LoadAllSettings()
        {
            VideoSettingsManager.Load();
            AudioSettingsManager.Load();
            InputSettingsManager.Load();
        }
    }
}
