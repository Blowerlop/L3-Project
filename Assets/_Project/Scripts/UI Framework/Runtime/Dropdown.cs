using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Project.Scripts.UIFramework
{
    public class Dropdown : InteractibleUIElement
    {
        [TabGroup("Group", "References")]
        [SerializeField, BoxGroup("Group/References/Dropdown")] private TMP_Dropdown _dropdown;


        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);
            
            _dropdown.OnPointerEnter(eventData);
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            
            _dropdown.OnPointerExit(eventData);
        }

        public override void OnPointerDown(PointerEventData eventData)
        {
            base.OnPointerDown(eventData);
            
            _dropdown.OnPointerDown(eventData);
        }

        public override void OnPointerUp(PointerEventData eventData)
        {
            base.OnPointerUp(eventData);
            
            _dropdown.OnPointerUp(eventData);
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
            
            _dropdown.OnPointerClick(eventData);
        }
    }
}