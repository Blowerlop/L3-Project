using _Project._200_Dev.Entities.Player.Interfaces;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player.Effects
{
    public class StunEffect : Effect 
    {
        public float Duration;

        public override EffectType Type => EffectType.Bad;
        protected override bool AddToEffectableList => true;

        private Entity _affectedEntity;
        private Coroutine _appliedCoroutine;
        
        [Server]
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            _affectedEntity = effectable.AffectedEntity;
            
            _affectedEntity.Stun();
            
            AffectedEffectable.AffectedEntity.StartCoroutine(
                Utilities.Utilities.WaitForSecondsAndDoActionCoroutine(Duration, KillEffect));
            
            return true;
        }

        protected override void KillEffect_Internal()
        {
            _affectedEntity.UnStun();
            if (_appliedCoroutine != null) AffectedEffectable.AffectedEntity.StopCoroutine(_appliedCoroutine);
        }
        
        public override Effect GetInstance()
        {
            return new StunEffect()
            {
                Duration = Duration
            };
        }

        public override float GetEffectDuration()
        {
            return Duration;
        }
    }
}