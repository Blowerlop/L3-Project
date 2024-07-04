using UnityEditor;

namespace Project
{
    public class EnterPlayModeOptions
    {
        private const string _MENU_ITEM_PATH = "Tools/EnterPlayModeOptions/Enable Domain Reload";
        
        [MenuItem(_MENU_ITEM_PATH)]
        public static void ToggleEnterPlayModeOptions()
        {
            EditorSettings.enterPlayModeOptionsEnabled = !EditorSettings.enterPlayModeOptionsEnabled;
            Menu.SetChecked(_MENU_ITEM_PATH, EditorSettings.enterPlayModeOptionsEnabled);
        }
    }
}
