using UnityEngine;

namespace Project.Effects
{
    public class SlowEffect : Effect 
    {
        public int SlowAmount;
        public float Duration;

        public override EffectType Type => SlowAmount < 0 ? EffectType.Good : EffectType.Bad;
        protected override bool AddToEffectableList => true;
        
        private MoveSpeedStat _stat;
        private Coroutine _appliedCoroutine;
        
        private int _slowedValue;
        
        [Server]
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            var entity = effectable.AffectedEntity;
            
            if (!entity.Stats.TryGet(out _stat))
            {
                Debug.LogWarning(
                    $"Can't apply SlowEffect on Entity {entity.data.name} because it doesn't have a MoveSpeedStat");
                return false;
            }
            
            _slowedValue = _stat.Slow(SlowAmount);

            _appliedCoroutine = AffectedEffectable.AffectedEntity.StartCoroutine(
                Utilities.WaitForSecondsAndDoActionCoroutine(Duration, KillEffect));
            
            return true;
        }

        protected override void KillEffect_Internal()
        {
            _stat.value += _slowedValue;
            if (_appliedCoroutine != null) AffectedEffectable.AffectedEntity.StopCoroutine(_appliedCoroutine);
        }
        
        public override Effect GetInstance()
        {
            return new SlowEffect()
            {
                SlowAmount = SlowAmount, 
                Duration = Duration
            };
        }

        public override float GetEffectValue()
        {
            return SlowAmount;
        }

        public override float GetEffectDuration()
        {
            return Duration;
        }
    }
}