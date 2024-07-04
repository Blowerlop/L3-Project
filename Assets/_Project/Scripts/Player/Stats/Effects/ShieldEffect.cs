using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Effects
{
    public class ShieldEffect : Effect 
    {
        public override EffectType Type => EffectType.Good;
        protected override bool AddToEffectableList => true;
        
        public int ShieldAmount;
        public bool HasDuration;
        [ShowIf(nameof(HasDuration))] public float Duration;

        private int _shieldId;
        
        private Coroutine _appliedCoroutine;
        
        [Server]
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            var entity = effectable.AffectedEntity;
            
            _shieldId = entity.Shield(ShieldAmount);

            if (!HasDuration) return true;
            
            _appliedCoroutine = AffectedEffectable.AffectedEntity.StartCoroutine(
                Utilities.WaitForSecondsAndDoActionCoroutine(Duration, KillEffect));
            
            return true;
        }

        protected override void KillEffect_Internal()
        {
            if (!HasDuration) return;
            
            AffectedEffectable.AffectedEntity.UnShield(_shieldId);
            if (_appliedCoroutine != null) AffectedEffectable.AffectedEntity.StopCoroutine(_appliedCoroutine);
        }
        
        public override Effect GetInstance()
        {
            return new ShieldEffect()
            {
                ShieldAmount = ShieldAmount, 
                HasDuration = HasDuration, 
                Duration = Duration
            };
        }

        public override float GetEffectValue()
        {
            return ShieldAmount;
        }

        public override float GetEffectDuration()
        {
            return Duration; 
        }
    }
}