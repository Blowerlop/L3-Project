using System;
using Sirenix.OdinInspector;
using Unity.Netcode;

namespace _Project._200_Dev.Entities.Player.Stats
{
    public class NetworkHealth : NStat<HealthStat, int>
    {
        [ShowInInspector] public override NetworkVariable<int> _nValue { get; set; } = new(1);
        [ShowInInspector] public override NetworkVariable<int> _nMaxValue { get; set; } = new(1);
        
        
        public override int Clamp(int current, int min, int max)
        {
            if (min > max || max < min) throw new InvalidOperationException();

            if (current <= min) return min;
            if (current >= max) return max;

            return current;
        }
    }
}