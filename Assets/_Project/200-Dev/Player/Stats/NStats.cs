using System;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project
{
    public abstract class NStat<TStat, T> : NetworkBehaviour where T : struct, IComparable, IFormattable, IConvertible where TStat : Stat<T>
    {
        [SerializeField] private PlayerStats _playerStats;
        private Stat<T> _stat;

        [SerializeField] private bool maxOnInit = true;
        
        public abstract NetworkVariable<T> _nValue { get; set; }
        public abstract NetworkVariable<T> _nMaxValue { get; set; }
        

        [ShowInInspector, ReadOnly] public virtual T Value
        {
            get
            {
                #if UNITY_EDITOR
                if (_nValue == null) return default;
                #endif
                
                return _nValue.Value;
            }
            set => _nValue.Value = Clamp(value, default, _nMaxValue.Value);
        }

        [ShowInInspector, ReadOnly] public T MaxValue
        {
            get
            {
                #if UNITY_EDITOR
                if (_nMaxValue == null) return default;
                #endif
                
                return _nMaxValue.Value;
            }
            set => _nMaxValue.Value = value;
        }

        [SerializeField] private bool _debug;

    
        public void Init()
        {
            _stat = _playerStats.Get<TStat>();
            
            _nValue.OnValueChanged += OnValueChanged_UpdateStatValues;
            _nMaxValue.OnValueChanged += OnValueChanged_UpdateStatValues;

            if (IsServer)
            {
                _nMaxValue.Value = _stat.maxValue;
                if (maxOnInit) SetToMaxValue();
            }
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();
        
            _nValue.OnValueChanged -= OnValueChanged_UpdateStatValues;
            _nMaxValue.OnValueChanged -= OnValueChanged_UpdateStatValues;
        }
    
        private void OnValueChanged_UpdateStatValues(T _, T __)
        {
            /*if (_debug) */
            Debug.Log($"[Player {OwnerClientId}] Stat value changed : {_nValue.Value} / Max {_nMaxValue.Value} for {GetType().Name}");

            _stat.value = _nValue.Value;
            _stat.maxValue = _nMaxValue.Value;
            // OnValueChanged?.Invoke(_nValue.Value, _nMaxValue.Value);
        }

        public void SetToMaxValue()
        {
            Debug.Log($"[Player {OwnerClientId}] Setting to max value {MaxValue} for {GetType().Name}");
            Value = MaxValue;
        }

        public void SetToMinValue() => Value = default;
        
        public object Clone()
        {
            return this;
        }

        public abstract T Clamp(T current, T min, T max);


        [Button]
        private void SetValue(T value)
        {
            Value = value;
        }
    }
}