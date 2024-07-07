using Project.Extensions;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Project.Scripts.UIFramework
{
    public class Setting : InteractibleUIElement
    {
        [SerializeField, BoxGroup("Group/Settings/Text")] private string _content;
        [SerializeField, BoxGroup("Group/Settings/Text")] private bool _textSizeAuto;
        [SerializeField, BoxGroup("Group/Settings/Text"), HideIf("@_textSizeAuto")] private float _size = 24;
        [SerializeField, BoxGroup("Group/Settings/Global settings")] private float _space = 10;
        
        [TabGroup("Group", "References")]
        [SerializeField, BoxGroup("Group/References/Normal")] private HorizontalLayoutGroup _horizontalLayoutGroup;
        [SerializeField, BoxGroup("Group/References/Normal")] private TMP_Text _text;

        private InteractibleUIElement[] _interactibleUIElements;
        
        
        private void Awake()
        {
            _interactibleUIElements = transform.GetComponentsInChildrenWithoutParent<InteractibleUIElement>().ToArray();
        }

        private void OnValidate()
        {
            _text.text = _content;
            _text.enableAutoSizing = _textSizeAuto;
            if (_textSizeAuto == false) _text.fontSize = _size;
            
            _horizontalLayoutGroup.spacing = _space;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);

            _interactibleUIElements.ForEach(x => x.OnPointerClick(null));
        }
    }
}