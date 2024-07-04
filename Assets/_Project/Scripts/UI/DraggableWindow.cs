using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Project
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
            // Vector2 potentialPosition = _target.anchoredPosition + (eventData.delta / _canvas.scaleFactor);

            // // Right and Left
            // if (potentialPosition.x >= 0 || potentialPosition.x <= 0)
            // {
            //     _target.anchoredPosition = new Vector2(0.0f, _target.anchoredPosition.y);
            //     potentialPosition.x = 0.0f;
            // }
            //
            // // Top and Bottom
            // if (potentialPosition.y >= 0 || potentialPosition.y <= 0)
            // {
            //     _target.anchoredPosition = new Vector2(_target.anchoredPosition.x, 0.0f);
            //     potentialPosition.y = 0.0f;
            // }

            // _target.anchoredPosition = potentialPosition;
            
            
            
            _target.anchoredPosition += (eventData.delta / _canvas.scaleFactor);
        }
    }
}
