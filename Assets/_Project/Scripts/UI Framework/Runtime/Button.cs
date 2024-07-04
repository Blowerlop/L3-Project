using Project.Extensions;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace Project.Scripts.UIFramework
{
    public class Button : InteractibleUIElement
    {
        // Icon
        [SerializeField, BoxGroup("Group/Settings/Icon")] private bool _enableIcon = true;
        [SerializeField, BoxGroup("Group/Settings/Icon"), ShowIf("_enableIcon")] private Sprite _icon;
        // Text
        [SerializeField, BoxGroup("Group/Settings/Text")] private bool _enableText = true;
        [SerializeField, BoxGroup("Group/Settings/Text"), ShowIf("_enableText")] private string _text;
        [SerializeField, BoxGroup("Group/Settings/Text"), ShowIf("_enableText")] private bool _textSizeAuto;
        
        // Global settings
        [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableIcon || _enableText")] private float _size = 24;
        [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableIcon && _enableText")] private float _space = 10;
        // [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableIcon && _enableText")] private EColorType _textIconColorType = EColorType.Secondary;
        [SerializeField, BoxGroup("Group/Settings/Global")] private bool _enableBackground = true;
        // [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableBackground")] private EColorType _backgroundColorType = EColorType.Primary;
        
        // References
        [TabGroup("Group", "References")]
        [SerializeField, BoxGroup("Group/References/Background")] private Image _background;
        
        [SerializeField, BoxGroup("Group/References/Normal")] private HorizontalLayoutGroup _normalHorizontalLayoutGroup;
        [SerializeField, BoxGroup("Group/References/Normal")] private TMP_Text _normalButtonText;
        [SerializeField, BoxGroup("Group/References/Normal")] private Image _normalButtonIcon;
        
        [SerializeField, BoxGroup("Group/References/Hover")] private HorizontalLayoutGroup _hoverHorizontalLayoutGroup;
        [SerializeField, BoxGroup("Group/References/Hover")] private TMP_Text _hoverButtonText;
        [SerializeField, BoxGroup("Group/References/Hover")] private Image _hoverButtonIcon;
        

        
        protected virtual void OnValidate()
        {
            SetupIcon();
            SetupText();
            if (_enableText && _enableIcon)
            {
                _normalHorizontalLayoutGroup.spacing = _space;
                _hoverHorizontalLayoutGroup.spacing = _space;
            }
            
            _background.gameObject.SetActive(_enableBackground);
        }

        protected virtual void SetupIcon()
        {
            _normalButtonIcon.gameObject.SetActive(_enableIcon);
            _hoverButtonIcon.gameObject.SetActive(_enableIcon);
            
            if (_enableIcon)
            {
                _normalButtonIcon.sprite = _icon;
                LayoutElement buttonIconLayoutElement = _normalButtonIcon.GetComponent<LayoutElement>();
                buttonIconLayoutElement.preferredHeight = _size;
                buttonIconLayoutElement.preferredWidth = _size;
                
                _hoverButtonIcon.sprite = _icon;
                buttonIconLayoutElement = _hoverButtonIcon.GetComponent<LayoutElement>();
                buttonIconLayoutElement.preferredHeight = _size;
                buttonIconLayoutElement.preferredWidth = _size;
            }
        }

        protected virtual void SetupText()
        {
            _normalButtonText.gameObject.SetActive(_enableText);
            _hoverButtonText.gameObject.SetActive(_enableText);
            
            if (_enableText)
            {
                _normalButtonText.text = _text;
                _hoverButtonText.text = _text;
                
                if (_textSizeAuto)
                {
                    _normalButtonText.enableAutoSizing = true;
                    _hoverButtonText.enableAutoSizing = true;
                }
                else
                {
                    _normalButtonText.enableAutoSizing = false;
                    _normalButtonText.fontSize = _size;
                    
                    _hoverButtonText.enableAutoSizing = false;
                    _hoverButtonText.fontSize = _size;
                }
            }
        }

        public void SetVisibility(bool visible)
        {
            GetComponentsInChildren<Graphic>().ForEach(x => x.enabled = visible);
        }
    }
}