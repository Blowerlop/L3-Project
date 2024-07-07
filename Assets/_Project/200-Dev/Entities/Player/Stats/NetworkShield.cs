using System;
using System.Collections.Generic;
using Sirenix.OdinInspector;
using Unity.Netcode;

namespace _Project._200_Dev.Entities.Player.Stats
{
    public class ShieldData
    {
        private static int _currentId;
        
        public ShieldData(int amount)
        {
            ID = _currentId++;
            Amount = amount;
        }
        
        public readonly int ID;
        public int Amount;
    }
    
    public class NetworkShield : NStat<ShieldStat, int>
    {
        [ShowInInspector] public override NetworkVariable<int> _nValue { get; set; } = new(1);
        [ShowInInspector] public override NetworkVariable<int> _nMaxValue { get; set; } = new(1);
        
        private List<ShieldData> _appliedShields = new();
        
        public bool HasShield => Value > 0;
        
        [ShowInInspector, ReadOnly] public override int Value
        {
            get => base.Value;
            set
            {
                var diff =  value - _nValue.Value;
                _nValue.Value = Clamp(value, default, _nMaxValue.Value);

                if (diff < 0)
                {
                    if (_appliedShields.Count == 0) return;
                    
                    var shield = _appliedShields[0];
                        
                    shield.Amount += diff;
                    if (shield.Amount <= 0)
                    {
                        _appliedShields.Remove(shield);
                    }
                }
            }
        }

        public override int Clamp(int current, int min, int max)
        {
            if (min > max || max < min) throw new InvalidOperationException();

            if (current <= min) return min;
            if (current >= max) return max;

            return current;
        }

        public void AddShield(ShieldData data)
        {
            _nValue.Value += data.Amount;
            _appliedShields.Add(data);
        }

        public void RemoveShield(int id)
        {
            var shield = _appliedShields.Find(s => s != null && s.ID == id);
            if (shield == null) return;
            
            _nValue.Value -= shield.Amount;
            _appliedShields.Remove(shield);
        }
    }
}