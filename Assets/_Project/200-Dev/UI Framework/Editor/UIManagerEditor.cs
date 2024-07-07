using _Project._200_Dev.UI_Framework.Runtime;
using Project.Editor;
using UnityEditor;

namespace Project.Scripts.UIFramework
{
    public static class UIManagerEditor
    {
        [MenuItem("Tools/UIManager")]
        public static void OpenUIManager()
        {
            PopUpAssetInspector.Create(UIManager.instance);
        }
    }
}