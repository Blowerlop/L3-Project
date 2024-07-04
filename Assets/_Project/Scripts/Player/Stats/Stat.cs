using System;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project
{
    [Serializable]
    public abstract class StatBase : ICloneable
    {
        public abstract object Clone();

        public abstract void SetToDefaultValue();
    }
    
    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T">Value type: int, float, long, etc...</typeparam>
    [Serializable]
    public abstract class Stat<T> : StatBase where T : struct, IComparable, IFormattable, IConvertible
    {
        [SerializeField, OnValueChanged("SetValue"), MinValue("_minValue"), MaxValue("_maxValue")] private T _defaultValue;
        
        [SerializeField, ReadOnly] private T _value;
        public T value
        {
            get => _value;
            set
            {
                // Return if the value is equal to the current value
                if (_value.CompareTo(value) == 0) return;

                _value = Clamp(value, _minValue, maxValue);
                
                Debug.Log("value changed to: " + _value + " for " + GetType().Name);
                OnValueChanged?.Invoke(_value, _maxValue);
            }
        }

        [SerializeField] private T _minValue;
        
        [SerializeField] private T _maxValue;
        public T maxValue
        {
            get => _maxValue;
            set
            {
                if (_maxValue.CompareTo(value) == 0) return;
                
                _maxValue = value;

                OnValueChanged?.Invoke(_value, _maxValue);
            }
        }

        // Value / MaxValue
        public event Action<T, T> OnValueChanged;
        

        public void SetToMaxValue()
        {
            value = maxValue;
        }

        public override object Clone()
        {
            return (Stat<T>)MemberwiseClone();
        }

        [Button]
        private void SetValue(T val)
        {
            value = val;
        }

        public T Clamp(T val, T min, T max)
        {
            if (min.CompareTo(max) == 1 || max.CompareTo(min) == -1) throw new InvalidOperationException();

            if (val.CompareTo(min) == -1) return min;
            if (val.CompareTo(max) == 1) return max;

            return val;
        }

        public override void SetToDefaultValue()
        {
            value = _defaultValue;
        }
    }
}