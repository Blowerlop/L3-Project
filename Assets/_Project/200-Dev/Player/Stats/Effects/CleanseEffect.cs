using UnityEngine;

namespace Project.Effects
{
    public class CleanseEffect : Effect
    {
        public override EffectType Type => EffectType.Good;

        protected override bool AddToEffectableList => false;

        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            effectable.SrvCleanse();
            return true;
        }

        protected override void KillEffect_Internal() { }
        
        public override Effect GetInstance()
        {
            return this;
        }
    }
}