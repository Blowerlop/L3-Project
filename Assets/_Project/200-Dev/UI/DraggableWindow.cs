using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.EventSystems;

namespace _Project._200_Dev.UI
{
    public class DraggableWindow : MonoBehaviour, IDragHandler
    {
        private Canvas _canvas;
        [SerializeField, HideIf("_selfTarget")] private RectTransform _target;
        [SerializeField] private bool _selfTarget = false;

        
        private void Awake()
        {
            _canvas = GetComponentInParent<Canvas>();
            if (_selfTarget) _target = GetComponent<RectTransform>();
        }

        
        public void OnDrag(PointerEventData eventData)
        {
            _target.anchoredPosition += (eventData.delta / _canvas.scaleFactor);
        }
    }
}
