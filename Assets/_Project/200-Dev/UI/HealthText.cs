using System;
using DG.Tweening;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;

namespace _Project._200_Dev.UI
{
    public class HealthText : MonoBehaviour
    {
        [SerializeField] private Color damageColor, healColor;
        
        [SerializeField] private float jumpPower;
        [SerializeField] private float duration;
        [SerializeField] private float fadeDuration;
        
        [SerializeField] private float distance;

        [SerializeField] private TextMeshPro text;

        public Action OnStopped;
        
        private Transform _parent;
        private Vector3 _lastParentPosition;
        
        private bool _isActive;
        
        [Button]
        public void Init(int value, Transform parent, Vector3 direction)
        {
            _isActive = true;
            var isHeal = value >= 0;
            
            gameObject.SetActive(true);
            transform.position = parent.position;
            _parent = parent;
            _lastParentPosition = parent.position;
            
            var color = isHeal ? healColor : damageColor;
            text.color = color;
            
            color.a = 0;
            
            Sequence sequence = DOTween.Sequence();

            if (isHeal)
            {
                transform.localScale = Vector3.zero;
                transform.position += Vector3.right;
                
                sequence.Append(transform.DOMoveY(transform.position.y + distance * 1.25f, duration));
                sequence.Join(transform.DOScale(Vector3.one, duration * 0.33f));
                
                sequence.Join(text.DOColor(color, fadeDuration).SetDelay(duration - fadeDuration));
                sequence.Join(transform.DOScale(Vector3.one * 0.5f, fadeDuration));
            }
            else
            {
                var target = transform.position + direction * distance;
                transform.localScale = Vector3.one;
                
                sequence.Append(transform.DOJump(target, jumpPower, 1, duration));
                sequence.Join(text.DOColor(color, fadeDuration).SetDelay(duration - fadeDuration));
            }
            
            sequence.OnComplete(() =>
            {
                OnStopped?.Invoke();
                OnStopped = null;
                
                _isActive = false;
            });
            
            value = Mathf.Abs(value);
            text.text = (isHeal ? "+" : "") + value;
        }

        private void Update()
        {
            if (!_isActive) return;
            if (_parent == null) return;
            
            var diff = _parent.position - _lastParentPosition;
            
            transform.position += diff;
            
            _lastParentPosition = _parent.position;
        }
    }
}
