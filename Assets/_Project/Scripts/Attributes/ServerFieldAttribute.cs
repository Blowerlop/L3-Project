using System;
using System.Diagnostics;
using System.Linq;
using Sirenix.Utilities;
using Unity.Netcode;
using UnityEditor;
using UnityEngine;

namespace Project
{
    [Conditional("UNITY_EDITOR")]
    [AttributeUsage(AttributeTargets.Field)]
    public class ServerFieldAttribute : PropertyAttribute
    {
    }

    #if UNITY_EDITOR
    [CustomPropertyDrawer(typeof(ServerFieldAttribute))]
    public class ServerFieldAttributeDrawer : PropertyDrawer
    {
        private const string _TOOLTIP_TEXT_SERVER = "Server side";
        private static readonly string _tooltipTextClient = $"{_TOOLTIP_TEXT_SERVER} --> You're a client";
        private static readonly string _labelText = $"({_TOOLTIP_TEXT_SERVER})";

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            
            GUI.Label(position.SetX(EditorGUIUtility.fieldWidth + 10.0f), _labelText, new GUIStyle{fontStyle = FontStyle.Bold});

            if (Application.isPlaying 
                && NetworkManager.Singleton != null 
                && NetworkManager.Singleton.IsListening 
                && NetworkManager.Singleton.IsServer == false)
            {
                GUI.enabled = false;
                label.tooltip = _tooltipTextClient;
                EditorGUI.PropertyField(position, property, label);
                GUI.Label(position, string.Concat(Enumerable.Repeat("-", (int)position.size.x)));
            }
            else
            {
                label.tooltip = _TOOLTIP_TEXT_SERVER;
                EditorGUI.PropertyField(position, property, label);
            }
            
            GUI.enabled = true;
        }
    }
    #endif
}
