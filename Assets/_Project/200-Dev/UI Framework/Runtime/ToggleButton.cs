using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace _Project._200_Dev.UI_Framework.Runtime
{
    public class ToggleButton : InteractibleUIElement
    {
        // Icon
        [SerializeField, BoxGroup("Group/Settings/Icon")] private bool _enableIcon = true;
        [SerializeField, BoxGroup("Group/Settings/Icon"), ShowIf("_enableIcon")] private Sprite _toggleOffIcon;
        [SerializeField, BoxGroup("Group/Settings/Icon"), ShowIf("_enableIcon")] private Sprite _toggleOnIcon;
        // Text
        [SerializeField, BoxGroup("Group/Settings/Text")] private bool _enableText = true;
        [SerializeField, BoxGroup("Group/Settings/Text"), ShowIf("_enableText")] private string _toggleOffText;
        [SerializeField, BoxGroup("Group/Settings/Text"), ShowIf("_enableText")] private string _toggleOnText;
        [SerializeField, BoxGroup("Group/Settings/Text"), ShowIf("_enableText")] private bool _textSizeAuto;
        // Toggle
        [SerializeField, BoxGroup("Group/Settings/Toggle")] private bool _toggleDefaultState;
        [SerializeField, BoxGroup("Group/Settings/Toggle")] private bool _callbackOnStart;
        [SerializeField, BoxGroup("Group/Settings/Toggle"), ReadOnly] private bool _toggle;
        // Global settings
        [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableIcon || _enableText")] private float _size = 24;
        [SerializeField, BoxGroup("Group/Settings/Global"), ShowIf("@_enableIcon && _enableText")] private float _space = 10;
        
        // Toggle off
        [TabGroup("Group", "References")]
        [SerializeField, BoxGroup("Group/References/ToggleOff")] private HorizontalLayoutGroup _horizontalLayoutGroupOff;
        [SerializeField, BoxGroup("Group/References/ToggleOff")] private TMP_Text _textOff;
        [SerializeField, BoxGroup("Group/References/ToggleOff")] private Image _imageOff;
        // Toggle on
        [SerializeField, BoxGroup("Group/References/ToggleOn")] private HorizontalLayoutGroup _horizontalLayoutGroupOn;
        [SerializeField, BoxGroup("Group/References/ToggleOn")] private TMP_Text _textOn;
        [SerializeField, BoxGroup("Group/References/ToggleOn")] private Image _imageOn;

        [SerializeField, TabGroup("Group", "Events")] public UnityEvent<bool> _onToggle = new UnityEvent<bool>();
        [SerializeField, TabGroup("Group", "Events")] public UnityEvent _onToggleOn = new UnityEvent();
        [SerializeField, TabGroup("Group", "Events")] public UnityEvent _onToggleOff = new UnityEvent();
        
        [SerializeField, FoldoutGroup("Group/Events/Animations", 999)] public UnityEvent _onToggleAnimation = new UnityEvent();
        [SerializeField, FoldoutGroup("Group/Events/Animations", 999)] public UnityEvent _onToggleOnAnimation = new UnityEvent();
        [SerializeField, FoldoutGroup("Group/Events/Animations", 999)] public UnityEvent _onToggleOffAnimation = new UnityEvent();
        

        private void Start()
        {
            _toggle = _toggleDefaultState;
            if (_toggle)
            {
                _onToggleAnimation.Invoke();
                _onToggleOnAnimation.Invoke();
            }
            else _onToggleOffAnimation.Invoke();
            
            if (_callbackOnStart)
            {
                _onToggle.Invoke(_toggle);
                if (_toggle) _onToggleOn.Invoke();
                else _onToggleOff.Invoke();
            }
        }


        private void OnValidate()
        {
            SetupIcon();
            SetupText();
            if (_enableText && _enableIcon)
            {
                _horizontalLayoutGroupOff.spacing = _space;
                _horizontalLayoutGroupOn.spacing = _space;
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
            Toggle();
        }

        private void Toggle()
        {
            SetToggle(!_toggle);
        }

        public void SetToggle(bool state)
        {
            if (_toggle == state) return;
            
            _toggle = state;
            _onToggleAnimation.Invoke();
            _onToggle.Invoke(_toggle);
            if (_toggle)
            {
                _onToggleOnAnimation.Invoke();
                _onToggleOn.Invoke();
            }
            else
            {
                _onToggleOffAnimation.Invoke();
                _onToggleOff.Invoke();
            }
        }
        
        
        protected virtual void SetupIcon()
        {
            _imageOff.gameObject.SetActive(_enableIcon);
            _imageOn.gameObject.SetActive(_enableIcon);
            
            if (_enableIcon)
            {
                _imageOff.sprite = _toggleOffIcon;
                LayoutElement buttonIconLayoutElement = _imageOff.GetComponent<LayoutElement>();
                buttonIconLayoutElement.preferredHeight = _size;
                buttonIconLayoutElement.preferredWidth = _size;
                
                _imageOn.sprite = _toggleOnIcon;
                buttonIconLayoutElement = _imageOn.GetComponent<LayoutElement>();
                buttonIconLayoutElement.preferredHeight = _size;
                buttonIconLayoutElement.preferredWidth = _size;
            }
        }

        protected virtual void SetupText()
        {
            _textOff.gameObject.SetActive(_enableText);
            _textOn.gameObject.SetActive(_enableText);
            
            if (_enableText)
            {
                _textOff.text = _toggleOffText;
                if (_textSizeAuto) _textOff.enableAutoSizing = true;
                else
                {
                    _textOff.enableAutoSizing = false;
                    _textOff.fontSize = _size;
                }
                
                _textOn.text = _toggleOnText;
                if (_textSizeAuto) _textOn.enableAutoSizing = true;
                else
                {
                    _textOn.enableAutoSizing = false;
                    _textOn.fontSize = _size;
                }
            }
        }
    }
}