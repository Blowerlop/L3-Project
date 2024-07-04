using System;
using Sirenix.OdinInspector;
using UnityEditor;
using UnityEngine;

namespace Project
{
    public class ContextMenuAdditions : MonoBehaviour
    {
        [InitializeOnLoadMethod]
        static void Start() {
 
            EditorApplication.contextualPropertyMenu += OnPropertyContextMenu;
        }
 
        static void OnPropertyContextMenu(GenericMenu menu, SerializedProperty property) {
 
            SerializedProperty propertyCopy = property.Copy();
            // propertyCopy.Reset();
 
            menu.AddItem(new GUIContent("Print Property"), false, () =>    {
 
                Debug.Log(propertyCopy.displayName);
                property.Reset();
            });
        }


        [MenuItem("CONTEXT/Transform/Test")]
        private static void Test()
        {
            
        }
    }
}
