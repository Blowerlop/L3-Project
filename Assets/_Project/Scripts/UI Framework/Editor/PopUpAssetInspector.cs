using UnityEditor;
using UnityEngine;

namespace Project.Editor
{
    public class PopUpAssetInspector : EditorWindow
    {
        private Object asset;
        private UnityEditor.Editor assetEditor;
   
        public static PopUpAssetInspector Create(Object asset)
        {
            var window = CreateWindow<PopUpAssetInspector>($"{asset.name} | {asset.GetType().Name}");
            window.asset = asset;
            window.assetEditor = UnityEditor.Editor.CreateEditor(asset);
            return window;
        }
 
        private void OnGUI()
        {
            GUI.enabled = false;
            asset = EditorGUILayout.ObjectField("Asset", asset, asset.GetType(), false);
            GUI.enabled = true;
 
            EditorGUILayout.BeginVertical(EditorStyles.helpBox);
            assetEditor.OnInspectorGUI();
            EditorGUILayout.EndVertical();
        }
    }
}