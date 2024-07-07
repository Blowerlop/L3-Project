using DG.Tweening;
using TMPro;
using UnityEngine;

namespace Project
{
    public class PlaceholderLabel : MonoSingleton<PlaceholderLabel>
    {
        [SerializeField] private TextMeshProUGUI text;
        
        
        private Tween _tween;
        
        public void SetText(string value, float time = 0)
        {
            text.text = value;

            if (time <= 0) return;
            
            if (_tween.IsActive())
                _tween.Kill();
            
            _tween = DOVirtual.DelayedCall(time,() => text.text = "");
        }
    }
}
