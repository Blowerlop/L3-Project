using Sirenix.OdinInspector;
using TMPro;
using UnityEditor;
using UnityEngine;

namespace Project.Scripts.UIFramework
{
    public enum EColorType
    {
        Primary,
        Secondary,
        Accent,
        Positive,
        Negative
    }
    
    [CreateAssetMenu(menuName = "Scriptable Objects/UI Manager")]
    public class SOUIManager : ScriptableObject, IScriptableObjectSerializeReference
    {
        [Title("Colors")]
        [ColorUsage(false), OnValueChanged("@UIManager.UpdateUI()")] public Color baseColor = Color.white;
        [ColorUsage(false), OnValueChanged("@UIManager.UpdateUI()")] public Color secondaryColor = Color.white;
        [ColorUsage(false), OnValueChanged("@UIManager.UpdateUI()")] public Color accentColor = Color.white;
        [ColorUsage(false), OnValueChanged("@UIManager.UpdateUI()")] public Color positiveColor = Color.white;
        [ColorUsage(false), OnValueChanged("@UIManager.UpdateUI()")] public Color negativeColor = Color.white;
        
        [Title("Font")]
        [OnValueChanged("@UIManager.UpdateUI()")] public TMP_FontAsset titleFont;
        [OnValueChanged("@UIManager.UpdateUI()")] public TMP_FontAsset contentFont;
        
        // No longer used. Hard coded in InteractibleUIElement.cs
        // [Title("Sounds")]
        // public AudioClip hoverSound;
        // public AudioClip clickSound;
        
        
        #region Editor
#if UNITY_EDITOR
        [Button]
        [Tooltip("Sometimes Github don't detect the changes made on the ScriptableObject, so we need to force the write on the disk")]
        private void ForceSaveOnDisk()
        {
            EditorUtility.SetDirty(this);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }
#endif
        #endregion
    }
}
