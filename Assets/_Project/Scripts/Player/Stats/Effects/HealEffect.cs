using UnityEngine;

namespace Project.Effects
{
    public class HealEffect : Effect
    {
        public override EffectType Type => EffectType.Good;
        protected override bool AddToEffectableList => false;
       
        public int HealAmount;


        [Server]
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            var entity = effectable.AffectedEntity;
            
            entity.Heal(HealAmount);
            return true;
        }

        protected override void KillEffect_Internal() { }
        
        public override Effect GetInstance()
        {
            return this;
        }

        public override float GetEffectValue()
        {
            return HealAmount;
        }
    }
}