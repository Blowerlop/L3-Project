using Project._Project.TESTT_REBIND;
using UnityEngine;

namespace Project._Project.Scripts.UI.Settings
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
