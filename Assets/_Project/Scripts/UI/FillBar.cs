using DG.Tweening;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.UI;

namespace Project
{
    public class FillBar : MonoBehaviour
    {
        [SerializeField] protected Image _fillImage;
        
        [SerializeField] private bool useSecondFill;
        [SerializeField, ShowIf(nameof(useSecondFill))] protected Image secondFillImage;
        [SerializeField, ShowIf(nameof(useSecondFill))] private float secondFillDelay;
        
        [SerializeField] private float _fillAnimDuration = 0.25f;
        [SerializeField] private Ease _fillEase;

        private float _maxValue;
        
        private Sequence _fillSequence;

        private void InitSequence()
        {
            if(_fillSequence != null && _fillSequence.IsActive()) 
                _fillSequence.Kill();
            
            _fillSequence = DOTween.Sequence();
        }
        
        [Button]
        protected void SetFillAmount(float normalizedValue, bool instant = false)
        {
            InitSequence();
            
            if (instant)
            {
                _fillImage.fillAmount = normalizedValue;
            }
            else
            {
                _fillSequence.Append(_fillImage.DOFillAmount(normalizedValue, useSecondFill ? 0 : _fillAnimDuration).SetEase(_fillEase));
                
                if (useSecondFill)
                    _fillSequence.Join(secondFillImage.DOFillAmount(normalizedValue, _fillAnimDuration).SetEase(_fillEase).SetDelay(secondFillDelay));
            }
        }
        

        protected void SetFillAmount(float current, float max, bool instant = false)
        {
            float normalized = current / max;
            SetFillAmount(normalized, instant);
        }
    }
}