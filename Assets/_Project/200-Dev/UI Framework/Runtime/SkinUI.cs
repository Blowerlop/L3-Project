using System;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace Project.Scripts.UIFramework
{
    public enum ETextType
    {
        Title,
        Content
    }
    
    
    public sealed class SkinUI : MonoBehaviour
    {
        private enum EComponentTypeUI
        {
            Image,
            Text,
        }
        
        [NonSerialized] private EComponentTypeUI _componentType;
        [SerializeField] private EColorType _colorType;
        [SerializeField, ShowIf("@_componentType == EComponentTypeUI.Text")] private ETextType _textType;
        [SerializeField] private bool _useCustomAlpha;



        private void Start()
        {
            OnValidate();
        }
        
        public void OnValidate()
        {
            if (TryGetComponent(out Image image))
            {
                _componentType = EComponentTypeUI.Image;
                
                Color color = GetColor();
                if (_useCustomAlpha) color.a = image.color.a;
                
                image.color = color;
            }
            else if (TryGetComponent(out TMP_Text text))
            {
                _componentType = EComponentTypeUI.Text;
                
                Color color = GetColor();
                
                if (_useCustomAlpha) color.a = text.color.a;
                
                text.color = color;
                text.font = GetFont();
            }
            else throw new MissingComponentException("Missing component Image or TMP_Text");
        }
        
        private Color GetColor()
        {
            return UIManager.GetColorByType(_colorType);
        }

        private TMP_FontAsset GetFont()
        {
            return UIManager.GetFontByType(_textType);
        }
        
        public void UpdateColorType(string colorTypeName)
        {
            if (Enum.TryParse(colorTypeName, out EColorType colorType))
            {
                _colorType = colorType;
                OnValidate();
            }
            else Debug.LogError($"Color type {colorTypeName} not found");
        }
    }
}