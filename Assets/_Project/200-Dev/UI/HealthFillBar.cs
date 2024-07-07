using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.UI;

namespace Project
{
    public class HealthFillBar : PlayerStatFillBar<HealthStat>
    {
        [SerializeField] private bool useHealthTicks;
        [SerializeField, ShowIf(nameof(useHealthTicks))] private RawImage tickBar;
        [SerializeField, ShowIf(nameof(useHealthTicks))] private int healthPerUVCoord;
        
        private int _lastMaxValue;
        
        protected override void OnValueChanged(int currentValue, int maxValue)
        {
            base.OnValueChanged(currentValue, maxValue);

            if (!useHealthTicks) return;
            if (_lastMaxValue == maxValue) return;
            
            float normalized = (float)maxValue / healthPerUVCoord;
            tickBar.uvRect = new Rect(0, 0, normalized, 1);
            
            _lastMaxValue = maxValue;
        }
    }
}