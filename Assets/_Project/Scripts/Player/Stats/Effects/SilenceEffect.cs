using UnityEngine;

namespace Project.Effects
{
    public class SilenceEffect : Effect
    {
        public override EffectType Type => EffectType.Bad;

        protected override bool AddToEffectableList => true;
        
        public float Duration;
        
        private Coroutine _appliedCoroutine;

        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            effectable.AffectedEntity.Silence();
            
            _appliedCoroutine = AffectedEffectable.AffectedEntity.StartCoroutine(
                Utilities.WaitForSecondsAndDoActionCoroutine(Duration, KillEffect));

            return true;
        }

        protected override void KillEffect_Internal()
        {
            AffectedEffectable.AffectedEntity.Unsilence();
            if (_appliedCoroutine != null) AffectedEffectable.AffectedEntity.StopCoroutine(_appliedCoroutine);
        }

        public override Effect GetInstance()
        {
            return new SilenceEffect() { Duration = Duration };
        }
        
        public override float GetEffectDuration()
        {
            return Duration; 
        }
    }
}